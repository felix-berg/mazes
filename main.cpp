#include <fbg.hpp>
#include <mazegraph.hpp>
#include <visualisations.hpp>
#include <algorithms/depth_first.hpp>
#include <find_all_paths.hpp>

#include <chrono>

int main() {
    using namespace mazes;

    const Maze m(101, 101, {
        #include "101x101.txt"
    });

    const MazeGraph mg = graph_from_maze(m);
    const auto paths = find_all_paths(mg, 0, mg.size() - 1);
    std::cout << "Number of solutions: " << paths.size() << '\n';

    fbg::LoopWin win ("Maze solver", 1000, 1000);

    VisualMaze vm(win, m);
    win.attach(vm);

    std::vector<VisualPath> vps;
    for (auto & p : paths)
        vps.push_back(VisualPath(win, m, mg, p));


    for (auto & vp : vps)
        win.attach(vp);

    win.draw = [&](float) {

    };


    win.run();
    log_window_performance(win);
    return 0;
}
