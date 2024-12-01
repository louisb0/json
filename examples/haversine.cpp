#include "profiler/profiler.hpp"
#include "json/parser.hpp"
#include "json/value.hpp"

#include <cmath>
#include <iomanip>
#include <iostream>

double square(double a) { return a * a; }

double deg_to_rad(double degrees) { return 0.01745329251994329577 * degrees; }

double haversine(double x0, double y0, double x1, double y1, double radius) {
    double lat1 = y0;
    double lat2 = y1;
    double lon1 = x0;
    double lon2 = x1;

    double d_lat = deg_to_rad(lat2 - lat1);
    double d_lon = deg_to_rad(lon2 - lon1);
    lat1 = deg_to_rad(lat1);
    lat2 = deg_to_rad(lat2);

    double a = square(sin(d_lat / 2.0)) + cos(lat1) * cos(lat2) * square(sin(d_lon / 2));
    double c = 2.0 * asin(sqrt(a));

    double result = radius * c;

    return result;
}

int main(int argc, char *argv[]) {
    profiler::start_profile();

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <json_path>" << std::endl;
        return 1;
    }

    json::value json = json::parse_file(argv[1]);

    {
        PROFILE_BLOCK("computation");
        double total = 0;
        for (auto val : json["pairs"].as_array()) {
            total += haversine(val["x0"].as_number(), val["y0"].as_number(), val["x1"].as_number(),
                               val["y1"].as_number(), 6371000);
        }
        std::cout << std::setprecision(15) << total;
    }

    profiler::end_and_print_profile();
}
