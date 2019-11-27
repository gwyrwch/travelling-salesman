#pragma once

#include <algo/NearestNeighbour.h>

namespace NAlgo {
    ESolution ParseSolutionName(const std::string& solution_name) {
        // todo:
        return ESolution::NearestNeighbour;
    }

    int GetSolutionVersion(ESolution solution_code) {
        // todo:
        return 0;
    }

    std::unique_ptr<ISolution> MakeSolution(const std::string& solution_name) {
        ESolution solution_code = ParseSolutionName(solution_name);
        int version = GetSolutionVersion(solution_code);

        switch (solution_code) {
            case ESolution::NearestNeighbour:
                return std::make_unique<NearestNeighbour>(version);
            default:
                return nullptr;
        }
    }
}
