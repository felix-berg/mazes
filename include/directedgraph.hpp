#pragma once

#include <cstdint>
#include <vector>
#include <array>
#include <cassert>
#include <memory>
#include <algorithm>
#include <iostream>

#include <edges.hpp>

namespace mazes {

template<Edges E>
using AdjacencyList = std::vector<E>;

constexpr uint64_t unlimited = UINT64_MAX;

/// Directed graph using adjacency list -> not acyclic!
/// \tparam D Datatype to store in each node
/// \tparam MaxEdgesPerNode
template<typename D, uint64_t MaxEdgesPerNode = unlimited>
class DirectedGraph {
public:
    using EdgesType = GetEdgesType<MaxEdgesPerNode>;
    using AdjList = AdjacencyList<EdgesType>;

    using Path = std::vector<uint64_t>;

    constexpr DirectedGraph()
            : data_{}, adj_list_{} {};

    constexpr DirectedGraph(
            std::initializer_list<D> data,
            std::initializer_list<uint64_t> adj_list = {})
            : data_(data), adj_list_(adj_list) {};

    /// @return data of node at given index
    constexpr D &node(const uint64_t node_index) noexcept {
        return data_.at(node_index);
    }

    /// @return data of node at given index
    constexpr const D &node(const uint64_t node_index) const noexcept {
        return data_.at(node_index);
    }

    /// @param node_index index of node
    /// @return list of outgoing edges from node at node_index
    constexpr EdgesType &edges(const uint64_t node_index) noexcept {
        return adj_list_.at(node_index);
    }

    /// @param node_index index of node
    /// @return list of outgoing edges from node at node_index
    constexpr const EdgesType &edges(const uint64_t node_index) const noexcept {
        return adj_list_.at(node_index);
    }

    /// @param node data at node
    /// @return index of added node
    constexpr uint64_t add_node(const D &node) {
        data_.push_back(node);
        adj_list_.resize(adj_list_.size() + 1);
        return data_.size() - 1;
    }

    /// @param node data at node
    /// @return index of added node
    constexpr uint64_t add_node(D &&node) {
        data_.push_back(node);
        adj_list_.resize(adj_list_.size() + 1);
        return data_.size() - 1;
    }

    /// @brief create a directed edge
    constexpr void add_edge(const uint64_t from, const uint64_t to) noexcept {
        edges(from).add_adjacency(to);
    }

    /// @brief remove a directed edge
    constexpr void remove_edge(const uint64_t from, const uint64_t to) noexcept {
        edges(from).remove_adjacency(to);
    }

    /// @brief create two directed edges between node1 and node2
    constexpr void connect(const uint64_t node1, const uint64_t node2) noexcept {
        add_edge(node1, node2);
        add_edge(node2, node1);
    }

    /// @brief remove two directed edges between node1 and node2
    constexpr void disconnect(const uint64_t node1, const uint64_t node2) noexcept {
        remove_edge(node1, node2);
        remove_edge(node2, node1);
    }

    /// @return list of nodes in graph
    constexpr std::vector<D> &
    nodes() noexcept { return data_; };

    /// @return list of nodes in graph
    constexpr const std::vector<D> &
    nodes() const noexcept { return data_; };

    /// @return list of edges for each node. indices correspond with indices in the .nodes() array
    constexpr AdjList &
    adjacency_list() noexcept { return adj_list_; };

    /// @return list of edges for each node. indices correspond with indices in the .nodes() array
    constexpr const AdjList &
    adjacency_list() const noexcept { return adj_list_; };

    /// @brief Allocate space for given amount of nodes
    /// @param capacity amount of nodes to allocate
    constexpr void reserve(const uint64_t capacity) noexcept {
        data_.reserve(capacity);
        adj_list_.reserve(capacity);
    }

    /// @return current number of nodes in graph
    constexpr uint64_t size() const noexcept {
        return adj_list_.size();
    }

private:
    std::vector<D> data_;
    AdjList adj_list_;
};
}; // namespace mazes