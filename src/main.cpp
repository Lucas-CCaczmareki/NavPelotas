#include <iostream>
#include <fstream> //provavelmente pra acessar files
#include <vector>
#include <string>
#include "Graph.h"
#include "json.hpp"
#include <unordered_map>
#include "Dijkstra.h"

int main() {
    
    try {
        Graph g("data/nodes.json", "data/edges.json"); //aqui ele vai criar o grafo, vamo ver se executa pelo menos
        
        long long origem = 3403874962;
        long long destino = 3684162309;

        int o = g.getIndexFromId(origem);
        int d = g.getIndexFromId(destino);

        Dijkstra dj(g, o);
        dj.execute();

        std::cout << dj.getDistance(d) << "\n";
        dj.getPath(d);


    } catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }

    return 0;
}