#pragma once

#include <string>
#include <filesystem>
#include <vector>

namespace NAlgo {
    class Test {
    public:
        explicit Test(const std::filesystem::path& dataset_location, const std::string& test_name);
    };

    Test LoadSingleTest(const std::filesystem::path& dataset_location, const std::string& test_name);

    std::vector<Test> LoadAllTests(const std::filesystem::path& dataset_location);
}
