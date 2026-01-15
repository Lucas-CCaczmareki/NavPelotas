#include <iostream>
#include <fstream> //provavelmente pra acessar files
#include <vector>
#include <string>
#include "Graph.h"
#include "json.hpp"
#include <unordered_map>


int main() {
    
    try {
        Graph g("data/nodes.json", "data/edges.json"); //aqui ele vai criar o grafo, vamo ver se executa pelo menos

    } catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }

}