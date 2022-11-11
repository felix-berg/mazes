// Created by felix on 11/11/22.

#pragma once

#include <algorithms/depth_first.hpp>

namespace mazes {
template<typename D, size_t N>
constexpr std::vector<PathType<DirectedGraph<D, N>>> find_all_paths(
    const DirectedGraph<D, N> &graph,
    const uint64_t from, const uint64_t to)
{
    std::vector<PathType<DirectedGraph<D, N>>> paths;
    const auto on_find = [&paths](const auto & new_path) -> bool {
        paths.push_back(new_path);
    };

    DepthFirst::search_and_continue(graph, from, to, on_find);
    return paths;
}
};