#ifndef GRAPH_H
#define GRAPH_H

#include <vector>   // vai ser usado pra montar a lista de adjacência
#include <string>
#include <unordered_map>

class Graph {

public:
    // Estrutura das arestas.  
    struct Edge {
        int to;         // Endereço (id) do nodo pra onde ela aponta
        double weight;  // Peso da aresta
        // A direção é representada com a lista de adjacência.
    };

    // Construtor padrão pra testes
    Graph(int numVertices);

    // Construtor com o arquivo
    Graph(const std::string& nodes, const std::string& edges);

    // Adiciona aresta direcionada
    void addEdge(int u, int v, double weight);

    // Retorna a lista (vetor) de vizinhos da aresta selecionada
    const std::vector<Edge>& neighbours(int u) const;

    // Quantidade de nodos
    int size() const;

    // Quantidade de arestas
    int totalEdges() const;

    // Getter pra transformar id num index
    int getIndexFromId(long long id) const;

    // Getter pra transformar index num id
    long long getIdFromIndex(int idx) const;

private:
    // A lista de adjacência em si.
    // adj.push_back -> adiciona um nodo
    // adj[0].push_back -> adiciona um caminho nesse nodo
    std::vector<std::vector<Edge>> adj;

    // A hashtable pra converter o id(long long) em um index(int)
    // isso vai aumentar muito a eficiência de memória, pq não vai precisar ter bilhões de vetores vazios
    // só pra usar o id com index direto.
    std::unordered_map<long long, int> idToIndex;   // index nesse caso é o índice do nodo na lista de adjacência

    // Outra hashtable pra conseguir fazer o caminho contrário
    std::vector<long long> indexToId;
};

#endif