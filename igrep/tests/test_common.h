#pragma once
#include <gtest/gtest.h>
#include <filesystem>


class BaseTestFixture : public ::testing::Test {
    protected:
        std::filesystem::path temp_path;

        void SetUp() override{
            std::filesystem::path tmp = std::filesystem::path(SOURCE_DIR) / "testdata" / "temp";
            create_directories(tmp);
            temp_path = tmp;
        }

        void TearDown() override {
            std::filesystem::remove_all(temp_path);
        }
};