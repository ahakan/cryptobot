#ifndef TEST_UTILITIES
#define TEST_UTILITIES

// Includes
#include "utilities.h"

// Libraries
#include "gtest/gtest.h"

// Standard Libraries
#include <iostream>
#include <string>
#include <vector>

class TestUtilities : public ::testing::Test 
{
    public:
        static BinanceUtilities *pBu; 

        static int mArgc;
	    static char** mArgs;

    public:
                            TestUtilities(void);
        virtual             ~TestUtilities(void);
        static void         SetUpTestCase();
	    static void         TearDownTestCase();
	    virtual void        SetUp(void);
	    virtual void        TearDown(void);
};


#endif