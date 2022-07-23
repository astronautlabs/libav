{
  "targets": [{
    "target_name" : "nlav",
    'sources' : [ 
      "native/avcodec/codec-context.cpp",
      "native/avcodec/codec.cpp",
      "native/avcodec/index.cpp",
      "native/avcodec/packet.cpp",
      "native/avcodec/profile.cpp",

      "native/avutil/avutil.cpp",
      "native/avutil/buffer.cpp",
      "native/avutil/channel-layout.cpp",
      "native/avutil/class.cpp",
      "native/avutil/dict.cpp",
      "native/avutil/frame.cpp",
      "native/avutil/index.cpp",
      "native/libavaddon.cpp",
      "native/resource.cpp"
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
      "<!(node -p \"require('node-addon-api').include_dir\")"
    ],
    'dependencies': [
        '<!(node -p \"require(\'node-addon-api\').gyp\")'
    ],
    'defines': [
      '__STDC_CONSTANT_MACROS'
    ],
    "conditions": [
      ['OS=="linux"', {
        'cflags': [ 
          '-std=c++11',
          '<!(pkg-config libavutil --cflags)',
          '<!(pkg-config libavformat --cflags)',
          '<!(pkg-config libavcodec --cflags)',
          '<!(pkg-config libavdevice --cflags)',
          '<!(pkg-config libavfilter --cflags)',
          '<!(pkg-config libswresample --cflags)',
          '<!(pkg-config libswscale --cflags)',
        ],
        "libraries": [
            "-lavutil",
            "-lavformat",
            "-lavcodec",
            "-lavdevice",
            "-lavfilter",
            "-lswresample",
            "-lswscale"
        ],
      }],
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
        "include_dirs": [
          "dist/ffmpeg/include"
        ],
        "libraries": [
            "-l../dist/ffmpeg/lib/avutil",
            "-l../dist/ffmpeg/lib/avformat",
            "-l../dist/ffmpeg/lib/avcodec",
            "-l../dist/ffmpeg/lib/avdevice",
            "-l../dist/ffmpeg/lib/avfilter",
            "-l../dist/ffmpeg/lib/swresample",
            "-l../dist/ffmpeg/lib/swscale"
        ],
        "copies": [
            {
              "destination": "build/Release/",
              "files": [
                "./dist/ffmpeg/lib/avutil-57.dll",
                "./dist/ffmpeg/lib/avformat-59.dll",
                "./dist/ffmpeg/lib/avcodec-59.dll",
                "./dist/ffmpeg/lib/avdevice-59.dll",
                "./dist/ffmpeg/lib/avfilter-8.dll",
                "./dist/ffmpeg/lib/swresample-4.dll",
                "./dist/ffmpeg/lib/swscale-6.dll"
              ]
            },
            {
              "destination": "build/Debug/",
              "files": [
                "D:/dev/ffmpeg/ffmpeg/libavutil/avutil-57.dll",
                "D:/dev/ffmpeg/ffmpeg/libavformat/avformat-59.dll",
                "D:/dev/ffmpeg/ffmpeg/libavcodec/avcodec-59.dll",
                "D:/dev/ffmpeg/ffmpeg/libavdevice/avdevice-59.dll",
                "D:/dev/ffmpeg/ffmpeg/libavfilter/avfilter-8.dll",
                "D:/dev/ffmpeg/ffmpeg/libswresample/swresample-4.dll",
                "D:/dev/ffmpeg/ffmpeg/libswscale/swscale-6.dll"
              ]
            }
          ]
      }]
    ]
  }]
}
