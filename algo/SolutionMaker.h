#pragma once

#include <algo/NearestNeighbour.h>
#include <algo/NaiveSolution.h>
#include <algo/MinimumSpanningTree.h>
#include <algo/LocalSearch.h>
#include <algo/BranchAndBound.h>
#include <algo/GeneticAlgorithm.h>
#include <algo/SimulatedAnnealing.h>

#include <config/CacheConfig.h>
#include <fstream>


namespace NAlgo {
    ESolution ParseSolutionName(const std::string& solution_name) {
        if (solution_name == "NearestNeighbour" or solution_name == "NN") {
            return ESolution::NearestNeighbour;
        } else if (solution_name == "NaiveSolution" or solution_name == "NS") {
            return ESolution::NaiveSolution;
        } else if (solution_name == "MinimumSpanningTree" or solution_name == "MST") {
            return ESolution::MinimumSpanningTree;
        } else if (solution_name == "BranchAndBound" or solution_name == "BAB") {
            return ESolution::BranchAndBound;
        } else if(solution_name == "GeneticAlgorithm" or solution_name == "GA") {
            return ESolution::GeneticAlgorithm;
        }
        return ESolution::INVALID_SOLUTION_NAME;
    }

    EOptimizer ParseOptimizerName(const std::string& optimizer_name) {
        if (optimizer_name == "LocalSearch" or optimizer_name == "LS") {
            return EOptimizer::LocalSearch;
        } else if (optimizer_name == "SimulatedAnnealing" or optimizer_name == "SA") {
            return EOptimizer::SimulatedAnnealing;
        }
        throw;
    }

    int GetSolutionVersion(const std::string& solution_name) {
        /*
         * format: <testname>_<solution_name>_<version>.tour
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

    std::unique_ptr<ISolution> MakeSolution(const std::string& solution_name, SolutionConfig config) {
        ESolution solution_code = ParseSolutionName(solution_name);
        int version = GetSolutionVersion(solution_name);

        switch (solution_code) {
            case ESolution::NearestNeighbour:
                return std::make_unique<NearestNeighbour>(version, std::move(config));
            case ESolution::NaiveSolution:
                return std::make_unique<NaiveSolution>(version, std::move(config));
            case ESolution::MinimumSpanningTree:
                return std::make_unique<MinimumSpanningTree>(version, std::move(config));
            case ESolution::BranchAndBound:
                return std::make_unique<BranchAndBound>(version, std::move(config));
            case ESolution::GeneticAlgorithm:
                return std::make_unique<GeneticAlgorithm>(version, std::move(config));
            case ESolution::INVALID_SOLUTION_NAME:
                throw std::runtime_error("Invalid solution name");
            default:
                return nullptr;
        }
    }

    std::unique_ptr<IOptimizer> MakeOptimizer(const std::string& optimizer_name, OptimizerConfig config) {
        EOptimizer optimizer_code = ParseOptimizerName(optimizer_name);
        int version = GetSolutionVersion(optimizer_name);

        switch (optimizer_code) {
            case EOptimizer::LocalSearch:
                return std::make_unique<LocalSearch>(version, std::move(config));
            case EOptimizer::SimulatedAnnealing:
                return std::make_unique<SimulatedAnnealing>(version, std::move(config));
            default:
                return nullptr;
        }
    }
}
