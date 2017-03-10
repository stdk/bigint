import qbs

Project {
    minimumQbsVersion: "1.6"
    name: "bigint"

    CppApplication {
        name: "test"
        consoleApplication: true
        files: [
            "bigint.h",
            "catch.hpp",
            "test.cpp",
        ]

        Group {     // Properties for the produced executable
            fileTagsFilter: product.type
            qbs.install: true
        }
    }

    CppApplication {
        name: "calc"
        consoleApplication: true
        files: [
            "bigint.h",
            "calc.cpp",
            "test.py",
        ]

        Group {     // Properties for the produced executable
            fileTagsFilter: product.type
            qbs.install: true
        }
    }
}


