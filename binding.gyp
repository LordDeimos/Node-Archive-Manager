{
    "targets": [
        {
            "target_name": "archive_manager",
            "sources": ["main.cpp"],
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
            ]
        }
    ]
}
