{
  "variables": {
    "srcdir%": "./src/ibmras",
    "opensourcedir": "./src/opensource/node/appmetrics",
    "licensesdir": "./src/ibmras/vm/node/package/external/licenses",
    "pahodir%": "../Paho/org.eclipse.paho.mqtt.c",
    "pahosrcdir%": "../Paho/org.eclipse.paho.mqtt.c/src",
    "internaldeploydir%": "<(PRODUCT_DIR)/deploy/internal/healthcenter",
    "externaldeploydir%": "<(PRODUCT_DIR)/deploy/external/healthcenter",
    "externalbinariesdir%": "<(PRODUCT_DIR)/deploy/external/binaries",
    'build_id%': '<!(["python", "./src/ibmras/vm/node/generate_build_id.py"])',
    'version%': '<!(["python", "./src/ibmras/vm/node/get_from_json.py", "./src/ibmras/vm/node/package.json", "version"])',
  },
  "conditions": [
    ['OS=="aix"', {
      "variables": {
        "portdir%": "aix",
        "SHARED_LIB_SUFFIX": ".a",
      },
    }],
    ['OS=="linux"', {
      "variables": {
        "portdir%": "linux"
      },
    }],
    ['OS=="win"', {
      "variables": {
        "portdir%": "windows"
      },
    }]
  ],

  "target_defaults": {
    "cflags_cc!": [ '-fno-exceptions' ],
    "defines": [ "EXPORT", "IBMRAS_DEBUG_LOGGING" ],
    "include_dirs": [ "src", "<(pahosrcdir)", "src/ibmras/vm/node/node_modules/nan" ],
    "target_conditions": [
      ['_type=="shared_library"', {
        'product_prefix': '<(SHARED_LIB_PREFIX)',
        "conditions": [
          ['OS=="aix"', {
            'product_extension': 'a',
          },{
          }],
        ],
      }],
    ],
    "conditions": [
      ['OS=="aix"', {
        "defines": [ "_AIX", "AIX" ],
        "libraries": [ "-Wl,-bexpall,-brtllib,-G,-bernotok,-brtl" ],
      }],
      ['OS=="linux"', {
        "defines": [ "_LINUX", "LINUX" ],
      }],
      ['OS=="win"', {
        "defines": [ "_WINDOWS", "WINDOWS"  ],
        "libraries": [ "Ws2_32" ],
        "msvs_settings": {
          "VCCLCompilerTool": {
            "AdditionalOptions": [
              "/EHsc",
              "/MD",
            ]
          },
        },
      }]
    ],
  },

  "targets": [
    {
      "target_name": "healthcenter",
      "include_dirs": [ "<(srcdir)/vm/node" ],
      "sources": [ 
        "<(srcdir)/common/Logger.cpp",
        "<(srcdir)/common/LogManager.cpp",
        "<(srcdir)/common/MemoryManager.cpp",
        "<(srcdir)/common/util/FileUtils.cpp",
        "<(srcdir)/common/util/LibraryUtils.cpp",
        "<(srcdir)/common/port/<(portdir)/Thread.cpp",
        "<(srcdir)/common/port/<(portdir)/Process.cpp",
        "<(srcdir)/common/port/Lock.cpp",
        "<(srcdir)/common/port/ThreadData.cpp",
        "<(srcdir)/common/Properties.cpp",
        "<(srcdir)/common/PropertiesFile.cpp",
        "<(srcdir)/common/util/strUtils.cpp",
        "<(srcdir)/common/util/sysUtils.cpp",
        "<(INTERMEDIATE_DIR)/monitoring/agent/Agent.cpp",
        "<(srcdir)/monitoring/agent/threads/ThreadPool.cpp",
        "<(srcdir)/monitoring/agent/threads/WorkerThread.cpp",
        "<(srcdir)/monitoring/agent/SystemReceiver.cpp",
        "<(srcdir)/monitoring/connector/ConnectorManager.cpp",
        "<(srcdir)/monitoring/agent/Bucket.cpp",
        "<(srcdir)/monitoring/agent/BucketList.cpp",
        "<(srcdir)/monitoring/Plugin.cpp",
        "<(srcdir)/monitoring/connector/configuration/ConfigurationConnector.cpp",
        "<(srcdir)/vm/node/nodeagent.cpp"
      ],
      'variables': {
        'agentversion%':'<(version).<(build_id)'
      },
      'actions': [{
        'action_name': 'Set version',
        'inputs': [ "<(srcdir)/monitoring/agent/Agent.cpp" ],
        'outputs': [ "<(INTERMEDIATE_DIR)/monitoring/agent/Agent.cpp" ],
        'action': [
          'python',
          '<(srcdir)/vm/node/replace_in_file.py',
          '<(srcdir)/monitoring/agent/Agent.cpp',
          '<(INTERMEDIATE_DIR)/monitoring/agent/Agent.cpp',
          '--from="99\.99\.99\.29991231"',
          '--to="<(agentversion)"',
          '-v'
         ],
      }],
    },
    {
      "target_name": "hcmqtt",
      "type": "shared_library",
      "sources": [
        "<(pahosrcdir)/Clients.c",
        "<(pahosrcdir)/Heap.c",
        "<(pahosrcdir)/LinkedList.c",
        "<(pahosrcdir)/Log.c",
        "<(pahosrcdir)/Messages.c",
        "<(pahosrcdir)/MQTTAsync.c",
        "<(pahosrcdir)/MQTTPacket.c",
        "<(pahosrcdir)/MQTTPacketOut.c",
        "<(pahosrcdir)/MQTTPersistence.c",
        "<(pahosrcdir)/MQTTPersistenceDefault.c",
        "<(pahosrcdir)/MQTTProtocolClient.c",
        "<(pahosrcdir)/MQTTProtocolOut.c",
        "<(pahosrcdir)/SocketBuffer.c",
        "<(pahosrcdir)/Socket.c",
        "<(pahosrcdir)/StackTrace.c",
        "<(pahosrcdir)/Thread.c",
        "<(pahosrcdir)/Tree.c",
        "<(pahosrcdir)/utf-8.c",
        "<(srcdir)/monitoring/connector/mqtt/MQTTConnector.cpp",
        "<(srcdir)/common/Logger.cpp",
        "<(srcdir)/common/LogManager.cpp",
        "<(srcdir)/common/port/<(portdir)/Process.cpp",
        "<(srcdir)/common/port/Lock.cpp",
        "<(srcdir)/common/Properties.cpp",
        "<(srcdir)/common/util/strUtils.cpp",
        "<(srcdir)/common/util/sysUtils.cpp",
        "<(srcdir)/common/MemoryManager.cpp",
      ],
      "conditions": [
        [ 'node_byteorder=="big"', {
          "defines": [ "REVERSED" ], 
        }],
      ],
    },
    {
      "target_name": "cpuplugin",
      "type": "shared_library",
      "sources": [
        "<(srcdir)/monitoring/plugins/common/cpu/cpuplugin.cpp",
      ],
      "conditions": [
        ['OS=="win"', {
          "libraries": [ "Pdh" ],
        }],
        ['OS=="aix"', {
          "libraries": [ "-lperfstat" ],
        }],
      ],
    },
    {
      "target_name": "memoryplugin",
      "type": "shared_library",
      "sources": [
        "<(srcdir)/monitoring/plugins/common/memory/MemoryPlugin.cpp",
      ],
      "conditions": [
        ['OS=="win"', {
          "libraries": [ "Psapi" ],
        }],
      ],
    },
    {
      "target_name": "envplugin",
      "type": "shared_library",
      "sources": [
        "<(srcdir)/monitoring/plugins/common/environment/envplugin.cpp",
      ],
    },
    {
      "target_name": "apiplugin",
      "type": "shared_library",
      "sources": [
        "<(srcdir)/monitoring/connector/api/APIConnector.cpp"
      ],
    },
    {
      "target_name": "ostream",
      "type": "shared_library",
      "sources": [
        "<(srcdir)/monitoring/connector/ostream/OStreamConnector.cpp",
        "<(srcdir)/common/Logger.cpp",
        "<(srcdir)/common/LogManager.cpp",
        "<(srcdir)/common/port/<(portdir)/Process.cpp",
        "<(srcdir)/common/port/Lock.cpp",
        "<(srcdir)/common/Properties.cpp",
        "<(srcdir)/common/util/strUtils.cpp",
        "<(srcdir)/common/util/sysUtils.cpp",
        "<(srcdir)/common/MemoryManager.cpp",
      ],
    },

	{
	  "target_name": "opensource",
	  "type": "none",
      "dependencies": [
        "<(opensourcedir)/binding.gyp:nodeenvplugin",
        "<(opensourcedir)/binding.gyp:nodegcplugin",
        "<(opensourcedir)/binding.gyp:nodeprofplugin",
      ],
    },
    {
      "target_name": "internal",
      "type": "none",
      "dependencies": [
        "healthcenter",
        "hcmqtt",
        "cpuplugin",
        "envplugin",
        "memoryplugin",
        "apiplugin",
        "opensource",
     ],
      "copies": [
        {
          "destination": "<(internaldeploydir)",
          "files": [
            "<(opensourcedir)/appmetrics-api.js",
            "<(opensourcedir)/healthcenter.properties",
            "<(opensourcedir)/index.js",
            "<(opensourcedir)/launcher.js",
            "<(srcdir)/vm/node/package/internal/package.json",
            "<(PRODUCT_DIR)/healthcenter.node",
          ],
        },
        {
          "destination": "<(internaldeploydir)/bin",
          "files": [
            "<(opensourcedir)/bin/appmetrics-cli.js",
          ],
        },
        {
          "destination": "<(internaldeploydir)/plugins",
          "files": [
            "<(PRODUCT_DIR)/<(SHARED_LIB_PREFIX)nodeenvplugin<(SHARED_LIB_SUFFIX)",
            "<(PRODUCT_DIR)/<(SHARED_LIB_PREFIX)nodegcplugin<(SHARED_LIB_SUFFIX)",
            "<(PRODUCT_DIR)/<(SHARED_LIB_PREFIX)nodeprofplugin<(SHARED_LIB_SUFFIX)",
            "<(PRODUCT_DIR)/<(SHARED_LIB_PREFIX)hcmqtt<(SHARED_LIB_SUFFIX)",
            "<(PRODUCT_DIR)/<(SHARED_LIB_PREFIX)cpuplugin<(SHARED_LIB_SUFFIX)",
            "<(PRODUCT_DIR)/<(SHARED_LIB_PREFIX)envplugin<(SHARED_LIB_SUFFIX)",
            "<(PRODUCT_DIR)/<(SHARED_LIB_PREFIX)memoryplugin<(SHARED_LIB_SUFFIX)",
            "<(PRODUCT_DIR)/<(SHARED_LIB_PREFIX)apiplugin<(SHARED_LIB_SUFFIX)",            
          ],
        },
      ],
    },

    {
      "target_name": "external",
      "type": "none",
      "dependencies": [
        "healthcenter",
        "hcmqtt",
        "cpuplugin",
        "envplugin",
        "memoryplugin",
        "apiplugin",
        "opensource",
      ],
      "copies": [
        {
          "destination": "<(externaldeploydir)",
          "files": [
            "<(opensourcedir)/appmetrics-api.js",
            "<(opensourcedir)/healthcenter.properties",
            "<(opensourcedir)/index.js",
            "<(opensourcedir)/launcher.js",
            "<(srcdir)/vm/node/package/external/package.json",
            "<(srcdir)/vm/node/package/external/download_all_binaries.js",
          ],
        },
        {
          "destination": "<(externaldeploydir)/bin",
          "files": [
            "<(opensourcedir)/bin/appmetrics-cli.js",
          ],
        },
        {
          "destination": "<(externaldeploydir)/licenses",
          "files": [
            "<(licensesdir)/LA_cs",
            "<(licensesdir)/LA_de",
            "<(licensesdir)/LA_el",
            "<(licensesdir)/LA_en",
            "<(licensesdir)/LA_es",
            "<(licensesdir)/LA_fr",
            "<(licensesdir)/LA_in",
            "<(licensesdir)/LA_it",
            "<(licensesdir)/LA_ja",
            "<(licensesdir)/LA_ko",
            "<(licensesdir)/LA_lt",
            "<(licensesdir)/LA_pl",
            "<(licensesdir)/LA_pt",
            "<(licensesdir)/LA_ru",
            "<(licensesdir)/LA_sl",
            "<(licensesdir)/LA_tr",
            "<(licensesdir)/LA_zh",
            "<(licensesdir)/LA_zh_TW",
            "<(licensesdir)/LI_cs",
            "<(licensesdir)/LI_de",
            "<(licensesdir)/LI_el",
            "<(licensesdir)/LI_en",
            "<(licensesdir)/LI_es",
            "<(licensesdir)/LI_fr",
            "<(licensesdir)/LI_in",
            "<(licensesdir)/LI_it",
            "<(licensesdir)/LI_ja",
            "<(licensesdir)/LI_ko",
            "<(licensesdir)/LI_lt",
            "<(licensesdir)/LI_pl",
            "<(licensesdir)/LI_pt",
            "<(licensesdir)/LI_ru",
            "<(licensesdir)/LI_sl",
            "<(licensesdir)/LI_tr",
            "<(licensesdir)/LI_zh",
            "<(licensesdir)/LI_zh_TW",
            "<(licensesdir)/notices",
          ],
        },
        {
          "destination": "<(externalbinariesdir)",
          "files": [
            "<(PRODUCT_DIR)/healthcenter.node",
          ],
        },
        {
          "destination": "<(externalbinariesdir)/plugins",
          "files": [
            "<(PRODUCT_DIR)/<(SHARED_LIB_PREFIX)nodeenvplugin<(SHARED_LIB_SUFFIX)",
            "<(PRODUCT_DIR)/<(SHARED_LIB_PREFIX)nodegcplugin<(SHARED_LIB_SUFFIX)",
            "<(PRODUCT_DIR)/<(SHARED_LIB_PREFIX)nodeprofplugin<(SHARED_LIB_SUFFIX)",
            "<(PRODUCT_DIR)/<(SHARED_LIB_PREFIX)hcmqtt<(SHARED_LIB_SUFFIX)",
            "<(PRODUCT_DIR)/<(SHARED_LIB_PREFIX)cpuplugin<(SHARED_LIB_SUFFIX)",
            "<(PRODUCT_DIR)/<(SHARED_LIB_PREFIX)envplugin<(SHARED_LIB_SUFFIX)",
            "<(PRODUCT_DIR)/<(SHARED_LIB_PREFIX)memoryplugin<(SHARED_LIB_SUFFIX)",
            "<(PRODUCT_DIR)/<(SHARED_LIB_PREFIX)apiplugin<(SHARED_LIB_SUFFIX)",            
          ],
        },
        {
          "destination": "<(externalbinariesdir)/licenses",
          "files": [
            "<(licensesdir)/LA_cs",
            "<(licensesdir)/LA_de",
            "<(licensesdir)/LA_el",
            "<(licensesdir)/LA_en",
            "<(licensesdir)/LA_es",
            "<(licensesdir)/LA_fr",
            "<(licensesdir)/LA_in",
            "<(licensesdir)/LA_it",
            "<(licensesdir)/LA_ja",
            "<(licensesdir)/LA_ko",
            "<(licensesdir)/LA_lt",
            "<(licensesdir)/LA_pl",
            "<(licensesdir)/LA_pt",
            "<(licensesdir)/LA_ru",
            "<(licensesdir)/LA_sl",
            "<(licensesdir)/LA_tr",
            "<(licensesdir)/LA_zh",
            "<(licensesdir)/LA_zh_TW",
            "<(licensesdir)/LI_cs",
            "<(licensesdir)/LI_de",
            "<(licensesdir)/LI_el",
            "<(licensesdir)/LI_en",
            "<(licensesdir)/LI_es",
            "<(licensesdir)/LI_fr",
            "<(licensesdir)/LI_in",
            "<(licensesdir)/LI_it",
            "<(licensesdir)/LI_ja",
            "<(licensesdir)/LI_ko",
            "<(licensesdir)/LI_lt",
            "<(licensesdir)/LI_pl",
            "<(licensesdir)/LI_pt",
            "<(licensesdir)/LI_ru",
            "<(licensesdir)/LI_sl",
            "<(licensesdir)/LI_tr",
            "<(licensesdir)/LI_zh",
            "<(licensesdir)/LI_zh_TW",
            "<(licensesdir)/notices",
          ],
        },
      ],
    },
        
  ],
}

