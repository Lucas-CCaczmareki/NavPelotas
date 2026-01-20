#include <iostream>
#include <fstream> //provavelmente pra acessar files
#include <vector>
#include <string>
#include "Graph.h"
#include "json.hpp"
#include <unordered_map>
#include "Dijkstra.h"

int main() {
    
    // try {
    //     Graph g("data/nodes.json", "data/edges.json"); //aqui ele vai criar o grafo, vamo ver se executa pelo menos
        
    //     // num nodes
    //     std::cout << g.size() << "\n";  // isso ta ok, coerente com o arquivo
    //     std::cout << g.totalEdges() << "\n";
        

    // } catch(const std::exception& e) {
    //     std::cerr << e.what() << '\n';
    // }

    Graph g(7);
    
    g.addEdge(0, 2, 3);
    g.addEdge(2, 0, 3);

    g.addEdge(0, 5, 2);
    g.addEdge(5, 0, 2);

    g.addEdge(2, 5, 2);
    g.addEdge(5, 2, 2);

    g.addEdge(2, 4, 1);
    g.addEdge(4, 2, 1);
    
    g.addEdge(5, 4, 3);
    g.addEdge(4, 5, 3);

    g.addEdge(2, 3, 4);
    g.addEdge(3, 2, 4);

    g.addEdge(3, 1, 1);
    g.addEdge(1, 3, 1);

    g.addEdge(4, 1, 2);
    g.addEdge(1, 4, 2);

    g.addEdge(5, 1, 6);
    g.addEdge(1, 5, 6);

    g.addEdge(5, 6, 5);
    g.addEdge(6, 5, 5);

    g.addEdge(1, 6, 2);
    g.addEdge(6, 1, 2);

    Dijkstra dj(g, 0);

    dj.execute();
    
    // Ta errado
    std::cout << "Distance from A0 to B1: "<< dj.getDistance(1) << "\n";

    dj.getPath(1);



    return 0;
}