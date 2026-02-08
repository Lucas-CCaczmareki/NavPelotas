#include "Graph.h"
#include "json.hpp"
#include <iostream>
#include <fstream>

// Construtor padrão
Graph::Graph(int numVertices) {
    adj.resize(numVertices);                    // Cria todos os vértices (sem nenhuma conexão ainda)
    nodeCoords.resize(numVertices, {0.0,0.0});  // Redimensiona o vetor de coordenadas com 0.0 pra evitar crash
}

// Construtor com arquivo
Graph::Graph(const std::string& nodes, const std::string& edges) {
    nlohmann::json j_file;
    std::ifstream nodes_file(nodes);
    std::ifstream edges_file(edges);

    // Se falhar (ex: rodando de dentro da pasta debug), tenta voltar um nível
    if (!nodes_file.is_open()) {
        nodes_file.clear();             // Limpa o estado de erro
        nodes_file.open("../" + nodes); // Tenta ../data/nodes.json
    }

    if (!edges_file.is_open()) {
        edges_file.clear();
        edges_file.open("../" + edges); // Tenta ../data/edges.json
    }

    // Checa se os arquivos abriram
    if (!(nodes_file.is_open() && edges_file.is_open())) {
        throw std::runtime_error("Erro ao abrir algum dos arquivos!\n");
    }

    // =================== PARTE 1: Montar o vetor de nodes (adj.node<Edges>) ===================
    nodes_file >> j_file;
    
    // Seta um tamanho inicial pra hastable com base no tamanho do nodes.json
    // Motivo: Evita várias realocações durante o loop
    idToIndex.reserve(j_file.size());
    nodeCoords.reserve(j_file.size());      // reserva memoria pras coordenadas

    // Mapeia todos os IDs externos para um índice 
    // Constrói o "espaço" pra esse nodo na lista de adjacência, sem edges ainda
    for(auto& node : j_file) {
        // Mapeamento ID externo -> índice
        // Motivo: Aumenta eficiência de memória; Evita criar vários vetores vazios para conseguir usar long long no acesso direto

        long long id = node["id"];          // usa o id do JSON pra facilitar a escrita

        // le lat e lon do json
        double lat = node.value("y", 0.0);  // y é a latitude
        double lon = node.value("x", 0.0);  // x é a longitude


        
        int i = adj.size();                 // próximo índice i livre = tamanho atual da lista de adjacência (começa em 0)
        idToIndex[id] = i;                  // ID -> I; Ordem igual a do JSON
        indexToId.push_back(id);            // I -> ID (caminho contrário)

        // Cria um nodo diretamente (emplace) dentro da lista de adjacência usando o construtor padrão do vector<Edges>.
        // nao precisa cirar e copiar um objeto temporario como o push_back faz
        // Vetor de EDGES vazio ainda.
        adj.emplace_back();
        nodeCoords.push_back({lat,lon});    // guarda a coordenada no vetor (mesmo indice i)
    }

    // =================== PARTE 2: Preenchendo o vetor das arestas (adj[i].edge) ===================
    edges_file >> j_file;

    
    for(auto& edge : j_file) {
        bool oneway = edge["data"].value("oneway", false);  // Caso o campo esteja vazio, atribui false como padrão
        double length = edge["data"].value("length", 1e18); // Caso o campo esteja vazio, atribui um numero gigante

        // Se a via é twoway/mão dupla (vai e volta)
        if(!oneway) { // = if (twoway == true)

            // Campo 'reversed' não importa aqui, pois adicionamos nos 2 sentidos que qualquer forma.
            addEdge(idToIndex[edge["u"]], idToIndex[edge["v"]], length); // add no sentido u -> v
            addEdge(idToIndex[edge["v"]], idToIndex[edge["u"]], length); // add no sentido v -> u
        
        // Caso a via seja oneway/mão única (só vai, ou só volta)
        } else {

            // Campo 'reversed' só é lido quando a via É oneway.
            // Indica que a via está mapeada no sentido oposto ao expresso no JSON.
            // Não é necessário tratar o caso/excpection de array, pois ele só ocorre em vias twoway.
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
}

// Adiciona uma edge dirigida u -> v com peso associado
void Graph::addEdge(int u, int v, double weight) {
    adj[u].emplace_back(Graph::Edge{v, weight});
}

// Retorna a lista de arestas saindo do nodo u
// Retorna referência const para evitar cópia e modificação externa
const std::vector<Graph::Edge>& Graph::neighbours(int u) const {
    return adj[u];
}

// Retorna número total de nodes do grafo
int Graph::size() const {
    return adj.size();
}

// Retorna o número total de arestas do grafo
int Graph::totalEdges() const {
    int edges = 0;

    // Percorre todos nodes e vai somando o numero de arestas (tamanho do vetor)
    for(auto& node : adj) {
        edges += node.size();
    }

    return edges;
}

// Converte um ID externo do JSON em um indíce interno
int Graph::getIndexFromId(long long id) const {
    return idToIndex.at(id);
}

// Converte um índice interno do grafo para o id original do JSON
long long Graph::getIdFromIndex(int idx) const {
    return indexToId.at(idx);
}

// Retorna a coordenada (lat, lon) do nodo u
// Retorna (0,0) se o índice for inválido
Graph::Coordinate Graph::getCoord(int u) const{
    // precaução pra nao acessar memoria invalida
    if (u >= 0 && u < nodeCoords.size()) {
        return nodeCoords[u];
    }
    return {0.0,0.0};
}
