{
    "targets": [{
            "target_name": "manager",
            "sources": ["./src/main.cpp"],
            'conditions': [
                ['OS=="win"', {
                    'include_dirs': [
                        "./lib"
                    ],
                    'conditions': [
                        ["target_arch == 'ia32'", {
                            'libraries': [
                                "../lib/32/zlibstatic.lib",
                                "../lib/32/liblzma.lib",
                                "../lib/32/libbz2.exp",
                                "../lib/32/libbz2-static.lib",
                                "../lib/32/archive_static.lib"
                            ]
                        }],
                        ["target_arch=='x64'", {
                            'libraries': [
                                "../lib/64/zlibstatic.lib",
                                "../lib/64/liblzma.lib",
                                "../lib/64/libbz2.exp",
                                "../lib/64/libbz2-static.lib",
                                "../lib/64/archive_static.lib"
                            ]
                        }]
                    ],
                    "defines": [
                        "LIBARCHIVE_STATIC",
                        "LIBBZ2_STATIC"
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
                    'conditions': [
                        ["target_arch == 'ia32'", {
                            "copies": [{
                                "files": [ "./lib/32/liblzma.dll"],
                                "destination": "./build/Release/",
                            }]
                        }],
                        ["target_arch=='x64'", {
                            "copies": [{
                                "files": ["./lib/64/liblzma.dll"],
                                "destination": "./build/Release/",
                            }]
                        }]
                    ]
                }]
            ]
        }
    ]
}