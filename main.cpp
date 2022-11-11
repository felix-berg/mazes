#include <fbg.hpp>
#include <mazegraph.hpp>
#include <visualisations.hpp>
#include <depth_first.hpp>

#include <chrono>

struct BenchmarkInfo {
};


int main() {
    using namespace mazes;

    Maze m(21, 21, {
        #include "21x21.txt"
    });

    MazeGraph mg = graph_from_maze(m);
    std::vector<GraphPath> paths = find_all_paths(mg, 0, mg.size() - 1);

    fbg::LoopWin win ("Maze solver", 1000, 1000);

    VisualMaze vm(win, m);
    win.attach(vm);

    std::vector<VisualPath> vps;
    for (auto & p : paths)
        vps.push_back(VisualPath(win, m, mg, p));

    std::cout << "Num: " << paths.size() << '\n';

    for (auto & vp : vps)
        win.attach(vp);

    win.run();
    return 0;
}
