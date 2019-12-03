#pragma once

#include <algo/NearestNeighbour.h>
#include <algo/NaiveSolution.h>
#include <algo/MinimumSpanningTree.h>

#include <config/CacheConfig.h>
#include <fstream>


namespace NAlgo {
    ESolution ParseSolutionName(const std::string& solution_name) {
        if (solution_name == "NearestNeighbour") {
            return ESolution::NearestNeighbour;
        } else if (solution_name == "NaiveSolution") {
            return ESolution::NaiveSolution;
        } else if (solution_name == "MinimumSpanningTree") {
            return ESolution::MinimumSpanningTree;
        }
        return ESolution::INVALID_SOLUTION_NAME;
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
            case ESolution::NaiveSolution:
                return std::make_unique<NaiveSolution>(version);
            case ESolution::MinimumSpanningTree:
                return std::make_unique<MinimumSpanningTree>(version);
            default:
                return nullptr;
        }
    }
}
