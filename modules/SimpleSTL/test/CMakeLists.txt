add_simplecpp_test(SimpleSTL Test
        TestShared.h
        Test.cpp
)

# Prevent MSVC from complaining about template usage
target_compile_options(SimpleCPP-SimpleSTL-Test PRIVATE /bigobj)

add_simplecpp_test(SimpleSTL ReleaseTest
        TestShared.h
        ReleaseTest.cpp
)