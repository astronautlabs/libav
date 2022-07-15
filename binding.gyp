{
  "targets": [{
    "target_name" : "libav",
    'sources' : [ 
      "native/avutil/avutil.cpp",
      "native/avutil/buffer.cpp",
      "native/avutil/dict.cpp",
      "native/avutil/index.cpp",
      "native/libav.cpp"
    ],
    'cflags!': [ '-fno-exceptions' ],
    'cflags_cc!': [ '-fno-exceptions' ],
    'xcode_settings': {
      'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
      'CLANG_CXX_LIBRARY': 'libc++',
      'MACOSX_DEPLOYMENT_TARGET': '10.7',
      'GCC_SYMBOLS_PRIVATE_EXTERN': 'YES', # -fvisibility=hidden
      'GCC_ENABLE_CPP_RTTI': 'YES',
      'MACOSX_DEPLOYMENT_TARGET': '10.7',
      'OTHER_CPLUSPLUSFLAGS': [
        '-std=c++11',
        '-stdlib=libc++'
      ]
    },
    "include_dirs" : [
      "<!(node -p \"require('node-addon-api').include_dir\")",
      "D:/dev/ffmpeg/ffmpeg"
    ],
    'dependencies': [
        '<!(node -p \"require(\'node-addon-api\').gyp\")'
    ],
    'defines': [
      '__STDC_CONSTANT_MACROS'
    ],
    "conditions": [
      ['OS=="win"', {
        "configurations": {
          "Release": {
            "msvs_settings": {
              "VCCLCompilerTool": {
                "RuntimeTypeInfo": "true",
                "ExceptionHandling": 1
              }
            }
          },
          "Debug": {
            "msvs_settings": {
              "VCCLCompilerTool": {
                "RuntimeTypeInfo": "true",
                "ExceptionHandling": 1
              }
            }
          }
        },
        "libraries": [
            "-lD:/dev/ffmpeg/ffmpeg/libavutil/avutil",
            "-lD:/dev/ffmpeg/ffmpeg/libavformat/avformat",
            "-lD:/dev/ffmpeg/ffmpeg/libavcodec/avcodec",
            "-lD:/dev/ffmpeg/ffmpeg/libavdevice/avdevice",
            "-lD:/dev/ffmpeg/ffmpeg/libavfilter/avfilter"
        ],
        "copies": [
            {
              "destination": "build/Release/",
              "files": [
                "D:/dev/ffmpeg/ffmpeg/libavutil/avutil-57.dll",
                "D:/dev/ffmpeg/ffmpeg/libavformat/avformat-59.dll",
                "D:/dev/ffmpeg/ffmpeg/libavcodec/avcodec-59.dll",
                "D:/dev/ffmpeg/ffmpeg/libavdevice/avdevice-59.dll",
                "D:/dev/ffmpeg/ffmpeg/libavfilter/avfilter-8.dll"
              ]
            },
            {
              "destination": "build/Debug/",
              "files": [
                "D:/dev/ffmpeg/ffmpeg/libavutil/avutil-57.dll",
                "D:/dev/ffmpeg/ffmpeg/libavformat/avformat-59.dll",
                "D:/dev/ffmpeg/ffmpeg/libavcodec/avcodec-59.dll",
                "D:/dev/ffmpeg/ffmpeg/libavdevice/avdevice-59.dll",
                "D:/dev/ffmpeg/ffmpeg/libavfilter/avfilter-8.dll"
              ]
            }
          ]
      }]
    ]
  }]
}
