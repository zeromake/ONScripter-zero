add_requires("gtest")


target("test.gb18030")
    add_packages("gtest")
    add_files("gb18030.cpp")
    add_includedirs("../src")
    add_files("../src/charset/*.c")
