// O .h é um header. Ele só diz pro .cpp que a gente ta chamando que essas funções dentro existem 

// INCLUDE GUARD
// Isso aqui confere se o graph_h já existe no momento que chamamos o cabeçalho
// Se não existe, ele faz o define e cria, se já existe, ele ignora.
// Impede que dê erro por múltiplas chamadas, ou que dê erro se outro header contém o graph_h

// Basicamente: O include guard garante que o header seja processado apenas uma vez, 
             // mesmo se incluído múltiplas vezes direta ou indiretamente
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
    // const garante que o construtor não modifique a string. & evita que ele crie uma cópia local
    // & é um parâmetro de referência, serve pra passar exatamente o endereço do item enviado
    Graph(const std::string& nodes, const std::string& edges);

    // Adiciona aresta direcionada
    void addEdge(int u, int v, double weight);

    // Retorna a lista (vetor) de vizinhos da aresta selecionada
    // esse const ao final da assinatura significa que o método não pode alterar nenhum atributo da classe (.this)
    // isso é importante por que o neighbours funciona como um getter, ent n é pra ele modificar nada
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