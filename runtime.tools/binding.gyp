{
  "variables": {
    "srcdir%": "./src/ibmras",
    "pahodir%": "../Paho/org.eclipse.paho.mqtt.c",
    "pahosrcdir%": "../Paho/org.eclipse.paho.mqtt.c/src",
    "deploydir%": "<(PRODUCT_DIR)/deploy/healthcenter",
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
#        "<(srcdir)/common/data/legacy/LegacyData.cpp",
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
    },
    {
      "target_name": "nodeenvplugin",
      "type": "shared_library",
      "sources": [
        "<(srcdir)/monitoring/plugins/node/env/nodeenvplugin.cpp",
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
      "target_name": "nodeprofplugin",
      "type": "shared_library",
      "sources": [
        "<(srcdir)/monitoring/plugins/node/prof/nodeprofplugin.cpp",
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
      "target_name": "nodegcplugin",
      "type": "shared_library",
      "sources": [
        "<(srcdir)/monitoring/plugins/node/gc/nodegcplugin.cpp",
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
      "target_name": "cpuplugin",
      "type": "shared_library",
      "sources": [
        "<(srcdir)/monitoring/plugins/common/cpu/cpuplugin.cpp",
        "<(srcdir)/monitoring/plugins/common/cpu/cputime.cpp",
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
        "<(srcdir)/monitoring/plugins/common/memory/memoryplugin.cpp",
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
      "target_name": "install",
      "type": "none",
      "dependencies": [
        "healthcenter",
        "hcmqtt",
        "cpuplugin",
        "envplugin",
        "nodeenvplugin",
        "nodegcplugin",
        "nodeprofplugin",
        "memoryplugin",
     ],
      "copies": [
        {
          "destination": "<(deploydir)/bin",
          "files": [
            "<(srcdir)/vm/node/bin/healthcenter-cli.js",
          ],
        },
        {
          "destination": "<(deploydir)",
          "files": [
            "<(PRODUCT_DIR)/healthcenter.node",
            "./src/properties/node/healthcenter.properties",
            "<(srcdir)/vm/node/index.js",
            "<(srcdir)/vm/node/launcher.js",
            "<(srcdir)/vm/node/package.json",
          ],
        },
        {
          "destination": "<(deploydir)/plugins",
          "files": [
            "<(PRODUCT_DIR)/<(SHARED_LIB_PREFIX)hcmqtt<(SHARED_LIB_SUFFIX)",
            "<(PRODUCT_DIR)/<(SHARED_LIB_PREFIX)cpuplugin<(SHARED_LIB_SUFFIX)",
            "<(PRODUCT_DIR)/<(SHARED_LIB_PREFIX)envplugin<(SHARED_LIB_SUFFIX)",
            "<(PRODUCT_DIR)/<(SHARED_LIB_PREFIX)nodeenvplugin<(SHARED_LIB_SUFFIX)",
            "<(PRODUCT_DIR)/<(SHARED_LIB_PREFIX)nodegcplugin<(SHARED_LIB_SUFFIX)",
            "<(PRODUCT_DIR)/<(SHARED_LIB_PREFIX)nodeprofplugin<(SHARED_LIB_SUFFIX)",
            "<(PRODUCT_DIR)/<(SHARED_LIB_PREFIX)memoryplugin<(SHARED_LIB_SUFFIX)",
          ],
        },
      ],
    },
  ],
}

