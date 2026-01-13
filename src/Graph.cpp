#include "Graph.h"

// Construtor padrão
Graph::Graph(int numVertices) {
    // Cria todos os vértices (sem nenhuma conexão ainda)
    adj.resize(numVertices);
}

// Construtor com arquivo
Graph::Graph(const std::string& filename) {

}

// Adiciona uma edge à um node.
void Graph::addEdge(int u, int v, double weight) {
    // 1o jeito (q eu pensei)
    // Graph::Edge e;
    // e.to = v;
    // e.weight = weight;
    // adj[u].push_back(e);
    
    // jeito c++ de fazer: 
    // o emplace back já cria o objeto direto dentro da lista de adjacência e evita criar um objeto temporário.
    adj[u].emplace_back(Graph::Edge{v, weight});
}

// Retorna a lista de edges de um node
const std::vector<Graph::Edge>& Graph::neighbours(int u) const {
    return adj[u];
}

// Retorna quantos nodes nosso grafo tem
int Graph::size() const {
    return adj.size();
}