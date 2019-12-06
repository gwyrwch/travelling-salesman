#include "Test.h"

#include <algo/Distance.h>
#include <util/String.h>

#include <iostream>
#include <fstream>
#include <filesystem>

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
            line = NUtil::Trim(line);
            if (line == "NODE_COORD_SECTION" || line == "EDGE_WEIGHT_SECTION") {
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
            } else if(tokens[0] == "EDGE_WEIGHT_FORMAT") {
                weight_format = tokens[1];
            } else if (tokens[0] == "DISPLAY_DATA_TYPE") {
                // todo check tokens[1]
            } else if (tokens[0] == "NODE_COORD_TYPE") {
                // todo check tokens[1]
            } else  {
                throw std::runtime_error("Unknown field in test " + line);
            }
        }

        if (weight_type == "EUC_2D" || weight_type == "ATT" || weight_type == "CEIL_2D" || weight_type  == "GEO") {
            points.resize(vertex_num);
            for (int i = 0; i < vertex_num; i++) {
                double vertex, x, y;
                if (!(test_in >> vertex >> x >> y)) {
                    throw std::runtime_error("unexpected end of file");
                }
                points[--vertex] = {x, y};
            }
        } else if  (weight_type == "EXPLICIT") {
            if (weight_format == "FULL_MATRIX") {
                for (int i = 0; i < vertex_num; i++) {
                    std::vector<int> l;
                    for (int j = 0; j < vertex_num; j++) {
                        int w;
                        if (!(test_in >> w)) {
                            throw std::runtime_error("unexpected end of file");
                        }
                        l.push_back(w);
                    }
                    matrix.push_back(l);
                }
            } else if (weight_format == "UPPER_ROW") {
                matrix.assign(vertex_num, std::vector<int>(vertex_num, 0));
                for (int i = 0; i < vertex_num; i++) {
                    for (int j = i + 1; j < vertex_num; j++) {
                        test_in >> matrix[i][j];
                        matrix[j][i] = matrix[i][j];
                    }
                }
            } else if (weight_format == "LOWER_DIAG_ROW") {
                matrix.assign(vertex_num, std::vector<int>(vertex_num, 0));
                for (int i = 0; i < vertex_num; i++) {
                    for (int j = 0; j <= i; j++) {
                        test_in >> matrix[i][j];
                        matrix[j][i] = matrix[i][j];
                    }
                }
            } else if (weight_format == "UPPER_DIAG_ROW") {
                matrix.assign(vertex_num, std::vector<int>(vertex_num, 0));
                for (int i = 0; i < vertex_num; i++) {
                    for (int j = i; j < vertex_num; j++) {
                        test_in >> matrix[i][j];
                        matrix[j][i] = matrix[i][j];
                    }
                }
            }

        }

        // TODO: ENSURE getline ==  EOF
    }

    Test LoadSingleTest(const std::filesystem::path &dataset_location, const std::string &test_name) {
        return Test(dataset_location, test_name);
    }

    std::vector<Test> LoadAllTests(const std::filesystem::path &dataset_location) {
        std::vector<Test> all_tests;
        for(auto& p: std::filesystem::directory_iterator(dataset_location)) {
            if(p.path().string().find(".tsp") != std::string::npos) {
                std::cout << p.path() << std::endl;
                all_tests.push_back(Test(dataset_location, NUtil::Split(p.path().filename().string(), '.')[0]));
            }
        }


        return all_tests;
    }
}
