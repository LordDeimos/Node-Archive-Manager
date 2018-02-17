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
                                "../lib/32/*"
                            ]
                        }],
                        ["target_arch=='x64'", {
                            'libraries': [
                                "../lib/64/*"
                            ]
                        }]
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