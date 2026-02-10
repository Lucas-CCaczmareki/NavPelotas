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

## How to run on unix CLI:
- 1. `g++ -std=c++17 -O2 src/main.cpp src/Graph.cpp src/dijkstra.cpp src/trie.cpp fileModule/fileModule.cpp -Iinclude -o main && ./main`

## How to run with valgrind to check any memory leak
1. - install docker on oficial website
2. - Run on terminal: `docker build -t valgrind-runner . && docker run valgrind-runner`
  - macos doesn't support valgrind so I've to use a docker container for this D:


## Decisoes e resumos

- O que justifica usar lista de adjacencia nesse caso?
  - Quando o grafo é esparso é preferivel lista de adjacencia pra economizar memoria e percorrer mais facil os vizinhos.
  - Quando o grafo é denso é preferivel matriz de adjacencia porque faz acesso em O(1) pra checar se existe aresta entre dois vertices.
    - **grafo esparso** -> poucas arestas por vértice
    - **grafo denso** -> quase todas arestas possiveis existem. edges ~= nodes ^2
  - **Motivo de usar lista de adjacencia**: Ao rodar dijkstra com lista de adjacencia se o grafo é esparso temos O(V+E),
  e se for matriz de adjacencia teriamos O(V^2) o que é ruim num cenário onde o grafo é esparso, e seria bom caso o grafo fosse denso.
  - **Esse grafo é esparso porque**: vertices=8185, arestas=39625, densidade=0.000591541
  - referencias: cormen + https://www.youtube.com/watch?v=jlKzLi16fuo + https://en.wikipedia.org/wiki/Dense_graph

- Motivo pra mapear id pra index e index pra id no grafo?
  - id pra index -> id é um numero enorme que pode nao ser sequencial, dessa forma ia ter um vector todo bugado
  - index pra id -> precisamos saber o id original do node (por exemplo, ao mostrar resultados ou buscar o nodo pelo JSON)

- Motivo da logica pra mapear char pra index na trie e pra que aqueles casos especiais de -, etc?
  - Acessar em O(1) cada char na trie e assim ter o O(n) pra cada string onde n = tamanho da string; E aqueles ifs especificos sao pra suportar os caracteres especiais necessarios nos nome das ruas;

- O que contem cada arquivo?
  - nodes.json -> cada node é uma esquina no mapa com latitude e longitude
  - edges.json -> ligacao entre duas esquinas com informacoes de sentido e distancia

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

## Implementação da Trie (Busca por nomes de ruas)

A **Trie** é utilizada para permitir **busca eficiente por nomes de ruas**, suportando **autocompletar** e **mapeamento de múltiplos IDs de nós** associados a uma mesma rua ou interseção.

Ela é usada principalmente para:
- Buscar ruas pelo **prefixo digitado pelo usuário**
- Retornar **todas as possíveis interseções** associadas àquele nome
- Fazer isso em tempo **linear no tamanho da string**

---

### Estrutura da Trie

Cada nó da Trie (`TrieNode`) contém:

- **`char val`**  
  Caractere armazenado no nó.

- **`TrieNode* next[ACCEPTED_SYMBOLS_AMOUNT]`**  
  Vetor de ponteiros para os próximos nós.  
  O índice é calculado diretamente a partir do caractere (acesso O(1)).

- **`bool isEndOfWord`**  
  Indica se aquele nó representa o final de uma palavra válida.

- **`std::string word`**  
  Palavra completa associada ao nó final (ex: nome da rua).

- **`unsigned long long* nodeIds`**  
  Lista dinâmica de IDs de nós do grafo associados àquela palavra.

- **`int nodeIdsSize`**  
  Quantidade de IDs associados.

---

### Mapeamento de caracteres (char → índice)

Para garantir **acesso O(1)**, cada caractere suportado é mapeado para um índice fixo:

- `'a'` a `'z'` → `0` a `25`
- `'0'` a `'9'` → `26` a `35`
- `'-'` → `36`
- `' '` (espaço) → `37`
- `'.'` → `38`
- `'`'` → `39`

Caracteres fora desse conjunto são **ignorados**.

**Motivo**:
- Evita uso de `map` ou `unordered_map`
- Garante busca e inserção em **O(n)**, onde `n` é o tamanho da string

---

### Inserção de palavras (`insertWord`)

Ao inserir uma palavra:
1. Percorre a Trie caractere por caractere
2. Cria nós quando necessário
3. No final da palavra:
   - Marca `isEndOfWord = true`
   - Armazena a palavra completa
   - Adiciona o `nodeId` à lista dinâmica de IDs

Isso permite que:
- Uma mesma rua esteja associada a **vários IDs**
- Interseções complexas sejam corretamente representadas

---

### Busca (`search`)

A busca funciona em dois cenários:

#### 1. Palavra completa
Se o usuário digitar exatamente o nome de uma rua:
- Retorna **um único resultado**
- Com todos os IDs associados àquela palavra

#### 2. Prefixo
Se o usuário digitar apenas parte do nome:
- A Trie percorre até o fim do prefixo
- Realiza uma **DFS** a partir desse ponto
- Retorna **todas as palavras possíveis** que completam o prefixo

Há uma **trava de segurança** que limita o retorno a **100 resultados**, evitando explosão de memória ou tempo.

---

### Complexidade

- **Inserção**: `O(n)`
- **Busca por palavra/prefixo**: `O(n)`
- Onde `n` é o tamanho da string

A DFS para autocompletar depende da quantidade de resultados possíveis, mas é limitada por segurança.

---

### Liberação de memória (`destroy`)

A Trie é destruída usando **DFS**:
- Garante que nenhum nó perca referência
- Libera corretamente:
  - Nós alocados com `new`
  - Vetores de IDs alocados com `realloc`

Isso evita **memory leaks** e mantém o uso de memória controlado.

---

### Resumo

- A Trie permite **busca rápida e eficiente** por nomes de ruas
- Suporta **autocompletar**
- Trata corretamente **múltiplos IDs por nome**
- Mantém complexidade linear
- Evita estruturas mais custosas como mapas ou hashes



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

- "lobo da costa x goncalves chaves" (não existe)
- "lobo da costa x santa cruz" (existe)

### 2. Saltos inesperados no caminho

Se o caminho parece "pular" ruas, provavelmente é porque:

- As ruas intermediárias são de **mão única** (`oneway: true`)
- O algoritmo segue o único caminho legal disponível

**Verificação**: Analisar o campo `oneway` e `reversed` das arestas no caminho.
