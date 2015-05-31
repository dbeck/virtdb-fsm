{
  'variables': {
    'fsm_sources' :  [
                         'src/fsm/fsm.cc',   'src/fsm/fsm.hh',
                       ],
  },
  'target_defaults': {
    'default_configuration': 'Debug',
    'configurations': {
      'Debug': {
        'defines':  [ 'DEBUG', '_DEBUG', ],
        'cflags':   [ '-O0', '-g3', ],
        'ldflags':  [ '-g3', ],
        'xcode_settings': {
          'OTHER_CFLAGS':  [ '-O0', '-g3', ],
          'OTHER_LDFLAGS': [ '-g3', ],
        },
      },
      'Release': {
        'defines':  [ 'NDEBUG', 'RELEASE', ],
        'cflags':   [ '-O3', ],
        'xcode_settings': {
        },
      },
    },
    'include_dirs': [
                      './src/',
                      '/usr/local/include/',
                      '/usr/include/',
                    ],
    'cflags': [
                '-Wall',
                '-fPIC',
                '-std=c++11',
              ],
    'defines':  [
                  'PIC',
                  'STD_CXX_11',
                  '_THREAD_SAFE',
                ],
  },
  'conditions': [
    ['OS=="mac"', {
     'defines':            [ 'FSM_MAC_BUILD', 'NO_IPV6_SUPPORT', ],
     'xcode_settings':  {
       'GCC_ENABLE_CPP_EXCEPTIONS':    'YES',
       'OTHER_CFLAGS':               [ '-std=c++11', ],
     },
     },
    ],
    ['OS=="linux"', {
     'defines':            [ 'FSM_LINUX_BUILD', ],
     'link_settings': {
       'ldflags':   [ '-Wl,--no-as-needed', ],
       'libraries': [ '-lrt', ],
      },
     },
    ],
  ],
  'targets' : [
    {
      'conditions': [
        ['OS=="mac"', {
          'variables':  { 'fsm_root':  '<!(pwd)/', },
          'xcode_settings':  {
            'GCC_ENABLE_CPP_EXCEPTIONS':    'YES',
            'OTHER_CFLAGS':               [ '-std=c++11', ],
          },
          'direct_dependent_settings': {
            'include_dirs': [ '<(fsm_root)/', ],
          },},],
        ['OS=="linux"', {
          'direct_dependent_settings': {
            'include_dirs':       [ '.', ],
          },},],
      ],
      'target_name':                   'fsm',
      'type':                          'static_library',
      'defines':                     [ 'USING_FSM_LIB',  ],
      'sources':                     [ '<@(fsm_sources)', ],
    },
    {
      'target_name':       'fsm_test',
      'type':              'executable',
      'dependencies':  [ 'fsm', 'deps_/gtest/gyp/gtest.gyp:gtest_lib', ],
      'include_dirs':  [ './deps_/gtest/include/', ],
      'sources':       [ 'test/fsm_test.cc', ],
    },
  ],
}
