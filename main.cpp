#include <fbg.hpp>
#include <mazegraph.hpp>
#include <visualisations.hpp>
#include <algorithms/depth_first.hpp>
#include <algorithms/dijkstra.hpp>
#include <algorithms/astar.hpp>
#include <find_all_paths.hpp>

#include <chrono>

int main() {
    using namespace mazes;

    const Maze maze(21, 21, {
        #include "21x21.txt"
    });

    const MazeGraph graph = graph_from_maze(maze);
    const uint64_t from = 0, to = graph.size() - 1;

    const auto map = [](float x, float fmin, float fmax, float tmin, float tmax) {
        const float fr = fmax - fmin;
        const float tr = tmax - tmin;
        return ((x - fmin) * tr) / fr + tmin;
    };

    float maxdiffx = graph.node(to).x;
    float maxdiffy = graph.node(to).y;
    float maxdiffsz = maxdiffx * maxdiffx + maxdiffy * maxdiffy;

    const auto edgelen = [&map, maxdiffx, maxdiffy, &graph](const uint64_t a, const uint64_t b) -> float {
        const Point p = graph.node(a), o = graph.node(b);
        if (p.x == o.x)
            return map(std::abs((long) p.y - (long) o.y), 0.0f, maxdiffy, 0.0f, 10.0f);
        else
            return map(std::abs((long) p.x - (long) o.x), 0.0f, maxdiffx, 0.0f, 10.0f);
    };

    const auto dist = [&map, &graph, maxdiffsz, endp = graph.node(to)](const uint64_t n) -> float {
        const Point diff = {
            endp.x - graph.node(n).x,
            endp.y - graph.node(n).y
        };
        const float v = diff.x * diff.x + diff.y * diff.y;
        return map(v, 0.0f, maxdiffsz, 0.0f, 1000.0f);
    };

    clock_t as = clock(), ae, ds, de;
    const auto apath = AStar::search<float>(graph, from, to,
        edgelen, dist).value();
    ae = (ds = clock());
    const auto dpath = Dijkstra::search<float>(graph, from, to, edgelen).value();
    de = clock();

    std::cout << "A* time: " << ae - as << '\n';
    std::cout << "Ds time: " << de - ds << '\n';

    assert(std::equal(apath.begin(), apath.end(), dpath.begin(), dpath.end()));

//    fbg::LoopWin win ("Maze solver", 1200, 1200);
//
//    VisualMaze vmaze(win, maze);
//    win.attach(vmaze);
//
////    VisualMazeGraph vmaze_g(win, maze, graph);
////    win.attach(vmaze_g);
//
//    VisualPath vpath(win, maze, graph, apath);
//    win.attach(vpath);
//
//    uint64_t ticks = 0;
//    win.draw = [&](float) {
////        vpath.show(0, ticks % vpath.lines.size());
//
//
//        ticks++;
//    };
//
//
//    win.run();
//    log_window_performance(win);
    return 0;
}
