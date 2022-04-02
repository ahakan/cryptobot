#include "gtest/gtest.h"

#include "test_utilities.h"

int main(int argc, char **argv) 
{
    TestUtilities::mArgc = argc;
    
    ::testing::InitGoogleTest(&argc, argv);

    int result = RUN_ALL_TESTS();

    return result;
}