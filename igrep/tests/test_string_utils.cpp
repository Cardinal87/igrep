#include<gtest/gtest.h>
#include"utils/StringUtils.h"
#include<string>

using namespace std;
using namespace igrep::utils;

TEST(StringUtilsTest, ProcessString_ValidData_RemovePunctuation){
    string str = ", . / \\ [] {} () \" hello ' ' $ @ # ! ? % & ";

    string normizlized = StringUtils::normalize_line(str);

    ASSERT_EQ(normizlized, "hello");
}

TEST(StringUtilsTest, ProcessString_ValidData_ChangeCaseToLower){
    string str = "UPPER CASE";

    string normizlized = StringUtils::normalize_line(str);

    ASSERT_EQ(normizlized, "upper case");
}

