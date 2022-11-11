#pragma once
#include <directedgraph.hpp>
#include <optional>
#include <path_type.hpp>

namespace mazes {

template <typename Func, typename Ret, typename ... Args>
concept impl_CallableWithSignature = requires(Func f, Args... args){
   { f(args...) } -> std::same_as<Ret>;
};

template <typename Func, typename T>
struct struct_CallableWithSignature;

template <typename Func, typename Ret, typename ... Args>
struct struct_CallableWithSignature<Func, Ret(Args...)> {
   static constexpr bool value = impl_CallableWithSignature<Func, Ret, Args...>;
};

template <typename Func, typename Sig>
concept CallableWithSignature = struct_CallableWithSignature<Func, Sig>::value;


/// @return implementation: whether the algorithm should continue
template<typename D, size_t N>
constexpr bool depth_first_impl(
        const DirectedGraph<D, N>& graph,
         PathType<DirectedGraph<D, N>>& path,
        std::vector<bool> &visited,
        const uint64_t from, const uint64_t to) {
    if (visited[from]) return false;

    path.push_back(from);
    visited[from] = true;

    if (from == to)
        return true;


    const Edges auto &edges = graph.edges(from);
    for (uint64_t i = 0; i < edges.size(); i++) {
        const uint64_t e = edges[i];
        if (depth_first_impl(graph, path, visited, e, to))
            return true;
    }

    visited[from] = false;

    path.pop_back();
    return false;
}

template<typename D, size_t N>
constexpr std::optional<PathType<DirectedGraph<D, N>>> depth_first_search(
        const DirectedGraph<D, N> &graph,
        const uint64_t from,
        const uint64_t to) {
    std::vector<bool> visited(graph.size());
    PathType<DirectedGraph<D, N>> path;
    bool success = depth_first_impl(graph, path, visited, from, to);
    if (success) {
        return path;
    } else return {};
}

template<typename D, size_t N, CallableWithSignature<void(const PathType<DirectedGraph<D, N>>&)> Func>
constexpr void depth_first_all_impl(
        const DirectedGraph<D, N>& graph,
        PathType<DirectedGraph<D, N>>& path,
        std::vector<bool>& visited,
        const uint64_t from, const uint64_t to,
        Func &&on_finished_path) {
    if (visited[from]) return;

    path.push_back(from);
    visited[from] = true;

    if (from == to) {
        on_finished_path(path);
    } else {
        const auto &edges = graph.edges(from);
        for (uint64_t i = 0; i < edges.size(); i++) {
            depth_first_all_impl(graph, path, visited, edges[i], to, on_finished_path);
        }
    }

    path.pop_back();
    visited[from] = false;
}

template<typename D, size_t N>
constexpr std::vector<PathType<DirectedGraph<D, N>>> find_all_paths(
        const DirectedGraph<D, N> &graph,
        const uint64_t from, const uint64_t to) {
    std::vector<PathType<DirectedGraph<D, N>>> result;
    PathType<DirectedGraph<D, N>> work_path;
    std::vector<bool> visited(graph.size());
    depth_first_all_impl(
            graph, work_path, visited,
            from, to, [&result](const PathType<DirectedGraph<D, N>>& np) {
                result.push_back(np);
            }
    );
    return result;
}
};