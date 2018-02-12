{
    "targets": [
        {
            "target_name": "manager",
            "sources": ["./src/main.cpp"],
            'conditions': [
                ['OS=="win"', {
                'include_dirs': [
                    "./lib"
                ],
                'libraries': [
                    "../lib/archive.exp",
                    "../lib/archive.lib",
                    "../lib/archive_static.lib"
                ],
                }],
                ['OS=="linux"',{
                    'libraries':[
                        "-larchive"
                    ]
                }]
            ],
            "include_dirs" : [
                "<!(node -e \"require('nan')\")"
            ]
        }
    ]
}
