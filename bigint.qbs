import qbs

Project {
    minimumQbsVersion: "1.6"
    name: "bigint"

    CppApplication {
        consoleApplication: true
        files: [
            "bigint.h",
            "catch.hpp",
            "main.cpp",
        ]

        Group {     // Properties for the produced executable
            fileTagsFilter: product.type
            qbs.install: true
        }
    }
}


