{
    'variables': {
        'module_root_dir': '<(PRODUCT_DIR)/../..',
        'p7zip_src': 'p7zip_16.02_src_all.tar.bz2',
        'p7zip_dir': 'p7zip_16.02',
        '7z_src': '7z1900-src.7z',
    },
    'targets': [
        ###########################################################################
        {
            'target_name': 'initial_clean',
            'type': 'none',
            'actions': [
                {
                    'action_name': 'initial_clean_1', 'inputs': [], 'outputs': ['initial_clean_1'],
                    'action': ['node', 'ext/files/clean.js', '<(CONFIGURATION_NAME)'],
                },
            ],
        },
        ###########################################################################
        {
            'target_name': 'extract_src',
            'type': 'none',
            'dependencies': ['initial_clean'],
            'conditions': [
                ['OS == "win"', {
                    'actions': [
                        {
                            'action_name': 'extract_src_1', 'inputs': [], 'outputs': ['extract_src_1'],
                            'action': ['../ext/files/7z32/7z.exe x -aos -y "../ext/files/<(7z_src)" -o"../ext/7z"'],
                        },
                    ],
                }, {
                    'actions': [
                        {
                            'action_name': 'extract_src_1', 'inputs': [], 'outputs': ['extract_src_1'],
                            'action': ['tar', 'xf', 'ext/files/<(p7zip_src)', '-C', 'ext'],
                        },
                        {
                            'action_name': 'extract_src_2', 'inputs': [], 'outputs': ['extract_src_2'],
                            'action': ['mv', '-n', 'ext/<(p7zip_dir)', 'ext/7z'],
                        },
                        {
                            'action_name': 'extract_src_3', 'inputs': [], 'outputs': ['extract_src_3'],
                            'action': ['rm', '-rf', 'ext/<(p7zip_dir)'],
                        },
                    ],
                }],
            ],
        },
        ###########################################################################
        {
            'target_name': 'n7zip',
            'dependencies': ['extract_src'],
            "sources": [
                "ext/n7zip.cpp",
                "ext/n7zip/debug.c",
                "ext/n7zip/utils.cpp",
                "ext/n7zip/guid.cpp",
                "ext/n7zip/library.cpp",
                "ext/n7zip/test_object.cpp",
                "ext/7z/CPP/Common/IntToString.cpp",
                "ext/7z/CPP/Common/MyString.cpp",
                "ext/7z/CPP/Common/StringConvert.cpp",
                "ext/7z/CPP/Common/UTFConvert.cpp",
                "ext/7z/CPP/Windows/PropVariant.cpp",
                "ext/7z/CPP/Windows/TimeUtils.cpp",
            ],
            "include_dirs": [
                "<!@(node -p \"require('node-addon-api').include\")",
                "ext",
                "ext/7z/CPP",
            ],
            'cflags!': ['-fno-exceptions'],
            'cflags_cc!': ['-fno-exceptions'],
            'xcode_settings': {
                'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
                'CLANG_CXX_LIBRARY': 'libc++',
                'MACOSX_DEPLOYMENT_TARGET': '10.7',
            },
            'msvs_settings': {
                'VCCLCompilerTool': {
                    'WarningLevel': '4',
                    'ExceptionHandling': 1
                },
            },
            "conditions": [
                [
                    'OS != "win"', {
                        "include_dirs": [
                            "ext/7z/CPP/myWindows",
                            "ext/7z/CPP/include_windows",
                        ],
                        "sources": [
                            "ext/7z/CPP/Common/MyWindows.cpp",
                        ],
                    }
                ],
                [
                    'OS == "mac"', {
                        'cflags+': [
                            '-fvisibility=hidden',
                        ],
                        'xcode_settings': {
                            'GCC_SYMBOLS_PRIVATE_EXTERN': 'YES',  # -fvisibility=hidden
                            # 'shared_timed_mutex' is unavailable: introduced in macOS 10.12
                            'MACOSX_DEPLOYMENT_TARGET': '10.12',
                        }
                    }
                ],
            ]
        },
        ###########################################################################
        {
            'target_name': 'make_7z',
            'dependencies': ['n7zip'],
            'type': 'none',
            'conditions': [
                ['OS != "win"', {
                    "actions": [
                        {
                            'action_name': 'make_7z_1', 'inputs': [], 'outputs': ['make_7z_1'],
                            'action': ['make', '7z', '-C', 'ext/7z'],
                        },
                    ],
                }],
            ],
        },
        ###########################################################################
        {
            'target_name': 'copy_library',
            'type': 'none',
            'dependencies': ['make_7z'],
            'conditions': [
                ['OS != "win"', {
                    'actions': [
                        {
                            'action_name': 'copy_library_1', 'inputs': [], 'outputs': ['copy_library_1'],
                            'action': ['mkdir', '-p', 'ext/7z/bin/Codecs'],
                        },
                        {
                            'action_name': 'copy_library_2', 'inputs': [], 'outputs': ['copy_library_2'],
                            'action': ['cp', '-r', 'ext/7z/bin/7z.so', 'ext/7z/bin/Codecs', '<(PRODUCT_DIR)'],
                        },
                    ],
                }],
            ],
        },
        ###########################################################################
        {
            'target_name': 'final_clean',
            'type': 'none',
            'dependencies': ['copy_library'],
            'actions': [
                {
                    'action_name': 'final_clean_1', 'inputs': [], 'outputs': ['final_clean_1'],
                    'action': ['node', 'ext/files/clean.js', '<(CONFIGURATION_NAME)'],
                },
            ],
        },
    ],
}
