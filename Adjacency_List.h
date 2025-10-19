//
// Created by aowyn on 10/16/25.
//

#ifndef RDG_UNLIMITED_ADJACENCY_LIST_H
#define RDG_UNLIMITED_ADJACENCY_LIST_H
#include <random>
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

    /**
     * get_unvisited_neighbors is a helper function of randomized_depth_first_search which treats the graph as a grid
     * and returns a vector of all neighbors of the vertex located at index curr which have not yet been visited.
     *
     * @param curr the index whose neighbors are to be checked
     * @return a vector of neighbors of curr that have not yet been visited by randomized_depth_first_search
     */
    [[nodiscard]] std::vector<int> get_unvisited_neighbors(int curr) const;

    [[nodiscard]] bool validate_index(int index) const;
public:
    /* CONSTRUCTOR */
    /**
     * The constructor for Adjacency_list that initializes this object as an empty graph
     */
    explicit Adjacency_List();

    /* ADDITIVE MANIPULATORS */
    /**
     * adds a vertex with no connected edges to the graph of value T
     * @param value the value of the vertex to be added
     */
    void add_vertex(T &value);
    /**
     * adds an edge between the vertices at index a and index b
     * @param a
     * @param b
     */
    void add_edge(int a, int b);

    /* GETTERS */
    /**
     * return the vertex value at index i
     * @param i the index of a vertex
     * @return the value of vertex i
     */
    T get_vertex(int i);
    /**
     * return the indexes of all edges of the vertex at index i
     * @param i the index of a vertex
     * @return the indexes of edge connections at vertex i
     */
    std::vector<int> get_edges(int i);

    [[nodiscard]] int get_size() const;

    /* SUBTRACTIVE MANIPULATORS */
    /**
     * removes the vertex at index i and all of its edged, then returns its value
     * @param i the index of the vertex to be removed
     * @return the value of the removed vertex
     */
    T remove_vertex(int i);
    /**
     * removes the edge between the vertex at index a and index b if it exists
     * @param a
     * @param b
     */
    void remove_edge(int a, int b);

    /* RANDOMIZATION */
    /**
     * Performs a randomized version of the depth first search algorithm on the adjacency list, this gives turns the list
     * into a maze that with size cells.
     * @param random_number_generator a random number generator for performing randomized depth first search
     */
    void randomized_depth_first_search(std::mt19937 &random_number_generator);
};


#endif //RDG_UNLIMITED_ADJACENCY_LIST_H
