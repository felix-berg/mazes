#pragma once
#include <directedgraph.hpp>
#include <optional>
#include <callable.hpp>
#include <path_type.hpp>

namespace mazes {

struct DepthFirst {
    /// @return implementation: whether the algorithm should continue
    template <typename D, uint64_t N>
    static constexpr bool impl(
        const DirectedGraph<D, N>& graph,
        PathType <DirectedGraph<D, N>>& path,
        std::vector<bool>& visited,
        const uint64_t from, const uint64_t to)
    {
        if (visited[from]) return false;

        path.push_back(from);
        visited[from] = true;

        if (from == to)
            return true;

        const Edges auto& edges = graph.edges(from);
        for (uint64_t i = 0; i < edges.size(); i++) {
            const uint64_t e = edges[i];
            if (impl(graph, path, visited, e, to))
                return true;
        }

        visited[from] = false;

        path.pop_back();
        return false;
    }

    template <typename D, size_t N,
        CallableWithSignature<void(const PathType<DirectedGraph<D, N>>&)> OnFindFunc>
    static constexpr void find_all_paths_unstoppable(
        const DirectedGraph<D, N>& graph,
        PathType<DirectedGraph<D, N>>& path,
        std::vector<bool>& visited,
        const uint64_t from, const uint64_t to,
        OnFindFunc&& on_find)
    {
        if (visited[from]) return;

        path.push_back(from);
        visited[from] = true;

        if (from == to) {
            on_find(path);
        } else {
            const auto& edges = graph.edges(from);
            for (uint64_t i = 0; i < edges.size(); i++) {
                find_all_paths_unstoppable(graph, path, visited, edges[i], to, on_find);
            }
        }

        path.pop_back();
        visited[from] = false;
    }

    template <typename D, size_t N,
        CallableWithSignature<bool(const PathType<DirectedGraph<D, N>>&)> OnFindFunc>
    static constexpr bool find_all_paths_stoppable(
        const DirectedGraph<D, N>& graph,
        PathType<DirectedGraph<D, N>>& path,
        std::vector<bool>& visited,
        const uint64_t from, const uint64_t to,
        OnFindFunc&& on_find
        )
    {
        if (visited[from]) return true;

        path.push_back(from);
        visited[from] = true;

        if (from == to) {
            on_find(path);
        } else {
            const auto& edges = graph.edges(from);
            for (uint64_t i = 0; i < edges.size(); i++) {
                if (!find_all_paths_stoppable(graph, path, visited, edges[i], to, on_find))
                    return false;
            }
        }

        path.pop_back();
        visited[from] = false;
        return true;
    }

public:
    template <typename D, uint64_t N>
    static constexpr std::optional<PathType < DirectedGraph<D, N>>>
    search(const DirectedGraph<D, N>& graph,
        const uint64_t from,
        const uint64_t to)
    {
        std::vector<bool> visited(graph.size());
        PathType <DirectedGraph<D, N>> path;
        bool success = impl(graph, path, visited, from, to);
        if (success) {
            return path;
        } else return {};
    }

    template <typename D, uint64_t N, CallableWithSignature<bool(const PathType<DirectedGraph<D, N>>&)> OnFindFunc>
    // Search for all paths in directed graph.
    // calls on_find when a path is found. If
    // on_find returns false, the search is
    // stopped.
    static constexpr void search_and_continue(
        const DirectedGraph<D, N>& graph,
        const uint64_t from,
        const uint64_t to,
        OnFindFunc&& on_find)
    {
        PathType<DirectedGraph<D, N>> path;
        std::vector<bool> visited(graph.size());
        find_all_paths_stoppable(graph, path, visited, from, to, on_find);
    }

    template <typename D, uint64_t N,
        CallableWithSignature<void(const PathType<DirectedGraph<D, N>>&)> OnFindFunc>
    /* Search for all paths in directed graph -> calls on_find when a path is found. */
    static constexpr void search_and_continue(
        const DirectedGraph<D, N>& graph,
        const uint64_t from, const uint64_t to,
        OnFindFunc&& on_find
        )
    {
        PathType<decltype(graph)> path;
        std::vector<bool> visited(graph.size());
        find_all_paths_unstoppable(graph, path, visited, from, to, on_find);
    }
};
};