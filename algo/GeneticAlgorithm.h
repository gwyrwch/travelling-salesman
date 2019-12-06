#pragma once

#include <algo/IOptimizer.h>
#include <lib/Timer.h>

namespace NAlgo {
    class GeneticAlgorithm : public IOptimizer {
    public:
        explicit GeneticAlgorithm(int version, OptimizerConfig config)
                : IOptimizer(version, std::move(config))
        {}
        Tour optimize(const Test& test, Tour baseline) override {

        }

        std::string optimizer_name() const override  {
            return "GeneticAlgorithm";
        }

    private:

    };
}

