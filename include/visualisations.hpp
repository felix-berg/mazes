#pragma once

#include <mazegraph.hpp>
#include <path_type.hpp>
#include <fbg.hpp>

namespace mazes {


struct VisualMaze : fbg::Context {
   const float cellw, cellh;
   std::vector<fbg::Rect> rectangles;

   VisualMaze(fbg::Window & window, const Maze& maze)
      : cellw { float(window.width()) / float(maze.width) },
        cellh { float(window.height()) / float(maze.height) }
   {
      const fbg::Rgba wall_color = { 255, 255, 255, 255 };
      const fbg::Rgba path_color = { 0, 0, 0, 255 };
      rectangles.reserve(maze.size());

      for (uint32_t y = 0; y < maze.height; y++) {
         for (uint32_t x = 0; x < maze.width; x++) {
            float midx = x * cellw + cellw / 2.0f;
            float midy = y * cellh + cellh / 2.0f;
            rectangles.push_back(fbg::Rect(midx, midy, cellw, cellh));
            auto & r = rectangles.back();
            r.noStroke();

            switch (maze.at({x,y})) {
               case Maze::wall: r.fill(wall_color); break;
               case Maze::solution: case Maze::path:
                  r.fill(path_color); break;
               default: r.fill({255, 0, 0, 255});
            };
         }
      }

      for (auto & r : rectangles)
         Context::attach(r);
   }
};

struct VisualMazeGraph : fbg::Context {
   std::vector<fbg::Circle> nodes;
   std::vector<fbg::Line> lines;

   VisualMazeGraph(fbg::Window& window, const Maze& maze, const MazeGraph& graph)
   {
      const float cellw { float(window.width()) / float(maze.width) };
      const float cellh { float(window.height()) / float(maze.height) };

      for (uint64_t i = 0; i < graph.size(); i++) {
         const Point point = graph.node(i);
         const auto &    connections = graph.edges(i);
         nodes.push_back({
            float(point.x) * cellw + cellw / 2.0f,
            float(point.y) * cellh + cellh / 2.0f,
            cellw / 2.5f });

         for (uint8_t oi = 0; oi < connections.size(); oi++) {
            const Point o = graph.node(connections[oi]);
            lines.push_back({
               float(point.x) * cellw + cellw / 2.0f,
               float(point.y) * cellh + cellh / 2.0f,
               float(o.x) * cellw + cellw / 2.0f,
               float(o.y) * cellh + cellh / 2.0f
            });
         }
      }

      for (auto & n : nodes)
         Context::attach(n);

      for (auto & l : lines)
         Context::attach(l);

      for (auto & l : lines) {
         l.stroke({0, 0, 255, 255});
         l.strokeweight(cellw / 8 + 1);
      }

      for (auto & c : nodes) {
         c.noFill();
         c.stroke({0, 0, 255, 255});
         c.strokeweight(cellw / 12 + 1);
      }
   }
   virtual ~VisualMazeGraph() { };
};

struct VisualPath : fbg::Context {
    std::vector<fbg::Circle> circles;
    std::vector<fbg::Line> lines;

    VisualPath(fbg::Window& window, const Maze& maze, const MazeGraph& graph, const PathType <MazeGraph>& path, const
    fbg::Rgba col = { 0, 0, 255, 255 })
    {
        const float cellw { float(window.width()) / float(maze.width) };
        const float cellh { float(window.height()) / float(maze.height) };

        for (auto & step : path) {
            const Point p = graph.node(step);
            circles.push_back({
                p.x * cellw + cellw / 2,
                p.y * cellh + cellh / 2,
                cellw / 6.0f + 1
            });
            circles.back().noStroke();
            circles.back().fill(col);
        }

        for (uint64_t i = 0; i < circles.size() - 1; i++) {
            lines.push_back({
                circles[i].pos(),
                circles[i + 1].pos()
            });
            lines.back().strokeweight(cellw / 12.0f + 1);
            lines.back().stroke(col);
        }

        for (auto & c : circles)
            Context::attach(c);
        for (auto & l : lines)
            Context::attach(l);
    }
};

//struct VisualPath : fbg::Context {
//
//    std::vector<fbg::Line> lines;
//
//    VisualPath(fbg::Window & window, const Maze& maze, const MazeGraph& graph, const PathType<MazeGraph>& path)
//    {
//         fbg::Line::DEFAULT_LINEMODE = fbg::LineMode::SMOOTH;
//        const float cellw { float(window.width()) / float(maze.width) };
//        const float cellh { float(window.height()) / float(maze.height) };
//
//        const Point * prev = nullptr;
//        for (uint64_t i : path) {
//            const Point * p = &graph.node(i);
//            if (prev)
//                lines.push_back({
//                                    float(prev->x) * cellw + cellw / 2.0f,
//                                    float(prev->y) * cellh + cellh / 2.0f,
//                                    float(p->x) * cellw + cellw / 2.0f,
//                                    float(p->y) * cellh + cellh / 2.0f
//                                });
//
//            prev = p;
//        }
//
//        for (auto & l : lines) {
//            l.stroke({0, 0, 255, 255});
//            l.strokeweight(cellw / 4);
//            Context::attach(l);
//        }
//    }
//
//   void hide(uint64_t index) {
//       lines.at(index).stroke({0, 0, 0, 0});
//   }
//
//   void show(uint64_t index) {
//       lines.at(index).stroke({0, 0, 255, 255});
//   }
//
//   void show(uint64_t from, uint64_t to) {
//        for (uint64_t i = 0; i < from; i++)
//            hide(i);
//        for (uint64_t i = from; i < to; i++)
//            show(i);
//        for (uint64_t i = to; i < lines.size(); i++)
//            hide(i);
//   }
//
//   VisualPath(VisualPath&&) = default;
//};
}
