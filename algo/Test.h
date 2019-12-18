#pragma once

#include <string>
#include <filesystem>
#include <vector>

namespace NAlgo {
    struct Point {
        long double x, y;
    };

    class Test {
    public:
        explicit Test(const std::filesystem::path& dataset_location, const std::string& test_name);

        Point GetPoint(int index) const {
            if (points.empty()) {
                throw std::runtime_error("no points where given for the test");
            }
            return points.at(index);
        }

        std::vector<Point> GetPoints() const {
            return points;
        }

        int GetVertexNum() const {
            return vertex_num;
        }

        int GetMatrixValue(int u, int v) const {
            return matrix[u][v];
        };

        int64_t EvalDistance(int u, int v) const {
            return distance_function(*this, u, v);
        }

        std::string GetTestName() const {
            return name;
        }

    private:
        void ParseTestFromFile(std::istream& test_in);

        std::string name;
        std::string comment;
        int vertex_num;

        std::function<int64_t(const Test&, int, int)> distance_function;
        std::string weight_type;
        std::string weight_format;

        /*
         * EDGE_WEIGHT_TYPE:
         *   EUC_2D test
         *   ATT test
         *   CEIL_2D test
         */
        std::vector<Point> points;

        /*
         * EDGE_WEIGHT_TYPE:
         *   EXPLICIT test
         */
        std::vector<std::vector<int>> matrix;
    };

    Test LoadSingleTest(const std::filesystem::path& dataset_location, const std::string& test_name);

    std::vector<Test> LoadAllTests(const std::filesystem::path& dataset_location);
}
