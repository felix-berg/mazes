// Created by felix on 11/10/22.

#pragma once

#include <maze.hpp>
#include <directedgraph.hpp>

namespace mazes {
// Graph type used for mazes
using MazeGraph = DirectedGraph<Point, 4>;

/// @brief A valid maze is defined by having one hole in the top, one in the bottom,
///        and completely intact walls on the left and right
bool valid_maze(const Maze& maze);


/// @brief Construct graph from given maze
/// @return graph with nodes and edges corresponding to decision points in maze
MazeGraph graph_from_maze(const Maze& maze);
} // namespace mazes