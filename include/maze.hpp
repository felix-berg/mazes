//
// Created by felix on 11/10/22.
//

#pragma once

#include <cstdint>
#include <vector>
#include <compare>
#include <cassert>
#include <iostream>

#include "point.hpp"

namespace mazes {
struct Maze {
    static constexpr uint8_t path = 0, wall = 1, solution = 3;
    const uint32_t width, height;

    Maze(uint32_t w, uint32_t h, std::initializer_list<uint8_t> lst) noexcept
            : width { w }, height { h }, cells_ { lst }
    {
        // assert(lst.size() == 10200);
        assert(lst.size() == w * h);
    }

    template <typename Ite>
    Maze(uint32_t w, uint32_t h, Ite beg, Ite end) noexcept
            : width { w }, height { h }, cells_(w * h)
    {
        std::copy(beg, end, cells_.begin());
    }

    template <std::convertible_to<uint32_t> Sz, std::convertible_to<uint8_t> ... Vs>
    constexpr Maze(Sz w, Sz h, Vs ... vs)
            : width    { static_cast<uint32_t>(w) },
              height   { static_cast<uint32_t>(h) },
              cells_ { static_cast<uint8_t>(vs)... }
    {
        assert(sizeof...(Vs) == w * h);
    }

    constexpr uint64_t index_of(Point p) const noexcept
    {
        return p.x + p.y * width;
    }

    constexpr Point point_of(uint64_t idx) const noexcept
    {
        return {
                uint32_t(idx % width),
                uint32_t(idx / width)
        };
    }

    constexpr uint8_t & at(uint64_t i) noexcept
    {
        return cells_.at(i);
    }

    constexpr const uint8_t & at(uint64_t i) const noexcept
    {
        return cells_.at(i);
    }

    constexpr uint8_t & at(Point p) noexcept
    {
        return cells_.at(p.x + p.y * width);
    }

    constexpr const uint8_t & at(Point p) const noexcept
    {
        return cells_.at(p.x + p.y * width);
    }

    constexpr bool path_at(uint64_t i) const noexcept { return at(i) == Maze::path; };
    constexpr bool path_at(Point p) const noexcept { return at(p) == Maze::path; };

    constexpr uint64_t size() const noexcept { return cells_.size(); };

private:
    std::vector<uint8_t> cells_;
};

//void print_maze(const Maze& maze) {
//    for (uint32_t y = 0; y < maze.height; y++)  {
//        for (uint32_t x = 0; x < maze.width; x++)
//            switch (maze.at({x, y})) {
//                case Maze::path: std::cout << " "; break;
//                case Maze::wall: std::cout << "#"; break;
//                case Maze::solution: std::cout << "·"; break;
//                default: std::cout << "?";
//            };
//        std::cout << '\n';
//    }
//}
}; // namespace mazes