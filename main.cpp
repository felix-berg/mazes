#include <fbg.hpp>
#include <mazegraph.hpp>
#include <visualisations.hpp>
#include <algorithms/depth_first.hpp>
#include <find_all_paths.hpp>
#include <algorithms/dijkstra.hpp>

#include <chrono>

int main() {
    using namespace mazes;

    const Maze maze(101, 101, {
        #include "101x101.txt"
    });

    const MazeGraph graph = graph_from_maze(maze);

    clock_t start_t = clock(), end_t;
    uint64_t iterations = 100000;

    PathType<MazeGraph> path;
    for (uint64_t i = 0; i < iterations; i++) {
        path =
            Dijkstra::search<long>(graph, 0, graph.size() - 1,
                [&graph](const uint64_t f, const uint64_t t) -> long {
                    const Point p = graph.node(f),
                                o = graph.node(t);
                    if (p.x == o.x)
                        return std::abs(int(p.y - o.y));
                    else
                        return std::abs(int(p.x - o.x));
                }
            ).value();
        assert(path.size() != 0);
    }

    end_t = clock();
    std::cout << "Took " << (end_t - start_t) << " µs, "
              << float(end_t - start_t) / float(CLOCKS_PER_SEC) << " seconds.\n";


    const auto paths = find_all_paths(graph, 0, graph.size() - 1);
    std::cout << "Solutions: " << paths.size() << '\n';

    fbg::LoopWin win ("Maze solver", 800, 800);

    VisualMaze vmaze(win, maze);
    win.attach(vmaze);

//    VisualMazeGraph vmaze_g(win, maze, graph);
//    win.attach(vmaze_g);

    VisualPath vpath(win, maze, graph, path);
    win.attach(vpath);

    uint64_t ticks = 0;
    win.draw = [&](float) {
//        vpath.show(0, ticks % vpath.lines.size());


        ticks++;
    };


    win.run();
    log_window_performance(win);
    return 0;
}
