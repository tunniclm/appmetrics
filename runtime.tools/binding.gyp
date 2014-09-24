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
      "target_name": "healthcenter",
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
      "dependencies": [ "healthcenter" ],
    },
    {
      "target_name": "nodegcplugin",
      "type": "shared_library",
      "sources": [
        "<(srcdir)/monitoring/plugins/nodegc/nodegcplugin.cpp",
      ],
      "dependencies": [ "healthcenter" ],
    },
    {
      "target_name": "ostream",
      "type": "shared_library",
      "sources": [
        "<(srcdir)/monitoring/connector/ostream/OStreamConnector.cpp",
      ],
      "dependencies": [ "healthcenter" ],
    },

    {
      "target_name": "node-healthcenter",
      
      "include_dirs": [ "<(srcdir)/vm/node" ],
      "sources": [ 
        "<(srcdir)/vm/node/nodeagent.cpp", 
        "<(srcdir)/vm/node/wrapper.cpp"
      ],
      "dependencies": [ "healthcenter", "nodecon" ],
    },
  ],
}

