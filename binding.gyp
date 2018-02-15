{
    "targets": [{
            "target_name": "manager",
            "sources": ["./src/main.cpp"],
            'conditions': [
                ['OS=="win"', {
                    'include_dirs': [
                        "./lib"
                    ],
                    'libraries': [
                        "../lib/zlibstatic.lib",
                        "../lib/liblzma.lib",
                        "../lib/bzip2.lib",
                        "../lib/archive_static.lib"
                    ],
                    "defines": [
                        "LIBARCHIVE_STATIC"
                    ],
                    "msvs_settings": {
                        "VCCLCompilerTool": {
                            "RuntimeLibrary": 0
                        },
                        "VCLinkerTool": {
                            "IgnoreDefaultLibraryNames": ["libcmt"]
                        }
                    }
                }],
                ['OS=="linux"', {
                    'libraries': [
                        "-larchive"
                    ]
                }]
            ],
            "include_dirs": [
                "<!(node -e \"require('nan')\")"
            ]
        },
        {
            "target_name": "post_build",
            "type": "none",
            "dependencies": ["manager"],
            "conditions": [
                ['OS=="win"', {
                    "copies": [{
                        "files": ["./lib/bzip2.dll","./lib/liblzma.dll"],
                        "destination": "./build/Release/",
                    }]
                }]
            ]
        }
    ]
}