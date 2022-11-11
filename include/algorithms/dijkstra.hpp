// Created by felix on 11/11/22.

#pragma once

#include <optional>
#include <algorithm>

#include <directedgraph.hpp>
#include <path_type.hpp>
#include <callable.hpp>
#include <deque>

namespace mazes {
struct Dijkstra {
public:
    struct PQElement {
        uint64_t node;
        uint64_t via_fi; /* index into "finished */
        long pathlen;
    };

    template <typename D, uint64_t N, CallableWithSignature<long(uint64_t, uint64_t)> EdgeLength>
    /// Dijkstra's shortest path between from and to
    /// \param get_edge_length A function that gets to adjacent nodes and computes the length of the edge between them
    /// \return Shortest path between nodes from and to, or std::nullopt, if no path is found
    static constexpr std::optional<PathType<DirectedGraph<D, N>>> search(
        const DirectedGraph<D, N>& graph,
        const uint64_t from, const uint64_t to,
        EdgeLength&& get_edge_length
        )
    {
        constexpr uint64_t via_none = UINT64_MAX;

        std::vector<bool> visited(graph.size(), 0);
        std::vector<PQElement> finished;
        std::deque<PQElement> pqueue { { from, via_none, 0 }};

        const auto insert_sorted = [&pqueue](PQElement&& elm) {
            const auto p = std::find_if(pqueue.begin(), pqueue.end(),
                         [&elm](const PQElement& o)
                         { return elm.pathlen < o.pathlen; });
            pqueue.insert(p, elm);
        };

        bool success = false;
        while (!pqueue.empty()) { /* "while not empty" */
            /* "pop front" */
            const PQElement elm = pqueue.front();
            pqueue.pop_front();

            finished.push_back(elm);
            const uint64_t elmidx = finished.size() - 1;
            visited[elm.node] = true;

            if (elm.node == to) {
                /* Found correct node, therefore shortest path. */
                success = true;
                break;
            }

            const Edges auto & edges = graph.edges(elm.node);
            for (uint64_t e : edges) {
                if (visited[e]) continue;

                const long el = get_edge_length(elm.node, e);
                const long pathlen = elm.pathlen + el;
                insert_sorted({ e, elmidx, pathlen });
            }
        }

        if (!success) return std::nullopt;

        /* reconstruct path from */
        const PQElement toelm = finished.back();
        PathType<DirectedGraph<D, N>> path { toelm.node };
        uint64_t viaidx = toelm.via_fi;
        while (viaidx != via_none) {
            path.push_back(finished.at(viaidx).node);
            viaidx = finished.at(viaidx).via_fi;
        }

        std::reverse(path.begin(), path.end());

        return path;
    }
};
}; // namespace mazes