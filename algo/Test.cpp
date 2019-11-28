#include "Test.h"

#include <algo/Distance.h>
#include <util/String.h>

#include <iostream>
#include <fstream>

namespace NAlgo {
    Test::Test(const std::filesystem::path &dataset_location, const std::string &test_name) {
        auto full_path = dataset_location / (test_name + ".tsp");

        std::ifstream test_in(full_path);
        if (test_in) {
            ParseTestFromFile(test_in);
        } else {
            throw std::runtime_error("failed load test " + test_name);
        }
    }

    void Test::ParseTestFromFile(std::istream &test_in) {
        std::string line;
        while (true) {
            getline(test_in, line);
            if (line == "NODE_COORD_SECTION") {
                break;
            }

            auto tokens = NUtil::SplitAndTrim(line, ':');

            // TODO: ENSURE(tokens.size() >= 2);

            if (tokens[0] == "NAME") {
                name = tokens[1];
                // TODO ENSURE name == test_name
            } else if (tokens[0] == "COMMENT") {
                comment = tokens[1];
            } else if (tokens[0] == "DIMENSION") {
                vertex_num = atoi(tokens[1].c_str());
            } else if (tokens[0] == "TYPE") {
                // TODO: ENSURE tokens[1]  == "TSP"
            } else if (tokens[0] == "EDGE_WEIGHT_TYPE") {
                distance_function = MakeDistanceFunction(tokens[1]);
                weight_type = tokens[1];
            } else {
                throw std::runtime_error("Unknown field in test " + line);
            }
        }

        if (weight_type == "EUC_2D") {
            points.resize(vertex_num);
            for (int i = 0; i < vertex_num; i++) {
                int vertex, x, y;
                if (!(test_in >> vertex >> x >> y)) {
                    throw std::runtime_error("unexpected end of file");
                }
                points[--vertex] = {x, y};
            }
        }

        // TODO: ENSURE getline ==  EOF
    }

    Test LoadSingleTest(const std::filesystem::path &dataset_location, const std::string &test_name) {
        return Test(dataset_location, test_name);
    }

    std::vector<Test> LoadAllTests(const std::filesystem::path &dataset_location) {
        // for(auto it = fs::directory_iterator("sandbox"); it != fs::directory_iterator(); ++it)
        return {};
    }
}
