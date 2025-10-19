//
// Created by aowyn on 10/16/25.
//
#include "Adjacency_List.h"

/* INCLUDE LIBRARIES */
#include <stack>
#include <stdexcept>
#include <vector>


template<typename T>
Adjacency_List<T>::Adjacency_List() {
    vertices = std::vector<T>();
    edges = std::vector<std::vector<int>>();
}

template<typename T>
bool Adjacency_List<T>::validate_index(int index) const {
    if (index >= size || index < 0) {
        return false;
    }
    return true;
}

template<typename T>
void Adjacency_List<T>::add_vertex(T &value) {
    vertices.emplace_back(value);
    edges.emplace_back();
    visited.emplace_back(false);
    size++;
}

template<typename T>
void Adjacency_List<T>::add_edge(int a, int b) {

    if (validate_index(a) && validate_index(b)) {
        edges[a].emplace_back(b);
        edges[b].emplace_back(a);
    }
    else {
        throw std::invalid_argument("ERROR in Adjacency_List::add_edge: one or more indexes out of range");
    }
}

template<typename T>
T Adjacency_List<T>::get_vertex(int i) {
    if (validate_index(i)) {
        return vertices[i];
    }
    else {
        throw std::invalid_argument("ERROR in Adjacency_List::get_vertex: index out of range");
    }
}

template<typename T>
std::vector<int> Adjacency_List<T>::get_edges(int i) {
    if (validate_index(i)) {
        return edges[i];
    }
    else {
        throw std::invalid_argument("ERROR in Adjacency_List::get_vertex: index out of range");
    }
}

template<typename T>
int Adjacency_List<T>::get_size() const {
    return size;
}

template<typename T>
T Adjacency_List<T>::remove_vertex(int i) {
    if (!validate_index(i)) {
        throw std::invalid_argument("ERROR in Adjacency_List::remove_vertex: index out of range");
    }
    T value = vertices.at(i);

    for ( const int connection : edges[i]) {
        std::erase(edges[connection], i);
    }
    edges.erase(edges.begin() + i);
    vertices.erase(vertices.begin() + i);
    size--;

    return value;
}

template<typename T>
void Adjacency_List<T>::remove_edge(const int a, const int b) {
    std::erase(edges[a], b);
    std::erase(edges[b], a);
}


template<typename T>
std::vector<int> Adjacency_List<T>::get_unvisited_neighbors(const int curr) const {
    const int width = floor(sqrt(size));

    std::vector<int> unvisitedNeighbors;
    if ((curr - width) >= 0 && !visited[curr - width]){
        unvisitedNeighbors.emplace_back(curr - width);
    }
    if ((curr + width) < size && !visited[curr + width]){
        unvisitedNeighbors.emplace_back(curr + width);
    }
    if ((curr % width) > 0 && !visited[curr - 1]){
        unvisitedNeighbors.emplace_back(curr - 1);
    }
    if ((curr % width) < (width - 1) && curr != size - 1 && !visited[curr + 1]) {
        unvisitedNeighbors.emplace_back(curr + 1);
    }

    return unvisitedNeighbors;
}

template<typename T>
void Adjacency_List<T>::randomized_depth_first_search(std::mt19937 &random_number_generator) {
    std::stack<int> unvisited;

    unvisited.push(random_number_generator() % vertices.size());
    visited[unvisited.top()] = true;

    while (!unvisited.empty()) {
        const int current = unvisited.top();
        unvisited.pop();

        if (!visited.at(current)) {
            if (auto unvisitedNeighbors = get_unvisited_neighbors(current); !unvisitedNeighbors.empty()) {
                unvisited.push(current);
                int next = unvisitedNeighbors.at(random_number_generator() % unvisitedNeighbors.size());

                edges[current].push_back(next);
                edges[next].push_back(current);

                visited[next] = true;
                unvisited.push(next);
            }
        }
    }
}
