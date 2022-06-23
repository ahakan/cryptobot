#include "test_utilities.h"


BinanceUtilities *TestUtilities::pBu = NULL; 

int TestUtilities::mArgc = 0;
char** TestUtilities::mArgs = NULL;


TEST_F(TestUtilities, calculate_average)
{
	std::vector<std::string> vector = {"100.25", "90.75", "95.00", "98.00"};

    std::string average = TestUtilities::pBu->calculateAverage(vector);

    EXPECT_STREQ(average.c_str(), "96.000000");
}


TEST_F(TestUtilities, calculate_rsi)
{
	std::vector<std::string> vector = {"100.25", "90.75", "95.00", "98.00"};

    std::string average = TestUtilities::pBu->calculateRSI(vector);

    EXPECT_STREQ(average.c_str(), "43.283580");
}


TEST_F(TestUtilities, upper_to_lower)
{
    std::string lower = TestUtilities::pBu->upperToLower("Hello! World.");

    EXPECT_STREQ(lower.c_str(), "hello! world.");
}


TEST_F(TestUtilities, round_string)
{
    std::string rounded = TestUtilities::pBu->roundString("100.23456789", 4);

    EXPECT_STREQ(rounded.c_str(), "100.23450000");
}


TEST_F(TestUtilities, round_string_2)
{
    std::string rounded = TestUtilities::pBu->roundString("100.23", 4);

    EXPECT_STREQ(rounded.c_str(), "100.2300");
}


TEST_F(TestUtilities, round_string_3)
{
    std::string rounded = TestUtilities::pBu->roundString("100.2300", 4);

    EXPECT_STREQ(rounded.c_str(), "100.2300");
}


TEST_F(TestUtilities, add_two_strings)
{
    std::string result = TestUtilities::pBu->atfts("100.230000", "10.49750000");

    std::string rounded = TestUtilities::pBu->roundString(result, 4);

    EXPECT_STREQ(rounded.c_str(), "110.727500");
}


TEST_F(TestUtilities, sub_two_strings)
{
    std::string result = TestUtilities::pBu->stfts("100.230000", "10.49750000");

    std::string rounded = TestUtilities::pBu->roundString(result, 4);

    EXPECT_STREQ(rounded.c_str(), "89.732500");
}


TEST_F(TestUtilities, multiply_two_strings)
{
    std::string result = TestUtilities::pBu->mtfts("10.490000", "2");

    std::string rounded = TestUtilities::pBu->roundString(result, 4);

    EXPECT_STREQ(rounded.c_str(), "20.980000");
}


TEST_F(TestUtilities, compare_two_strings)
{
    bool result = TestUtilities::pBu->ctscf("10.490000", "10.480000");

    EXPECT_TRUE(result);
}


TEST_F(TestUtilities, get_tick_size)
{
    int result = TestUtilities::pBu->getTickSize("0.0001000000");

    EXPECT_EQ(result, 4);
}


TestUtilities::TestUtilities(void)
{
	// std::cout << "TestUtilities." << std::endl;
}

TestUtilities::~TestUtilities(void)
{
	// std::cout << "~TestUtilities." << std::endl;
}

void TestUtilities::SetUpTestCase()
{
	// std::cout << "Set up test cases." << std::endl;

    if ( pBu != NULL )
	{
		delete pBu;
	}

	pBu = new BinanceUtilities;
}

void TestUtilities::TearDownTestCase()
{
	// std::cout << "Tear down test cases." << std::endl;

	delete pBu;
	pBu = NULL;
}

void TestUtilities::SetUp(void)
{
	// std::cout << "SetUp." << std::endl;
}

void TestUtilities::TearDown(void)
{
	// std::cout << "TearDown." << std::endl;
}
