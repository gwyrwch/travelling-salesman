#pragma once

#include <algo/Tour.h>
#include <string>

namespace NAlgo {
    class ISolution {
    public:
        explicit ISolution(int version)
            : version(version)
        {}
        virtual ~ISolution() = default;

        virtual Tour solve(const Test& test) = 0;

        /*
            Возвращает название алгоритма решения.
         */
        virtual std::string solution_name() const = 0;

        int solution_version() const {
            return version;
        }
    protected:
        int version;
    };

    enum class ESolution : int {
        NearestNeighbour,
        NaiveSolution,
        MinimumSpanningTree,

        INVALID_SOLUTION_NAME
    };
}
