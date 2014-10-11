{
  "variables": {
    "srcdir%": "./src/ibmras",
    "pahodir%": "../Paho/org.eclipse.paho.mqtt.c",
    "pahosrcdir%": "../Paho/org.eclipse.paho.mqtt.c/src",
    "deploydir%": "<(PRODUCT_DIR)/deploy/healthcenter",
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
    "include_dirs": [ "src", "<(pahosrcdir)" ],
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
        "libraries": [ "-Wl,-rpath=\$$ORIGIN" ],
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
      # Hardcode the "lib" prefix so this target doesn't clash with the "healthcenter" target on Windows
      "target_name": "libhealthcenter",
      "type": "shared_library",
      "sources": [ 
        "<(srcdir)/common/Logger.cpp",
        "<(srcdir)/common/LogManager.cpp",
        "<(srcdir)/common/util/FileUtils.cpp",
        "<(srcdir)/common/util/LibraryUtils.cpp",
        "<(srcdir)/common/port/<(portdir)/Thread.cpp",
        "<(srcdir)/common/port/<(portdir)/Process.cpp",
        "<(srcdir)/common/port/ThreadData.cpp",
        "<(srcdir)/common/Properties.cpp",
        "<(srcdir)/common/PropertiesFile.cpp",
        "<(srcdir)/common/data/legacy/LegacyData.cpp",
        "<(srcdir)/common/util/strUtils.cpp",
        "<(srcdir)/common/util/sysUtils.cpp",
        "<(srcdir)/monitoring/agent/Agent.cpp",
        "<(srcdir)/monitoring/agent/PullSourceCounter.cpp",
        "<(srcdir)/monitoring/agent/threads/ThreadPool.cpp",
        "<(srcdir)/monitoring/agent/threads/WorkerThread.cpp",
        "<(srcdir)/monitoring/agent/threads/WorkerThreadControl.cpp",
        "<(srcdir)/monitoring/agent/SystemReceiver.cpp",
        "<(srcdir)/monitoring/connector/ConnectorManager.cpp",
        "<(srcdir)/monitoring/agent/Bucket.cpp",
        "<(srcdir)/monitoring/agent/BucketList.cpp",
        "<(srcdir)/monitoring/agent/BucketDataQueueEntry.cpp",
        "<(srcdir)/monitoring/Plugin.cpp",
        "<(srcdir)/monitoring/connector/configuration/ConfigurationConnector.cpp",
      ],
    },
    {
      # This target is a workaround for AIX to prevent the value of <(PRODUCT_DIR) being written into the linked binaries
      "target_name": "libhealthcenter-aix",
      "type": "none",
      "dependencies": [ "libhealthcenter" ],
      "dependencies_traverse": 0,
      "direct_dependent_settings": {
        "libraries": [ "-L<(PRODUCT_DIR)", "-lhealthcenter" ],
      },
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
      ],
      "dependencies": [ "libhealthcenter" ],
      "conditions": [
        [ 'OS=="aix"', {
          "dependencies!": [ "libhealthcenter" ],
          "dependencies": [ "libhealthcenter-aix" ],
        }],      
        [ 'node_byteorder=="big"', {
          "defines": [ "REVERSED" ], 
        }],
      ],
    },
    {
      "target_name": "nodeenvplugin",
      "type": "shared_library",
      "sources": [
        "<(srcdir)/monitoring/plugins/nodeenv/nodeenvplugin.cpp",
      ],
      "dependencies": [ "libhealthcenter" ],
      "conditions": [
        [ 'OS=="aix"', {
          "dependencies!": [ "libhealthcenter" ],
          "dependencies": [ "libhealthcenter-aix" ],
        }],
      ],
    },
    {
      "target_name": "nodegcplugin",
      "type": "shared_library",
      "sources": [
        "<(srcdir)/monitoring/plugins/nodegc/nodegcplugin.cpp",
      ],
      "dependencies": [ "libhealthcenter" ],
      "conditions": [
        [ 'OS=="aix"', {
          "dependencies!": [ "libhealthcenter" ],
          "dependencies": [ "libhealthcenter-aix" ],
        }],
      ],
    },
    {
      "target_name": "cpuplugin",
      "type": "shared_library",
      "sources": [
        "<(srcdir)/monitoring/plugins/cpu/cpuplugin.cpp",
        "<(srcdir)/monitoring/plugins/cpu/cputime.cpp",
      ],
      "dependencies": [ "libhealthcenter" ],
      "conditions": [
        ['OS=="win"', {
          "libraries": [ "Pdh" ],
        }],
        ['OS=="aix"', {
          "libraries": [ "-lperfstat" ],
          "dependencies!": [ "libhealthcenter" ],
          "dependencies": [ "libhealthcenter-aix" ],
        }],
      ],
    },
    {
      "target_name": "envplugin",
      "type": "shared_library",
      "sources": [
        "<(srcdir)/monitoring/plugins/environment/envplugin.cpp",
      ],
      "dependencies": [ "libhealthcenter" ],
      "conditions": [
        [ 'OS=="aix"', {
          "dependencies!": [ "libhealthcenter" ],
          "dependencies": [ "libhealthcenter-aix" ],
        }],
      ],
    },
    {
      "target_name": "ostream",
      "type": "shared_library",
      "sources": [
        "<(srcdir)/monitoring/connector/ostream/OStreamConnector.cpp",
      ],
      "dependencies": [ "libhealthcenter" ],
      "conditions": [
        [ 'OS=="aix"', {
          "dependencies!": [ "libhealthcenter" ],
          "dependencies": [ "libhealthcenter-aix" ],
        }],
      ],
    },

    {
      "target_name": "healthcenter",
      
      "include_dirs": [ "<(srcdir)/vm/node" ],
      "sources": [ 
        "<(srcdir)/vm/node/nodeagent.cpp", 
        "<(srcdir)/vm/node/wrapper.cpp"
      ],
      "dependencies": [ "libhealthcenter" ],
      "conditions": [
        [ 'OS=="aix"', {
          "dependencies!": [ "libhealthcenter" ],
          "dependencies": [ "libhealthcenter-aix" ],
        }],
      ],
    },
    {
      "target_name": "install",
      "type": "none",
      "dependencies": [
        "libhealthcenter",
        "healthcenter",
        "hcmqtt",
        "cpuplugin",
        "envplugin",
        "nodeenvplugin",
        "nodegcplugin",
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
            "<(PRODUCT_DIR)/libhealthcenter<(SHARED_LIB_SUFFIX)",
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
          ],
        },
      ],
    },
  ],
}

