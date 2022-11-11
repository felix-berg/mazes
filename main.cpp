#include <fbg.hpp>
#include <mazegraph.hpp>
#include <visualisations.hpp>
#include <depth_first.hpp>

#include <chrono>

int main() {
    using namespace mazes;

    const Maze m(21, 21, {
        #include "21x21.txt"
    });

    const MazeGraph mg = graph_from_maze(m);
    const auto paths
        = find_all_paths(mg, 0, mg.size() - 1);

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
