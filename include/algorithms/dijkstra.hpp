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
class Dijkstra {
    template <typename EdgeLengthType>
    struct PQElement {
        uint64_t node; /* node of graph */
        uint64_t via_elmidx; /* index into finished elements */
        EdgeLengthType pathlen; /* combined pathlength up until node */

        constexpr PQElement() { };
        constexpr PQElement(uint64_t n, uint64_t v, EdgeLengthType pl)
            : node { n }, via_elmidx { v }, pathlen { pl } { };
    };

    static constexpr auto always_one = [](uint64_t, uint64_t) -> long { return long(1); };

public:
    template <typename EdgeLengthType = long, typename D, uint64_t N,
        CallableWithSignature<EdgeLengthType(uint64_t, uint64_t)> EdgeLength>
    /// Dijkstra's shortest path between from and to
    /// \tparam EdgeLengthType Return type of get_edge_length, the type of pathlengths
    /// \param get_edge_length A function that gets to adjacent nodes and computes the length of the edge between them
    /// \return Shortest path between nodes from and to, or std::nullopt, if no path is found
    /// \remark 4097294 µs, 4.09729 seconds for 100000 iterations on 101x101 maze with >14000 solutions
    ///         (more than double speed)
    static constexpr std::optional<PathType<DirectedGraph<D,N>>> search(
        const DirectedGraph<D, N>& graph,
        const uint64_t from, const uint64_t to,
        EdgeLength&& get_edge_length
        )
    {
        std::cout << "Running Dijkstra\n";
        using PQElm = PQElement<EdgeLengthType>;
        constexpr uint64_t via_none = UINT64_MAX;

        /* Priority queue is split into two sections:
         * |finished elements|priorityqueue|
         *                    ^ beginidx
         * finished nodes values are constant */
        std::vector<PQElm> pqueue { { from, via_none, EdgeLengthType() } };
        uint64_t beginidx = 0;
        pqueue.reserve(graph.size() / 12 + 1);

        std::vector<bool> added(graph.size(), 0);

        /* insert element into sorted priority queue (by pathlen)*/
        const auto insert_sorted = [&pqueue, &beginidx](PQElm&& elm) {
            const auto p = std::find_if(
                pqueue.begin() + beginidx, pqueue.end(),
                [&elm](const PQElm& other) {
                    return elm.pathlen < other.pathlen;
            });
            pqueue.insert(p, elm);
        };

        bool path_found = false;
        while (beginidx < pqueue.size()) { /* while queue is not empty*/
            const PQElm element = pqueue.at(beginidx);
            const uint64_t elmidx = beginidx;
            beginidx++;

            if (element.node == to) {
                path_found = true;
                break;
            }


            const Edges auto& edges = graph.edges(element.node);
            for (const uint64_t e : edges) {
                if (added[e]) continue;
                added[e] = true;
                const EdgeLengthType elen = get_edge_length(element.node, e);
                const EdgeLengthType pathlen = element.pathlen + elen;
                insert_sorted(PQElm(e, elmidx, pathlen));
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

    template <typename D, uint64_t N>
    static constexpr std::optional<PathType<DirectedGraph<D,N>>> search(
        const DirectedGraph<D, N>& graph,
        const uint64_t from, const uint64_t to
        )
    {
        return Dijkstra::search<long>(graph, from, to, always_one);
    }

    template <typename EdgeLengthType = long, typename D, uint64_t N,
        CallableWithSignature<EdgeLengthType(uint64_t, uint64_t)> EdgeLength>
    /// Dijkstra's shortest path between from and to
    /// \tparam EdgeLengthType Return type of get_edge_length, the type of pathlengths
    /// \param get_edge_length A function that gets to adjacent nodes and computes the length of the edge between them
    /// \return Shortest path between nodes from and to, or std::nullopt, if no path is found
    /// \remark 10977375 µs, 10.9774 seconds for 100000 iterations on 101x101 maze with >14000 solutions
    static constexpr std::optional<PathType<DirectedGraph<D, N>>> search_deque(
        const DirectedGraph<D, N>& graph,
        const uint64_t from, const uint64_t to,
        EdgeLength&& get_edge_length
    )
    {
        using PQElm = PQElement<EdgeLengthType>;

        /* index indicating no connection */
        constexpr uint64_t via_none = UINT64_MAX;

        std::vector<bool> in_queue(graph.size(), 0);
        in_queue[from] = true;

        /* stack of finished priorityqueue elements*/
        std::vector<PQElm> finished;

        /* priority queue -> starts with the first node: from */
        std::deque<PQElm> pqueue { { from, via_none, EdgeLengthType() }};

        /* insert element into priority queue, sorted by current path length. */
        const auto insert_sorted = [&pqueue](PQElm&& elm) {
            const auto p = std::find_if(pqueue.begin(), pqueue.end(),
                [&elm](const PQElm& o)
                { return elm.pathlen < o.pathlen; });
            pqueue.insert(p, elm);
        };

        bool path_found = false;
        while (!pqueue.empty()) {
            const PQElm elm = pqueue.front();
            pqueue.pop_front();

            finished.push_back(elm);
            const uint64_t elmidx = finished.size() - 1;

            if (elm.node == to) {
                /* Found correct node, therefore shortest path. */
                path_found = true;
                break;
            }

            const Edges auto & edges = graph.edges(elm.node);
            for (uint64_t e : edges) {
                if (in_queue[e]) continue;
                in_queue[e] = true;

                const EdgeLengthType el = get_edge_length(elm.node, e);
                const EdgeLengthType pathlen = elm.pathlen + el;
                insert_sorted({ e, elmidx, pathlen });
            }
        }

        if (!path_found) return std::nullopt;

        /* reconstruct path back to from */
        const PQElm toelm = finished.back();
        PathType<DirectedGraph<D, N>> path { toelm.node };
        uint64_t viaidx = toelm.via_elmidx;
        while (viaidx != via_none) { /* until viaidx points to from */
            path.push_back(finished.at(viaidx).node);
            viaidx = finished.at(viaidx).via_elmidx;
        }

        std::reverse(path.begin(), path.end());

        return path;
    }
};
}; // namespace mazes