{
  "targets": [{
    "target_name" : "libav",
    'sources' : [ 
      "src/bitstream/bitbuffer.cpp",
      "src/bitstream/reader.cpp",
      
      "src/receiver/receiver_main.cpp",
      "src/receiver/receiver.cpp",
      "src/receiver/receiver_api.cpp",

      "src/sender/sender_main.cpp",
      "src/sender/sender.cpp",
      "src/sender/sender_api.cpp",

      "src/init.cpp",
      "src/packet_payload.cpp",
      "src/packet_sequence.cpp"
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
      "D:/dev/ffmpeg/ffmpeg",
      "<!(node -p \"require('@astronautlabs/rtp').build_include_dir\")"
    ],
    'dependencies': [
        '<!(node -p \"require(\'node-addon-api\').gyp\")'
    ],
    "conditions": [
      ['OS=="linux"', {
        'link_settings' : {
          "libraries": [
            "<!(node -p \"require('@astronautlabs/rtp').build_dir\")/src/libjrtp.a"
          ],
          "ldflags" : [
            "-lm -ldl -lpthread"
	        ]
        },
      }],
      ['OS=="win"', {
        "cflags" : [
          '/GL-'
        ],
        "configurations": {
          "Release": {
            "msvs_settings": {
              "VCCLCompilerTool": {
                "RuntimeTypeInfo": "true",
                "ExceptionHandling": 1,
                'AdditionalOptions': [ ],
                'RuntimeLibrary': 2
              },
              "VCLinkerTool": {
                  "AdditionalLibraryDirectories": [
                      "<!(node -p \"require('@astronautlabs/rtp').release_library_dir\")"
                  ]
              }
            }
          },
          "Debug": {
            "msvs_settings": {
              "VCCLCompilerTool": {
                "RuntimeTypeInfo": "true",
                "ExceptionHandling": 1,
                'AdditionalOptions': [ ],
                'RuntimeLibrary': 3
              },
              "VCLinkerTool": {
                  "AdditionalLibraryDirectories": [
                      "<!(node -p \"require('@astronautlabs/rtp').debug_library_dir\")"
                  ]
              }
            }
          }
        },
        'link_settings' : {
            "libraries": [
                "jrtplib.lib",
                "Ws2_32.lib"
            ],
        }
      }],
      ['OS=="mac"', {
        'cflags+': ['-fvisibility=hidden'],
        "link_settings": {
          "libraries": [
            '-F/Library/Frameworks',
            '-framework', 'DeckLinkAPI'
          ]
        }
      }],
    ]
  }]
}
