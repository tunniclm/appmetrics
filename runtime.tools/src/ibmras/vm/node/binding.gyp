{
  "targets": [
    {
      "target_name": "healthcenter",
      "include_dirs": [ "../../src"],
      "sources": [ "nodeagent.cpp", "wrapper.cpp" ],
      "libraries": ["-lnodecon", "-lhealthcenter" ],

      "conditions": [
        ['OS=="linux"', {
          "defines": [ "_LINUX" ],
           "libraries": [
             "-L..",
      	   ]
        }],
        ['OS=="aix"', {
          "defines": [ "_AIX" ],
           "libraries": [
             "-L..",
      	   ]
        }],
        ['OS=="win"', {
          "defines": [ "_WINDOWS" ],
          "libraries": [ "Ws2_32" ],
          "msvs_settings": {
            "VCCLCompilerTool": {
              "AdditionalOptions": [
                "/EHsc",
                "/MD",
              ]
            },
            "VCLinkerTool": {
              "AdditionalLibraryDirectories": [
                "..", 
              ],
            },
          },
        }],        
      ],
    },
  ],
}

