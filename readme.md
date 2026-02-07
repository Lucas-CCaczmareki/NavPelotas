# Projeto de Grafos - Sistema de Rotas

## How to run on MacOS with QtCreator

- 1. Create new project -> Import Existing -> choose folder (name: qttest)
- 2. Select Kit (Qt 6 macOS + clang): `Qt Creator -> Preferences -> Kits -> add -> clang`
- 3. Run qmake:
  - 3.1 open terminal on bottom of your qt creator project
  - 3.2 find path to qmake: `ls ~/Qt/*/macos/bin/qmake` (normally is **/Users/username/Qt/6.10.2/macos/bin/qmake**)
  - 3.3 run: `/path/to/qmake NavPelotas.pro` (this generates the make file based on .pro file)
- 4. compile project: `make -j12` (-j12 is optional to use multiple cpu cores)
- 5. run project: `./NavPelotas.app/Contents/MacOS/NavPelotas`

## Estrutura do Projeto

### Arquivos principais

- **`Graph.h` / `Graph.cpp`**: Implementação da estrutura de grafo
- **`Dijkstra.h` / `Dijkstra.cpp`**: Implementação do algoritmo de Dijkstra
- **`main.cpp`**: Exemplo de uso básico
- **`json.hpp`**: Biblioteca nlohmann/json para parsing de JSON

### Arquivos de dados

- **`nodes.json`**: Nós do grafo (interseções/esquinas)
- **`edges.json`**: Arestas do grafo (ruas/segmentos)
- **`nodes_to_label.json`**: Mapeia IDs de nós para labels (ex: "rua x rua y")
- **`label_to_nodes.json`**: Mapeia labels para IDs de nós

---

## Estrutura do arquivo `edges.json`

Cada entrada do arquivo `edges.json` representa **uma aresta do grafo** (um segmento da malha viária).

### Campos principais

- **`u`**  
  ID do nó de origem (node do OSM).

- **`v`**  
  ID do nó de destino (node do OSM).

- **`key`**  
  Identificador interno do OSM para diferenciar múltiplas arestas entre o mesmo par `(u, v)`.  
  _Não é necessário para o Dijkstra._

- **`data`**  
  Metadados da aresta:
  - **`length`** (`double`)  
    Comprimento da aresta em metros (peso do Dijkstra).
  - **`oneway`** (`bool`)
    - `true` → via de mão única
    - `false` → via de mão dupla
  - **`reversed`** (`bool` ou `array`)  
    Só importa quando `oneway = true`:
    - `false` → sentido `u → v`
    - `true` → sentido `v → u`
    - `array` → múltiplas lanes / geometria composta (ignorar)
  - Outros campos (nome, tipo de via, etc.)  
    _Não usados no algoritmo._

### Regras usadas no código

- Se `oneway == false`  
  → adiciona arestas `u → v` **e** `v → u`
- Se `oneway == true`:
  - `reversed == false` → `u → v`
  - `reversed == true` → `v → u`
- `reversed` em formato de array é ignorado.

---

## Estrutura do arquivo `nodes.json`

Cada entrada representa uma interseção/esquina no grafo.

### Campos principais

- **`id`** (`long long`)  
  Identificador único do nó (ID do OSM).

- **`x`** (`double`)  
  Longitude.

- **`y`** (`double`)  
  Latitude.

- **`street_count`** (`int`)  
  Número de ruas que se encontram nessa interseção.

---

## Implementação do Dijkstra

### Classe `Dijkstra`

O algoritmo de Dijkstra encontra o **menor caminho** entre dois nós no grafo.

#### Atributos principais

```cpp
const Graph& graph;           // Referência ao grafo
int numNodes;                 // Número de nós
int origin;                   // Nó de origem
bool hasRun;                  // Flag se já executou

vector<double> dist;          // Distâncias mínimas da origem
vector<PrevNode> prev;        // Caminho reverso (reconstrução)
vector<bool> explored;        // Nós já explorados
priority_queue<pqNode> pq;    // Fila de prioridade
```

#### Métodos principais

- **`Dijkstra(const Graph& g, int origin)`**  
  Construtor que inicializa o algoritmo com um grafo e nó de origem.

- **`void execute()`**  
  Executa o algoritmo de Dijkstra a partir da origem.

- **`double getDistance(int destination)`**  
  Retorna a distância mínima até o destino.

- **`vector<PrevNode> getPath(int destination)`**  
  Reconstrói e retorna o caminho até o destino.

- **`void setOrigin(int origin)`**  
  Muda a origem e reseta o estado para nova execução.

### Como funciona

1. **Inicialização**:
   - Todas distâncias começam como `1e18` (infinito)
   - Distância da origem = 0
   - Adiciona origem na priority queue

2. **Loop principal**:
   - Remove nó com menor distância da fila
   - Marca como explorado
   - Atualiza distâncias dos vizinhos se encontrar caminho melhor
   - Adiciona vizinhos atualizados na fila

3. **Reconstrução do caminho**:
   - Usa vetor `prev[]` para voltar do destino até a origem
   - Inverte o caminho para obter ordem origem → destino

---

## Uso Básico

### Exemplo 1: Caminho simples

```cpp
#include "Graph.h"
#include "Dijkstra.h"

int main() {
    // Carrega o grafo
    Graph g("data/nodes.json", "data/edges.json");

    // Define origem e destino (IDs do OSM)
    long long origem = 1675174988;  // general osorio x general telles
    long long destino = 1675174986; // lobo da costa x santa cruz

    // Converte IDs para índices internos
    int o = g.getIndexFromId(origem);
    int d = g.getIndexFromId(destino);

    // Cria e executa Dijkstra
    Dijkstra dj(g, o);
    dj.execute();

    // Obtém resultados
    std::cout << "Distância: " << dj.getDistance(d) << "m\n";
    dj.getPath(d);  // Imprime o caminho

    return 0;
}
```

**Saída:**

```
executing dijkstra...
dijkstra executed.
772.245
1675174988, 1675174967, 1675174977, ..., 1675174986
```

---

## Lidando com Múltiplos IDs

**Problema**: Algumas interseções têm múltiplos IDs no dataset (353 casos no total).  
**Motivo**: Cruzamentos complexos, rotatórias ou pistas separadas no OpenStreetMap.

### Estratégia 1: Usar o primeiro ID

### Estratégia 2: Testar todos e pegar o menor

## Verificação do Grafo

### Estatísticas do dataset

- **Total de nós**: 8.185
- **Total de arestas**: 21.271
- **Esquinas com múltiplos IDs**: 353

### Checklist de validação

- (OK) Conferir número de vértices e arestas criadas
- (OK) Testar Dijkstra entre dois nós próximos
- (OK) Validar se a distância e o caminho fazem sentido
- (OK) Verificar se o caminho respeita ruas de mão única
- (!) Algumas esquinas podem estar ausentes no dataset
- (!) Visualização no mapa é opcional mas recomendada

## Problemas Conhecidos

### 1. Esquinas faltando no dataset

Algumas interseções esperadas podem não existir nos JSONs. Exemplo:

- ❌ "lobo da costa x goncalves chaves" (não existe)
- ✅ "lobo da costa x santa cruz" (existe)

### 2. Saltos inesperados no caminho

Se o caminho parece "pular" ruas, provavelmente é porque:

- As ruas intermediárias são de **mão única** (`oneway: true`)
- O algoritmo segue o único caminho legal disponível

**Verificação**: Analisar o campo `oneway` e `reversed` das arestas no caminho.
