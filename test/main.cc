#include "gtest/gtest.h"

#include "MockHttp.h"

TEST(MockHttp, parse)
{
    MockHttp mock_http;

    Json::Value a;

    ON_CALL(mock_http, parse("{'Message': 'Hello'}", a, true))
        .WillByDefault(::testing::Return("Hello"));

    EXPECT_EQ(a, "Hello");
}

int main(int argc, char **argv) 
{
    // TestUtilities::mArgc = argc;
    
    ::testing::InitGoogleTest(&argc, argv);

    int result = RUN_ALL_TESTS();

    return result;
}