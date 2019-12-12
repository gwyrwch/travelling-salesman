#pragma once

#include <algo/Test.h>
#include <algo/Tour.h>

namespace NAlgo {
    struct OptimizerConfig {
        double deadline;
    };

    class IOptimizer {
    public:
        explicit IOptimizer(int version, OptimizerConfig config)
            : version(version)
            , config(std::move(config))
        {}

        virtual ~IOptimizer() = default;

        virtual Tour optimize(const Test& test, Tour baseline) = 0;

        virtual std::string optimizer_name() const = 0;

        int optimizer_version() const {
            return version;
        }

        double get_deadline() const {
            return config.deadline;
        }

    protected:
        int version;
        OptimizerConfig config;
    };

    enum class EOptimizer : int {
        LocalSearch,
        SimulatedAnnealing
    };
}

