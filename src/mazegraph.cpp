// Created by felix on 11/10/22.
#include <mazegraph.hpp>

mazes::MazeGraph mazes::graph_from_maze(const Maze& maze) {
    assert(valid_maze(maze));
    MazeGraph graph;
    graph.reserve(maze.size() / 4);

    /* For each x: index of closest node in column looking up. */
    std::vector<uint64_t> prev_up_idxs(maze.width);

    /* Find entry point to maze */
    for (uint32_t x = 1; x < maze.width - 1; x++) {
        const Point p = { x, 0 };
        if (maze.path_at(p)) {
            uint64_t idx = graph.add_node(p);
            prev_up_idxs[x] = idx;
        }
    }


    /* Find all turns/dead ends in maze */
    for (uint32_t y = 1; y < maze.height - 1; y++) { /* reduced loop to avoid bounds checking */
        /* for each y: index of closets node in row looking to the left */
        uint64_t prev_left_idx = UINT64_MAX;
        for (uint32_t x = 1; x < maze.width - 1; x++) {
            if (maze.at({x,y}) != Maze::path)
                continue;

            /* Create bitmask where each bit represents whether
               there is path in that direction. (left, right, up, down)
               e.g. 0110 means path right and up. */
            uint8_t mask =
                    maze.path_at({x - 1, y})      | /* left  */
                    maze.path_at({x + 1, y}) << 1 | /* right */
                    maze.path_at({x, y - 1}) << 2 | /*  up   */
                    maze.path_at({x, y + 1}) << 3; /* down  */

            /* If passthrough (l-r) or (u-d), don't create node. */
            if (mask == 0b11 || mask == 0b1100)
                continue;

            const uint64_t new_index = graph.add_node({x, y});

            /* If path is open to the left */
            if (mask & 0b0001)
                graph.connect(prev_left_idx, new_index);

            /* If path is open up */
            if (mask & 0b0100)
                graph.connect(prev_up_idxs[x], new_index);

            /* Update previous node for row and column */
            prev_left_idx = new_index;
            prev_up_idxs[x] = new_index;
        }
    }

    /* Find exit point of maze */
    const uint32_t bottomy = maze.height - 1;
    for (uint32_t x = 1; x < maze.width - 1; x++) {
        const Point p = { x, bottomy };
        if (maze.path_at(p)) {
            auto idx = graph.add_node(p);
            /* Connects to path above */
            if (maze.path_at({x, bottomy - 1}))
                graph.connect(idx, prev_up_idxs[x]);
        }
    }

    return graph;
}

bool mazes::valid_maze(const mazes::Maze &maze) {
    /* check walls */
    for (uint32_t y = 0; y < maze.height; y++)
        if (maze.at({0, y}) != Maze::wall ||
            maze.at({maze.width - 1, y}) != Maze::wall)
            return false;

    /* check top */
    uint32_t n = 0;
    for (uint32_t x = 1; x < maze.width - 1; x++)
        n += maze.at(x) == Maze::path; /* .at(index): point -> index optimization (avoid multiply) */
    if (n != 1) return false;
    for (uint32_t x = 1; x < maze.width - 1; x++)
        n += maze.at({x, maze.height - 1}) == Maze::path;
    if (n != 2) return false;

    return true;
}

