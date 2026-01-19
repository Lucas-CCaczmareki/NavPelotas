#include "Graph.h"
#include "json.hpp"
#include <iostream>
#include <fstream>

// Construtor padrão
Graph::Graph(int numVertices) {
    // Cria todos os vértices (sem nenhuma conexão ainda)
    adj.resize(numVertices);
}

// Construtor com arquivo
Graph::Graph(const std::string& nodes, const std::string& edges) {
    nlohmann::json j_file;
    std::ifstream nodes_file(nodes);
    std::ifstream edges_file(edges);

    // Checa se os arquivos abriram
    if (!(nodes_file.is_open() && edges_file.is_open())) {
        throw std::runtime_error("Erro ao abrir algum dos arquivos!\n");
    }

    
    // Primeiro vamo montar o vetorzao de nodes(que é um vetor pra edges)
    nodes_file >> j_file;
    
    // indica pra hashtable q eu vou botar +- esse tanto de elementos
    // isso evita várias realocações durante um loop, por exemplo. É tipo aquele tamanho inicial quando eu implementei
    idToIndex.reserve(j_file.size()); 

    // =============================================
    //  Construindo os nodos na lista de adjacência
    // =============================================
    // Agora eu mapeio todos ids de nodo pra um index na hashtable e já construo eles na lista de adjacência
    for(auto& node : j_file) {
        // Mapeia id -> index
        long long id = node["id"];  // cria um id temporario pra facilitar a escrita
        
        // index = último espaço vazio da lista de adj. Escrever assim só pra ficar mais claro.
        // e desse jeito eu não preciso controlar o i++ por fora também.
        int i = adj.size();         
        idToIndex[id] = i;          // isso aqui faz o id sempre mapear pro index i, na mesma ordem que eles tão escrito no json

        // Cria um nodo
        // O emplace back vai criar o objeto (std::vector<Edge>) dentro do vetor de vetores adj.
        // Como eu não dei nenhum parâmetro, o compilador chama o construtor de vetor e struct padrão.
            // Ou seja, o espaço alocado lá existe mas não tem nada ainda.
        adj.emplace_back();
    }

    // =============================================
    //  Construindo as arestas para cada nodo
    // =============================================
    edges_file >> j_file;

    // Testando tamanho
    // int n_edgesFile = 0;
    // int n_edgesGraph = 0; 

    // Primeiro precisamos descobrir o sentido da via
    for(auto& edge : j_file) {

        // O value trata caso o campo esteja vazio. Ele atriu false como padrão
        bool oneway = edge["data"].value("oneway", false);
        double length = edge["data"].value("length", INT_MAX); //atribui um numero gigante caso o valor n seja dado, pra n usar a aresta mesmo

        // Contando tamanho pra testar
        // n_edgesFile++;
        // if (oneway) {
        //     n_edgesGraph += 1;
        // } else {
        //     n_edgesGraph += 2;
        // }

        // Ou seja, se a via é de mão dupla. Vai e volta
        if(!oneway) {
            // add no sentido u -> v
            addEdge(idToIndex[edge["u"]], idToIndex[edge["v"]], length);

            // add no sentido v -> u
            addEdge(idToIndex[edge["v"]], idToIndex[edge["u"]], length);
        
        // Caso seja 1 sentido só
        } else {
            // Aqui nao vai rolar a exception do reversed ser um array simplesmente pq se é um array 
            // a rua NAO é oneway e fodase o reversed nesse caso.
            bool reversed = edge["data"].value("reversed", false);
            // Se é reversed, o sentido é v -> u
            if(reversed) {
                addEdge(idToIndex[edge["v"]], idToIndex[edge["u"]], length);
            // Se não é reversed, o sentido é u -> v
            } else {
                addEdge(idToIndex[edge["u"]], idToIndex[edge["v"]], length);
            }
        }
    }
    // std::cout << "Grafo finalizado. Edges no arquivo: " << n_edgesFile << "\nEdges no grafo: " << n_edgesGraph << "\n\n";
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

// Retorna quantas arestas o grafo tem
int Graph::totalEdges() const {
    int edges = 0;

    // Em tese isso aqui vai percorrer todos nodes e ir somando o numero de arestas
    for(auto& node : adj) {
        edges += node.size();
    }

    return edges;
}