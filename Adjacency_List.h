//
// Created by aowyn on 10/16/25.
//

#ifndef RDG_UNLIMITED_ADJACENCY_LIST_H
#define RDG_UNLIMITED_ADJACENCY_LIST_H
#include <stdexcept>
#include <vector>

/**
 * A bidirectional Adjacency list implementation of a graph which supports randomized depth first searching.
 * ATTRIBUTES:
 * @var vertices, a vector of all values stored in the graph, this is changed very little
 * @var adjacency, A list containing all vertex connections. each vertex's connections are stored at the same index
 *      as the value of that vertex in vertices
 * @var visited, a list of booleans determining weather or not a vertex has been visited during searching algorithms
 *      currently only used in randomized_first_search
 * @var size, The number of vertices in the graph.
 *
 * @tparam T
 */
template<typename T>
class Adjacency_List {
private:
    std::vector<T> vertices;
    std::vector<std::vector<int>> edges;

    std::vector<bool> visited;
    int size = 0;


    [[nodiscard]] bool validate_index(const int index) const {
        if (index >= size || index < 0) {
            return false;
        }
        return true;
    }
public:
    /* CONSTRUCTOR */
    /**
     * The constructor for Adjacency_list that initializes this object as an empty graph
     */
    explicit Adjacency_List() {
        vertices = std::vector<T>();
        edges = std::vector<std::vector<int>>();
    }

    /* ADDITIVE MANIPULATORS */
    /**
     * adds a vertex with no connected edges to the graph of value T
     * @param value the value of the vertex to be added
     */
    void add_vertex(T &value) {
        vertices.emplace_back(value);
        edges.emplace_back();
        visited.emplace_back(false);
        size++;
    }
    /**
     * adds an edge between the vertices at index a and index b
     * @param a
     * @param b
     */
    void add_edge(int a, int b) {

        if (validate_index(a) && validate_index(b)) {
            edges[a].emplace_back(b);
            edges[b].emplace_back(a);
        }
        else {
            throw std::invalid_argument("ERROR in Adjacency_List::add_edge: one or more indexes out of range");
        }
    }

    /* GETTERS */
    /**
     * return the vertex value at index i
     * @param i the index of a vertex
     * @return the value of vertex i
     */
    T& get_vertex(int i) {
        if (validate_index(i)) {
            return vertices[i];
        }
        else {
            throw std::invalid_argument("ERROR in Adjacency_List::get_vertex: index out of range");
        }
    }

    int get_index_of(T &value) {
        for (int i = 0; i < vertices.size(); i++) {
            if (vertices.at(i) == value) {
                return i;
            }
        }
        return -1;
    }

    /**
     * return the indexes of all edges of the vertex at index i
     * @param i the index of a vertex
     * @return the indexes of edge connections at vertex i
     */
    std::vector<int> get_edges(const int i) {
        if (validate_index(i)) {
            return edges[i];
        }
        else {
            throw std::invalid_argument("ERROR in Adjacency_List::get_vertex: index out of range");
        }
    }

    [[nodiscard]] int get_size() const {
        return size;
    }

    /* SUBTRACTIVE MANIPULATORS */
    /**
     * removes the vertex at index i and all of its edged, then returns its value
     * @param i the index of the vertex to be removed
     * @return the value of the removed vertex
     */
    T remove_vertex(int i) {
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
    /**
     * removes the edge between the vertex at index a and index b if it exists
     * @param a
     * @param b
     */
    void remove_edge(const int a, const int b) {
        std::erase(edges[a], b);
        std::erase(edges[b], a);
    }
};


#endif //RDG_UNLIMITED_ADJACENCY_LIST_H
