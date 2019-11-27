#include "Test.h"

using namespace NAlgo;

Test::Test(const std::filesystem::path& dataset_location, const std::string& test_name) {
    // TODO load from filename
}

Test LoadSingleTest(const std::filesystem::path& dataset_location, const std::string& test_name) {
    return Test(dataset_location, test_name);
}

std::vector<Test> LoadAllTests(const std::filesystem::path& dataset_location) {
    return {};
}
