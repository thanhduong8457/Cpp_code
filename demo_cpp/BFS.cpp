#include <iostream>
#include <list>
#include <queue>
#include <vector>

// Graph class definition using adjacency list
class Graph {
private:
    int vertices;               // Number of vertices
    std::list<int> *adjList;    // Pointer to an array containing adjacency lists

public:
    // Constructor to initialize the graph
    Graph(int v) {
        vertices = v;
        adjList = new std::list<int>[v];
    }

    // Destructor to delete the adjacency list
    ~Graph() {
        delete[] adjList;
    }

    // Add an edge to the graph (undirected by default)
    void addEdge(int v, int w) {
        adjList[v].push_back(w);  // Add w to v's list
        adjList[w].push_back(v);  // Add v to w's list (for undirected graph)
    }

    // BFS traversal from a given source node
    void BFS(int startVertex) {
        // Mark all vertices as not visited
        std::vector<bool> visited(vertices, false);

        // Create a queue for BFS
        std::queue<int> queue;

        // Mark the starting vertex as visited and enqueue it
        visited[startVertex] = true;
        queue.push(startVertex);

        while (!queue.empty()) {
            // Dequeue a vertex from the queue and print it
            int currentVertex = queue.front();
            std::cout << currentVertex << " ";
            queue.pop();

            // Get all adjacent vertices of the dequeued vertex
            // If an adjacent has not been visited, mark it visited and enqueue it
            for (auto it = adjList[currentVertex].begin(); it != adjList[currentVertex].end(); ++it) {
                if (!visited[*it]) {
                    visited[*it] = true;
                    queue.push(*it);
                }
            }
        }
        std::cout << std::endl;
    }

    // Print the graph (for debugging purposes)
    void printGraph() {
        for (int i = 0; i < vertices; i++) {
            std::cout << "Vertex " << i << " is connected to: ";
            for (auto v : adjList[i]) {
                std::cout << v << " ";
            }
            std::cout << std::endl;
        }
    }
};

int main() {
    // Create a graph object with 5 vertices
    Graph graph(5);

    // Add edges to the graph
    graph.addEdge(0, 1);
    graph.addEdge(0, 4);
    graph.addEdge(1, 2);
    graph.addEdge(1, 3);
    graph.addEdge(1, 4);
    graph.addEdge(2, 3);
    graph.addEdge(3, 4);

    // Print the graph
    graph.printGraph();

    std::cout << "BFS starting from vertex 0: ";
    graph.BFS(0); // Perform BFS starting from vertex 0

    return 0;
}