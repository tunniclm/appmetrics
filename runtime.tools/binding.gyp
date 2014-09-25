{
  "variables": {
    "srcdir%": "./src/ibmras",
    "pahodir%": "../Paho/org.eclipse.paho.mqtt.c",
    "pahosrcdir%": "../Paho/org.eclipse.paho.mqtt.c/src",
  },
  "conditions": [
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
    "defines": [ "EXPORT", "IBMRAS_DEBUG_LOGGING" ],
    "include_dirs": [ "src", "<(pahosrcdir)" ],
    "conditions": [
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
        "<(srcdir)/common/port/<(portdir)/Thread.cpp",
        "<(srcdir)/common/port/<(portdir)/Process.cpp",
        "<(srcdir)/common/port/ThreadData.cpp",
        "<(srcdir)/common/Properties.cpp",
        "<(srcdir)/common/PropertiesFile.cpp",
        "<(srcdir)/common/data/legacy/LegacyData.cpp",
        "<(srcdir)/common/util/strUtils.cpp",
        "<(srcdir)/common/util/sysUtils.cpp",
        "<(srcdir)/monitoring/agent/Agent.cpp",
        "<(srcdir)/monitoring/agent/MonitorDataQueue.cpp",
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
      ],
    },
    {
      "target_name": "nodecon",
      "type": "static_library",
      "sources": [
        "<(srcdir)/vm/node/nodeconnector.cpp",
      ],
      "dependencies": [ "libhealthcenter" ],
    },
    {
      "target_name": "<(SHARED_LIB_PREFIX)nodeenvplugin",
      "type": "shared_library",
      "sources": [
        "<(srcdir)/monitoring/plugins/nodeenv/nodeenvplugin.cpp",
      ],
      "dependencies": [ "libhealthcenter" ],
    },
    {
      "target_name": "<(SHARED_LIB_PREFIX)nodegcplugin",
      "type": "shared_library",
      "sources": [
        "<(srcdir)/monitoring/plugins/nodegc/nodegcplugin.cpp",
      ],
      "dependencies": [ "libhealthcenter" ],
    },
    {
      "target_name": "<(SHARED_LIB_PREFIX)osplugin",
      "type": "shared_library",
      "sources": [
        "<(srcdir)/monitoring/plugins/os/Plugin.cpp",
      ],
      "dependencies": [ "libhealthcenter" ],
      "conditions": [
        ['OS=="linux"', {
          "sources": [ "<(srcdir)/monitoring/plugins/os/Linux.cpp" ],
        }],
        ['OS=="win"', {
          "sources": [ "<(srcdir)/monitoring/plugins/os/Windows.cpp" ],
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
    },

    {
      "target_name": "healthcenter",
      
      "include_dirs": [ "<(srcdir)/vm/node" ],
      "sources": [ 
        "<(srcdir)/vm/node/nodeagent.cpp", 
        "<(srcdir)/vm/node/wrapper.cpp"
      ],
      "dependencies": [ "libhealthcenter", "nodecon" ],
    },
    {
      "target_name": "install",
      "type": "none",
      "dependencies": [
        "libhealthcenter",
        "healthcenter",
        "<(SHARED_LIB_PREFIX)osplugin",
        "<(SHARED_LIB_PREFIX)nodeenvplugin",
        "<(SHARED_LIB_PREFIX)nodegcplugin",
     ],
      "copies": [
        {
          "destination": "<(PRODUCT_DIR)/deploy",
          "files": [
            "<(PRODUCT_DIR)/healthcenter.node",
            "<(PRODUCT_DIR)/libhealthcenter<(SHARED_LIB_SUFFIX)",
            "<(srcdir)/monitoring/monitoringagent.properties",
          ],
        },
        {
          "destination": "<(PRODUCT_DIR)/deploy/plugins",
          "files": [
            "<(PRODUCT_DIR)/<(SHARED_LIB_PREFIX)osplugin<(SHARED_LIB_SUFFIX)",
            "<(PRODUCT_DIR)/<(SHARED_LIB_PREFIX)nodeenvplugin<(SHARED_LIB_SUFFIX)",
            "<(PRODUCT_DIR)/<(SHARED_LIB_PREFIX)nodegcplugin<(SHARED_LIB_SUFFIX)",
          ],
        },
      ],
    },
  ],
}
