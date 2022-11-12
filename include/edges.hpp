// Created by felix on 11/10/22.
#pragma once

#include <cstdint>
#include <vector>
#include <array>
#include <algorithm>

namespace mazes {

template<typename A>
concept Edges =requires(A adj, uint64_t node, size_t index) {
    { adj.add_adjacency(node) } -> std::same_as<void>;
    { adj.remove_adjacency(node) } -> std::same_as<void>;
    { adj.size() } -> std::same_as<std::size_t>;
    { adj.at(index) } -> std::same_as<uint64_t &>;
    { adj[index] } -> std::same_as<uint64_t &>;
    { adj.begin() } -> std::random_access_iterator;
    { adj.end() } -> std::random_access_iterator;
    { *adj.begin() } -> std::same_as<uint64_t &>;
};

/// @brief Adjacency-list with static size
/// @tparam Size size of list
template<uint64_t Size>
struct StaticEdges
        : private std::array<uint64_t, Size> {
    using container = std::array<uint64_t, Size>;
    static constexpr uint64_t nullidx = UINT64_MAX;
    uint64_t sz_;

    constexpr StaticEdges()
            : std::array<uint64_t, Size>{},
              sz_{0} {
    }

    constexpr void add_adjacency(uint64_t n) noexcept {
        assert(size() < 4);
        container::at(sz_++) = n; /* "push back" */
    }

    constexpr void remove_adjacency(uint64_t n) noexcept {
        const auto p = std::find(begin(), end(), n);
        assert(p != end() && "Edge not found");

        for (auto it = p; it < end() - 1; ++it)
            *p = *(p + 1);
    }

    using container::begin;

    constexpr container::iterator end() noexcept {
        return container::begin() + sz_;
    }

    constexpr container::const_iterator end() const noexcept {
        return container::begin() + sz_;
    }

    using container::at;
    using container::operator[];

    constexpr uint64_t size() const noexcept {
        return sz_;
    }
};

struct DynamicEdges : protected std::vector<uint64_t> {
    using container = std::vector<uint64_t>;

    constexpr void add_adjacency(uint64_t n) noexcept {
        container::push_back(n);
    };

    constexpr void remove_adjacency(uint64_t n) noexcept {
        container::erase(
                std::remove(container::begin(), container::end(), n),
                container::end());
    }

    using container::operator[];
    using container::at;
    using container::size;
    using container::begin, container::end;
};

template<uint64_t MaxAdjacencies>
consteval auto get_edges_type() noexcept {
    if constexpr (MaxAdjacencies <= 4) {
        return StaticEdges<MaxAdjacencies>();
    } else {
        return DynamicEdges();
    }
}

template<uint64_t MaxEdgesPerNode>
using GetEdgesType
        = decltype(get_edges_type<MaxEdgesPerNode>());

} // namespace mazes