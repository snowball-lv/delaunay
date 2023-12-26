#pragma once
#include <limits>
#include <algorithm>

static inline float vlen(sf::Vector2f v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

class Tri {
public:
    sf::Vector2f a, b, c;
    sf::Vector2f circumcenter() const {
        // sf::Vector2f ap = a - a;
        sf::Vector2f bp = b - a;
        sf::Vector2f cp = c - a;
        float Dp = 2.0 * (bp.x * cp.y - bp.y * cp.x);
        float Upx = (1.0 / Dp) * (
                cp.y * (bp.x * bp.x + bp.y * bp.y)
              - bp.y * (cp.x * cp.x + cp.y * cp.y));
        float Upy = (1.0 / Dp) * (
                bp.x * (cp.x * cp.x + cp.y * cp.y)
              - cp.x * (bp.x * bp.x + bp.y * bp.y));
        sf::Vector2f Up = {Upx, Upy};
        return Up + a;
    }
    float ycenter() const {
        float min = std::min(a.y, std::min(b.y, c.y));
        float max = std::max(a.y, std::max(b.y, c.y));
        return (min + max) / 2.0;
        // return min;
        // return (a.y + b.y + c.y) / 3.0;
    }
    bool contains(sf::Vector2f v) const {
        sf::Vector2f center = circumcenter();
        float radius = vlen(a - center);
        float dist = vlen(v - center);
        return dist < radius;
    }
};

class Delaunay {
    std::vector<sf::Vector2f> points;
public:
    Delaunay(std::vector<sf::Vector2f> &points)
            : points(points) {
    }
    void triangulate() {

    }
};
