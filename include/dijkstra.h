#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <vector>
#include <queue>
#include <utility>
#include <functional>
#include "Graph.h"

class Dijkstra {
    // Só pra diminuir o tamanho do bag q eu tenho que escrever
    // using AdjList = std::vector<std::vector<Graph::Edge>>;
    
    // basicamente uma struct com 2 elementos e tipos adaptáveis
    using pqNode = std::pair<double, int>; // weight, id

public:
    struct Prev {
        int node;
        int edgeIdx; //index na lista de adjacência da aresta pela qual veio .
    };

    // Construtor que recebe um grafo
    Dijkstra(const Graph& g, int origin);

    // Função pra executar o algoritmo
    void execute();

    // Getter para o valor do menor caminho entre A e B
    double getDistance(int destination);

    // Getter para reconstruir o menor caminho entre A e B
    std::vector<Prev> getPath(int destination);

    // Setter pra mudar a origin]
    void setOrigin(int origin);

private:
    // Priority queue usando heap para escolher o próximo nodo sem percorrer todos para escolher o melhor
    // Definindo o comporatamento da nossa priority queue (a padrão é max-heap, e o dijkstra precisa da minheap)
    std::priority_queue<
        pqNode,                 // tipo do elemento da pq
        std::vector<pqNode>,    // container, como a heap é implementada internamente. Vector é o padrão
        
        // Esse campo aqui é chamado de compare(a, b), ele retorna true se A tem MENOS prioridade que B
        // Colocando greater ele compara se a > b pra decidir a prioridade. Ou seja:
        // Se A > B, ele retorna true, que significa que A tem MENOS prioridade que B e B (o menor) vai pro topo.
        std::greater<pqNode>    // regra de ordenação
    > pq;

    // Vetor para guardar as menores distâncias conhecidas 
    std::vector<double> dist;

    // Vetor para reconstruir o menor caminho
    std::vector<Prev> prev;

    // Metadados
    int numNodes;
    bool hasRun;    // indica se o algoritmo já rodou para a última origin inserida
    int origin;
    // int destiny; // roda com a origem, dps entra com o destino pra retorno. Não é necessário em teoria

    // Guardando o grafo como referência (pra n passar ela toda hora)
    const Graph& graph;
};

#endif