{
  "targets": [
    {
      "target_name": "ezito-async",
      "sources": [ "src/main.cc" ],
      "include_dirs" : [
        "src/*"
      ],
      "xcode_settings": {
        "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
        "CLANG_CXX_LIBRARY": "libc++",
        "MACOSX_DEPLOYMENT_TARGET": "10.7",
      },
    }
  ]
}