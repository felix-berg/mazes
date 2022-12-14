#include <fbg.hpp>
#include <mazegraph.hpp>
#include <visualisations.hpp>
#include <algorithms/depth_first.hpp>
#include <algorithms/breadth_first.hpp>
#include <algorithms/dijkstra.hpp>
#include <algorithms/astar.hpp>
#include <find_all_paths.hpp>

#include <chrono>

int main() {
    using namespace mazes;

    const Maze maze(101, 101, {
        #include "101x101.txt"
    });

    using namespace std::chrono;
    // microseconds
    time_point<high_resolution_clock, duration<double, std::ratio<1, 1000000>>> startTime, endTime;
    startTime = high_resolution_clock::now();
    const MazeGraph graph = graph_from_maze(maze);
    endTime = high_resolution_clock::now();
    std::cout << "Created graph with " << graph.nodes().size() << " nodes in " << endTime - startTime << ".\n";
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

    startTime = high_resolution_clock::now();
    const auto dpath = Dijkstra::search<float>(graph, from, to, edgelen).value();
    endTime = high_resolution_clock::now();
    std::cout << "Dijkstra took " << endTime - startTime << "\n";

    startTime = high_resolution_clock::now();
    const auto apath = AStar::search<float>(graph, from, to, edgelen, dist).value();
    endTime = high_resolution_clock::now();
    std::cout << "A* took " << endTime - startTime << "\n";

    startTime = high_resolution_clock::now();
    const auto dfpath = DepthFirst::search(graph, from, to).value();
    endTime = high_resolution_clock::now(),
    std::cout << "DF took " << endTime - startTime << "\n";

    startTime = high_resolution_clock::now();
    const auto bfpath = BreadthFirst::search(graph, from, to).value();
    endTime = high_resolution_clock::now(),
    std::cout << "BF took " << endTime - startTime << "\n";


    fbg::LoopWin win ("Maze solver", 1200, 1200);

    VisualMaze vmaze(win, maze);
    win.attach(vmaze);

    /* should both be shortest path */
    assert(std::equal(apath.begin(), apath.end(), dpath.begin(), dpath.end()));

    VisualMazeGraph vmaze_g(win, maze, graph);
    win.attach(vmaze_g);

    std::vector<VisualPath> vpaths;
    vpaths.push_back({ win, maze, graph, dpath , { 255, 0, 0, 255 }});
    vpaths.push_back({ win, maze, graph, apath , { 0, 255, 0, 255 }});
    vpaths.push_back({ win, maze, graph, dfpath, { 0, 0, 255, 255 }});
    vpaths.push_back({ win, maze, graph, bfpath, { 255, 255, 255, 255 }});

    uint64_t ticks = 0;
    uint64_t which = 0;
    win.draw = [&](float) {
        for (uint64_t i = 0; i < vpaths.size(); i++) {
            if (i != which)
                win.detach(vpaths.at(i));
            else
                win.attach(vpaths.at(i));
        }
        if (which == 4)
            win.attach(vmaze_g);
        else
            win.detach(vmaze_g);
        ticks++;
        which = (ticks % 250) / 50;
    };


    win.run();
    return 0;
}
