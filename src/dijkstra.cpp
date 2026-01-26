#include "Dijkstra.h"
#include <algorithm>
#include <iostream>

// Construtor que recebe um grafo por referência
// Referência é tipo um "apelido" pra um objeto que já existe
// ou seja, eu to chamando a lista adj lá do grafo de adj aqui. Esses nomes se referem ao mesmo objeto em memória

Dijkstra::Dijkstra(const Graph& g, int origin) :    // esses ':' representam o inicio da initializer list 
    // OBS: A initializer list é obrigatória pra atributos que são referência
    // OBS: A ordem da initializer list deve bater com a ordem de declaração no .h
    graph(g),   // o graph nosso atributo de referência pro grafo, recebe o grafo como parâmetro e guarda   
    
    // Metadados
    numNodes(graph.size()),
    origin(origin),
    hasRun(false),

    // Inicializa os vetores (chama os construtores(size, value))
    dist(numNodes, 1e18),     // cria numNodes campos e seta com 1e18
    prev(numNodes, {-1, -1}),    // cria numNodes campos e seta com valores que representam ids invalidos
    explored(numNodes, false)    // cria numNodes campos e seta todos com false
{
    dist[origin] = 0;            // atualiza esse valor pra lista de distâncias já ficar válida
    // explored[origin] = true;
}

// Função pra executar o algoritmo
/*
// 2-step process:
    // ---------------------------------------------------------------------------------------------------------
    // 1 - Update estimates
    // Vai conferir todas as arestas saindo do nodo atual e se o caminho até o próximo nodo
    // (considerando a soma do caminho pra chegar até o nodo atual, o custo acumulatório no caso)
    // for menor que o caminho registrado no dist[cur], atualiza ele com esse valor.
    // ---------------------------------------------------------------------------------------------------------

    // ---------------------------------------------------------------------------------------------------------
    // 2 - choose next vertex
    // Aqui a gente vai pra próxima cidade INEXPLORADA com o MENOR caminho pra lá 
    // (é aqui que a priority queue com heap entra pra eficiência, a gente evita de conferir todo o vetor de vizinhos)
    
    // ai quando a gente escolhe esse vértice e nos movemos pra cá, atualiza o custo acumulatório de caminho com 
    // o valor da aresta que usamos pra chegar aqui, e ai loopa o passo 1, de atualizar estimativas
    // ---------------------------------------------------------------------------------------------------------

    // E no meio desses processos todos, lembra de atualizar os valores das outras estruturas como o prev[]

*/
void Dijkstra::execute() {
    std::cout << "executing dijkstra...\n";
    
    // Recria a priority queue pra zerar ela entre execuções
    pq = std::priority_queue<pqNode>();
    pq.push({origin, 0});
    dist[origin] = 0;

    while(!pq.empty()) {

        // Choose next vertex (2nd step)
        pqNode cur_node = pq.top();
        pq.pop();

        // Ignora um caminho que já é certamente pior
        if (cur_node.path_weight > dist[cur_node.id]) continue;

        // Ignora entradas antigas e inválidas da pq
        if (explored[cur_node.id]) continue;

        // Marca o nodo atual que entramos como explorado
        explored[cur_node.id] = true;

        int i = 0;
        // Uptade estimates (1st step)
        for(auto& edge : graph.neighbours(cur_node.id)) {
            // se o peso desse caminho é menor que a menor distância conhecida, atualiza
            // OBS: Se houver mais de um caminho ótimo e eu quiser garantir que ele escolha sempre o ultimo caminho
            // é só trocar a comparação pra <=. Qual ele escolhe vai depender disso e outros fatores, mas é arbitrário, sempre vai vir o mesmo
            if((dist[cur_node.id] + edge.weight) < dist[edge.to]) {
                
                // DEBUG
                // std::cout
                //     << "edge "
                //     << graph.getIdFromIndex(cur_node.id)
                //     << " -> "
                //     << graph.getIdFromIndex(edge.to)
                //     << " | weight = "
                //     << edge.weight
                //     << "\n";

                
                // Dist mantém a soma do menor caminho da origem ate o idx dela atualizado
                dist[edge.to] = dist[cur_node.id] + edge.weight;
                
                // atualiza o prev
                prev[edge.to].node = cur_node.id;
                prev[edge.to].edgeIdx = i;

                // Coloca os vizinhos que tem um caminho melhor na priority queue
                pq.push({edge.to, (dist[cur_node.id] + edge.weight)});
            }
            i++; //atualiza o indice da edge
        }
    }
    std::cout << "dijkstra executed.\n";
}

// Getter para o valor do menor caminho entre A e B
double Dijkstra::getDistance(int destination) {
    return dist[destination];
}

// Getter para reconstruir o menor caminho entre A e B
/*
    // Lógica pra remontar o caminho:
    // Faz um loop começando do prev[destino] e vai adicionando os nodos até o valor de prev ser o origem
    // Ai pega o vetor e inverte
*/
std::vector<Dijkstra::Prev> Dijkstra::getPath(int destination) {
    // Proteção: destino inalcançável
    if (dist[destination] == 1e18) return {};

    // prev tem id node anterior, id da aresta que usou pra chegar no node atual

    // Monta o vetor de retorno
    std::vector<Dijkstra::Prev> path;

    int cur = destination;

    // Se o id do nodo por onde veio ainda é diferente 
    while(cur != origin) {
        path.push_back(prev[cur]);     // coloca o nodo no caminho
        cur = prev[cur].node;          // vai um nodo pra trás no caminho
    }

    // Inverte o vetor e retorna
    // O reverse inverte num intervalo [inicio, fim)
    std::reverse(path.begin(), path.end());

    // vou fazer ele printar o path só pra testes aqui por enquanto q vai ser mais fácil
    for(auto& path_node : path) {
        std::cout << graph.getIdFromIndex(path_node.node) << ", ";
    }
    std::cout << graph.getIdFromIndex(destination) << "\n";

    return path;
}

// Seta a origin pra outro ponto e atualiza a flag se já rodou ou não
void Dijkstra::setOrigin(int origin) {
    this->origin = origin;
    this->hasRun = false;
}