//
// Created by felix on 11/10/22.
//

#ifndef MAZES_POINT_HPP
#define MAZES_POINT_HPP

#include <cstdint>
#include <compare>

namespace mazes {
    struct Point {
        uint32_t x, y;
        constexpr friend auto operator<=>(const Point&, const Point&) = default;
    };
};

#endif //MAZES_POINT_HPP
