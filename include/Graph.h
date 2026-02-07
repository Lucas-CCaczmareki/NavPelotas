// Include guard
#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>
#include <unordered_map>

// Representa um grafo ponderado e direcionado utilizando lista de adjacência
class Graph {

public:
    // Estrutura das arestas.  
    struct Edge {
        int to;         // índice do nodo de destino
        double weight;  // peso da aresta
        /* Direção:
        A direção é representada com a lista de adjacência. Por exemplo:
        adj[0] -> 1, 2, 3
        adj[1] -> 0
        node 0 e node 1 é uma via de mão dupla. */
    };

    // Coordenadas geográficas associadas a um nodo
    struct Coordinate {
        double lat;
        double lon;
    };

    // Construtor auxiliar: cria um grafo vazio com N nodos
    Graph(int numVertices);

    // Constrói a paritir de arquivos JSON padronizado OSM
    Graph(const std::string& nodes, const std::string& edges);

    // Adiciona aresta direcionada u -> v com peso.
    void addEdge(int u, int v, double weight);

    // Retorna a lista de arestas saindo do nodo u
    const std::vector<Edge>& neighbours(int u) const;

    // Reorna a quantidade de nodos do grafo
    int size() const;

    // Retorna a quantidade total de arestas
    int totalEdges() const;

    // Converte um ID externo (JSON) para índice interno
    int getIndexFromId(long long id) const;

    // Converte um índice interno para ID externo
    long long getIdFromIndex(int idx) const;

    // Retorna a coordenada associada ao nodo u
    Coordinate getCoord(int u) const;

private:
    // Lista de adjacência:
    // É construída em ordem de leitura do arquivo nodes.
    // adj[u] contém todas as arestas saindo do nodo u.
    std::vector<std::vector<Edge>> adj;

    // Coordenadas associadas a cada nodo (mesmo índice de adj)
    std::vector<Coordinate> nodeCoords;

    // ID: identificador do nodo nos jsons. Ex: 240003004
    // INDEX: Índice do nodo na lista de adjacência.

    // Mapeamento hashtable ID externo -> índice interno
    std::unordered_map<long long, int> idToIndex;   

    // Mapeamento inverso: Índice interno -> ID externo
    std::vector<long long> indexToId;
};

#endif
