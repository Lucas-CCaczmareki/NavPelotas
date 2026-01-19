#include "Dijkstra.h"
#include <algorithm>

// Construtor que recebe um grafo por referência
// Referência é tipo um "apelido" pra um objeto que já existe
// ou seja, eu to chamando a lista adj lá do grafo de adj aqui. Esses nomes se referem ao mesmo objeto em memória

Dijkstra::Dijkstra(const Graph& g, int origin) :    // esses ':' representam o inicio da initializer list 
    // OBS: A initializer list é obrigatória pra atributos que são referência
    graph(g),   // o graph nosso atributo de referência pro grafo, recebe o grafo como parâmetro e guarda   
    
    // Metadados
    numNodes(graph.size()),
    origin(origin),
    hasRun(false),

    // Inicializa os vetores (chama os construtores(size, value))
    dist(numNodes, INT_MAX),    // cria numNodes campos e seta com INT_MAX
    prev(numNodes, {-1, -1})    // cria numNodes e seta com valores que representam ids invalidos

{
    dist[origin] = 0;   // atualiza esse valor pra lista de distâncias já ficar válida
}

// Função pra executar o algoritmo
void Dijkstra::execute() {
    // Recria a priority queue pra zerar ela entre execuções
    pq = std::priority_queue<
        pqNode,
        std::vector<pqNode>,
        std::greater<pqNode>
    >();

    // Continua a escrever a lógica do dijkstra aqui depois
}

// Getter para o valor do menor caminho entre A e B
double Dijkstra::getDistance(int destination) {
    return dist[destination];
}

// Getter para reconstruir o menor caminho entre A e B
std::vector<Dijkstra::Prev> Dijkstra::getPath(int destination) {
    // Lógica pra remontar o caminho:
    // Faz um loop começando do prev[destino] e vai adicionando os nodos até o valor de prev ser o origem
    // Ai pega o vetor e inverte

    // Monta o vetor de retorno
    std::vector<Dijkstra::Prev> path;

    int cur = destination;
    // Se o id do nodo por onde veio ainda é diferente 
    while(cur != origin) {
        path.push_back(prev[cur]);     // coloca o nodo no caminho
        cur = prev[cur].node;     // vai um nodo pra trás no caminho
    }

    // Inverte o vetor e retorna
    // O reverse inverte num intervalo [inicio, fim)
    std::reverse(path.begin(), path.end());

    return path;
}

// Seta a origin pra outro ponto e atualiza a flag se já rodou ou não
void Dijkstra::setOrigin(int origin) {
    this->origin = origin;
    this->hasRun = false;
}