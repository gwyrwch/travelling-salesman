#pragma once

#include <algo/NearestNeighbour.h>
#include <config/CacheConfig.h>
#include <fstream>

namespace NAlgo {
    ESolution ParseSolutionName(const std::string& solution_name) {
        // todo:
        return ESolution::NearestNeighbour;
    }

    int GetSolutionVersion(const std::string& solution_name) {
        /*
         * format: <solution_name>_<version>_<testname>.tour
         * <solution_name>.info contains version of the solution
         */
        std::filesystem::path version_path = NConfig::CacheConfig::CACHE_DIRECTORY / (solution_name + ".info");
        int version = 0;
        if (std::filesystem::exists(version_path)) {
            std::ifstream in(version_path);
            in >> version;
            ++version;
        }
        {
            std::ofstream out(version_path);
            out << version;
        }
        return version;
    }

    std::unique_ptr<ISolution> MakeSolution(const std::string& solution_name) {
        ESolution solution_code = ParseSolutionName(solution_name);
        int version = GetSolutionVersion(solution_name);

        switch (solution_code) {
            case ESolution::NearestNeighbour:
                return std::make_unique<NearestNeighbour>(version);
            default:
                return nullptr;
        }
    }
}
