import qbs

CppApplication {
    consoleApplication: true
    files: [
        "biginteger.cpp",
        "biginteger.h",
        "catch.hpp",
        "main.cpp",
    ]

    Group {     // Properties for the produced executable
        fileTagsFilter: product.type
        qbs.install: true
    }
}
