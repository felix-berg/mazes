// Created by felix on 11/11/22.

#pragma once

#include <optional>
#include <algorithm>

#include <directedgraph.hpp>
#include <path_type.hpp>
#include <callable.hpp>
#include <deque>
#include <concepts>

namespace mazes {
class AStar {
    template <typename DistanceType>
    struct PQElement {
        uint64_t node; /* node of graph */
        uint64_t via_elmidx; /* index into finished elements */
        DistanceType pathlen; /* combined pathlength up until node */
        DistanceType total_heuristic;

        constexpr PQElement() { };
        constexpr PQElement(uint64_t n, uint64_t v, DistanceType pl, DistanceType th)
            : node { n }, via_elmidx { v }, pathlen { pl }, total_heuristic { th } { };
    };

public:
    template <typename DistanceType = float, typename D, uint64_t N,
        CallableWithSignature<DistanceType(uint64_t, uint64_t)> EdgeLength,
        CallableWithSignature<DistanceType(uint64_t)> Distance>
    /// A* shortest path between from and to
    /// \tparam EdgeLengthType Return type of get_edge_length, the type of pathlengths
    /// \param get_edge_length A function that gets to adjacent nodes and computes the length of the edge between them
    /// \return Shortest path between nodes from and to, or std::nullopt, if no path is found
    static constexpr std::optional<PathType<DirectedGraph<D,N>>> search(
        const DirectedGraph<D, N>& graph,
        const uint64_t from, const uint64_t to,
        EdgeLength&& get_edge_length,
        Distance&& get_distance_to_finish
        )
    {
        using PQElm = PQElement<DistanceType>;
        constexpr uint64_t via_none = UINT64_MAX;

        /* Priority queue is split into two sections:
         * |finished elements|priorityqueue|
         *                    ^ beginidx
         * finished nodes values are constant */
        std::vector<PQElm> pqueue { { from, via_none, DistanceType(), get_distance_to_finish(from) } };
        uint64_t beginidx = 0;
        pqueue.reserve(graph.size() / 12 + 1);

        std::vector<bool> visited(graph.size());

        /* insert element into sorted priority queue (by pathlen)*/
        const auto insert_sorted = [&pqueue, &beginidx](PQElm&& elm) {
            const auto p = std::find_if(
                pqueue.begin() + beginidx, pqueue.end(),
                [&elm](const PQElm& other) {
                    return elm.total_heuristic < elm.total_heuristic;
            });
            pqueue.insert(p, elm);
        };

        bool path_found = false;
        while (beginidx < pqueue.size()) { /* while queue is not empty*/
            /* "pop front" (and push to finished) */
            const PQElm& element = pqueue.at(beginidx);
            const uint64_t elmidx = beginidx;
            beginidx++;

            if (element.node == to) {
                path_found = true;
                break;
            }

            visited[element.node] = true;

            const Edges auto& edges = graph.edges(element.node);
            for (const uint64_t e : edges) {
                if (visited[e]) continue;
                const DistanceType elen = get_edge_length(element.node, e);
                const DistanceType pathlen = element.pathlen + elen;
                insert_sorted(PQElm(e, elmidx, pathlen, pathlen + get_distance_to_finish(e)));
            }
        }

        if (!path_found) return std::nullopt;

        const uint64_t finished_end = beginidx;
        const PQElm& to_elm = pqueue.at(finished_end - 1);

        PathType<DirectedGraph<D,N>> path = { to_elm.node };
        path.reserve(graph.size() / 24 + 1);

        /* reconstruct path from last node, moving backward through via_elmidx */
        uint64_t viaidx = to_elm.via_elmidx;
        while (viaidx != via_none) {
            const PQElm& elm = pqueue.at(viaidx);
            path.push_back(elm.node);
            viaidx = elm.via_elmidx;
        }

        return path;
    }
};
}; // namespace mazes