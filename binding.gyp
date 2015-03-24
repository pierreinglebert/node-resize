{
  "targets": [{
      "target_name": "resizer"
      , "sources": [
          "src/resizer.cc"
        ]
      , 'libraries': [
          '<!@(pkg-config --libs opencv)'
        ]

      # For windows
      ,'include_dirs': [
          '<!@(pkg-config --cflags opencv)',
          '<!(node -e "require(\'nan\')")'
          ]

      , 'cflags': [
            '<!@(pkg-config --cflags "opencv >= 2.3.1" )'
            , '-Wall'
          ]
      , 'cflags!' : [ '-fno-exceptions']
      , 'cflags_cc!': [ '-fno-rtti',  '-fno-exceptions']
      , "conditions": [
        ['OS=="linux"', {
          'cflags': [
						'-std=gnu++0x'
					],
        }],
        ['OS=="mac"', {
          # cflags on OS X are stupid and have to be defined like this
          'xcode_settings': {
            'OTHER_CFLAGS': [
              "-mmacosx-version-min=10.7",
              "-std=c++11",
              "-stdlib=libc++",
              '<!@(pkg-config --cflags opencv)'
            ]
            , "GCC_ENABLE_CPP_RTTI": "YES"
            , "GCC_ENABLE_CPP_EXCEPTIONS": "YES"
          }
        }]

    ]
  }]
}
