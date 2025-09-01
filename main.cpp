#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "json.hpp"

struct Point {
    int x;
    int y;
};

// Decode a string value from base (digits only, base = 10)
int base_decode(const std::string& value, int base) {
    int result = 0;
    for (char ch : value) {
        result = result * base + (ch - '0');
    }
    return result;
}

int main() {
    std::ifstream infile("testcase.json");
    if (!infile.is_open()) {
        std::cerr << "Could not open JSON file.\n";
        return 1;
    }

    nlohmann::json j;
    infile >> j;
    infile.close();

    int n = j["keys"]["n"];
    int root = j["keys"]["k"]; // root given in problem statement

    std::vector<Point> points;

    // Read points and decode y values from respective bases
    for (auto& [key, value] : j.items()) {
        if (key == "keys") continue;
        int base = std::stoi(value["base"].get<std::string>());
        std::string value_str = value["value"].get<std::string>();
        int y = base_decode(value_str, base);

        int x = std::stoi(key); // Use JSON key as x
        points.push_back({x, y});
    }

    // Print decoded points
    std::cout << "Decoded points:\n";
    for (const auto& pt : points) {
        std::cout << "x = " << pt.x << ", y = " << pt.y << "\n";
    }

    if (points.size() < 2) {
        std::cerr << "Need at least 2 (x,y) pairs plus root.\n";
        return 2;
    }

    // For demo: pick the last two points from JSON + root
    // (works because JSON may contain many points)
    Point p1, p2;
bool first = true;
for (auto &pt : points) {
    if (pt.x == root) continue; // skip duplicate x
    if (first) { p1 = pt; first = false; }
    else { p2 = pt; break; }
}
Point p3 = {root, 0};

    std::cout << "\nUsing points:\n";
    std::cout << "(" << p1.x << ", " << p1.y << ")\n";
    std::cout << "(" << p2.x << ", " << p2.y << ")\n";
    std::cout << "(" << p3.x << ", " << p3.y << ") [root]\n";

    // Solve linear system for quadratic coefficients
    // y = a*x^2 + b*x + c
    double A[3][4] = {
        {1.0 * p1.x * p1.x, 1.0 * p1.x, 1.0, 1.0 * p1.y},
        {1.0 * p2.x * p2.x, 1.0 * p2.x, 1.0, 1.0 * p2.y},
        {1.0 * p3.x * p3.x, 1.0 * p3.x, 1.0, 1.0 * p3.y}
    };

    // Gaussian elimination
    for (int i = 0; i < 3; ++i) {
        double divisor = A[i][i];
        for (int j = i; j < 4; ++j)
            A[i][j] /= divisor;
        for (int k = i + 1; k < 3; ++k) {
            double factor = A[k][i];
            for (int j = i; j < 4; ++j)
                A[k][j] -= factor * A[i][j];
        }
    }

    // Back-substitution
    for (int i = 2; i >= 0; --i) {
        for (int k = i + 1; k < 3; ++k)
            A[i][3] -= A[i][k] * A[k][3];
    }

    double a = A[0][3];
    double b = A[1][3];
    double c = A[2][3];

    std::cout << "\nQuadratic coefficients:\n";
    std::cout << "a = " << a << "\n";
    std::cout << "b = " << b << "\n";
    std::cout << "c = " << c << "\n";

    std::cout << "\nFinal value of c: " << c << "\n";

    return 0;
}
