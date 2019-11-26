#pragma once

#include "Test.h"
#include "Tour.h"
#include <string>

class ISolution {
public:
    ISolution(Test test)
        : test(test)
    {}

    virtual Tour solve() = 0;

    /*
        Возвращает название алгоритма решения.
     */
    virtual std::string solution_name() = 0;
private:
    Test test;
};
