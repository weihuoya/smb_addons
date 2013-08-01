{
  "targets": [
    {
      "target_name": "smb",
      
      "include_dirs": [
        "ICTCLAS50",
        "boost_1_49_0",
        "mongo-cxx-driver\\2.4\\src"
      ],

      "msvs_settings": {
        "VCLinkerTool": {
          "AdditionalLibraryDirectories": [
            "ICTCLAS50",
            "boost_1_49_0\\stage",
            "mongo-cxx-driver\\2.4\\src"
          ]
        }
      },
      
      "sources": [
        "smb.cpp",
        "analyze.cpp",
        "query.cpp",
        "config.cpp",
        "weibo.cpp",
        "vsm.cpp",
        "singleton.cpp",
        "segment.cpp",
        "utf8.c"
      ]
    }
  ]
}