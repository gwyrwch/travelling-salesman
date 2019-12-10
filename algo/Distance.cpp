#include "Distance.h"

#include <cmath>

namespace NAlgo {
    long double sqr(long double d) {
        return d  *  d;
    }

    double PI = 3.141592;
    double get_latitude(Point p) {
        int deg = int(p.x + 0.5);
        double min = p.x - deg;
        return PI * (deg + 5.0 * min / 3.0 ) / 180.0;
    }

    double get_longitude(Point p) {
        int deg = int(p.y + 0.5);
        double min = p.y - deg;
        return PI * (deg + 5.0 * min / 3.0 ) / 180.0;
    }

    std::function<int64_t(const Test&, int,int)> MakeDistanceFunction(const std::string& name) {
        if (name == "EUC_2D") {
            return [](const Test& test, int u, int v) {
                auto pt1 = test.GetPoint(u);
                auto pt2 = test.GetPoint(v);
                return lround(std::sqrt(sqr(pt1.x - pt2.x) + sqr(pt1.y - pt2.y)));
            };
        } else if (name == "GEO") {
            return [](const Test& test, int u, int v) {
                auto pt1 = test.GetPoint(u);
                auto pt2 = test.GetPoint(v);

                double latitude_u = get_latitude(pt1);
                double longitude_u = get_longitude(pt1);

                double latitude_v = get_latitude(pt2);
                double longitude_v = get_longitude(pt2);

                double RRR = 6378.388;
                double q1 = cos(longitude_u - longitude_v);
                double q2 = cos(latitude_u - latitude_v);
                double  q3 = cos(latitude_u + latitude_v);

                return (int) ( RRR * acos(0.5 * ((1.0 + q1) * q2 - (1.0 - q1) * q3)) + 1.0);

            };
        } else if (name == "ATT") {
            return [](const Test& test, int u, int v) {
                auto pt1 = test.GetPoint(u);
                auto pt2 = test.GetPoint(v);

                auto r12 = std::sqrt((1. * sqr(pt1.x - pt2.x) + sqr(pt1.y - pt2.y)) / 10.0);
                auto t12 = int(r12 + 0.5);

                if (t12 < r12)
                    return t12 + 1;

                return t12;
            };
        } else if (name == "EXPLICIT") {
            return [](const Test& test, int u, int  v){
                return test.GetMatrixValue(u, v);
            };
        } else if (name == "CEIL_2D") {
            return [](const Test& test, int u, int v) {
                auto pt1 = test.GetPoint(u);
                auto pt2 = test.GetPoint(v);
                return std::ceil(std::sqrt(sqr(pt1.x - pt2.x) + sqr(pt1.y - pt2.y)));
            };
        }

        throw std::runtime_error("No such distance function + " + name);

    }
}
