// Created by felix on 11/12/22.

#pragma once

#include <directedgraph.hpp>
#include <path_type.hpp>
#include <optional>

namespace mazes {
class BreadthFirst {
    struct QueueElement {
        uint64_t node;
        uint64_t via_elmidx;
    };
public:
    template <typename D, uint64_t N>
    static constexpr std::optional<PathType<DirectedGraph<D, N>>> search(
        const DirectedGraph<D,N>& graph,
        const uint64_t from, const uint64_t to
        )
    {
        static constexpr uint64_t via_none = UINT64_MAX;
        std::vector<QueueElement> queue { { from, via_none } };
        std::vector<bool> visited(graph.size());
        visited[from] = true;
        uint64_t beginidx = 0;

        bool path_found = false;
        while (beginidx < queue.size()) {
            /* "pop front" */
            const QueueElement element = queue.at(beginidx);
            const uint64_t elmidx = beginidx;
            beginidx++;

            if (element.node == to) {
                path_found = true; break;
            }

            const Edges auto & edges = graph.edges(element.node);
            for (const uint64_t e : edges) {
                if (visited[e]) continue;
                visited[e] = true;
                queue.push_back({ e, elmidx });
            }
        }

        if (!path_found) return std::nullopt;

        const uint64_t finished_end = beginidx;
        const QueueElement& to_elm = queue.at(finished_end - 1);

        PathType<DirectedGraph<D,N>> path = { to_elm.node };
        path.reserve(graph.size() / 24 + 1);

        /* reconstruct path from last node, moving backward through via_elmidx */
        uint64_t viaidx = to_elm.via_elmidx;
        while (viaidx != via_none) {
            const QueueElement& elm = queue.at(viaidx);
            path.push_back(elm.node);
            viaidx = elm.via_elmidx;
        }

        return path;
    }
};

} // namespace mazes