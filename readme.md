# NavPelotas - Sistema de Navegação viária AED2
## Integrantes: Lucas Cavallin Caczmareki, João Godinho, Matheus Persch

## Visão Geral
Projeto desenvolvido para a disciplina de Algoritmos e Estruturas de dados 2 (AED2). Implementando um sistema de cálculo de rotas e autosugestão com trie, utilizando os dados do mapa de Pelotas do OpenStreetMap (OSM) para popular o grafo e usando a distância física entre um nodo e outro como peso para o Dijkstra.


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

### Grafo (Lista de Adjacência)
O grafo foi implementado usando lista de adjacência, pois dataset utilizado resulta num grafo esparso. Cada node é uma intersecção de rua (geralmente esquinas) e cada aresta um segmento de rua, com o peso definido pela distância física entre os nós.

### - Mapeamento de IDs (id <-> index)

Os IDs são números gigantes e não tem uma lógica sequencial exata (467969432). Isso inviabiliza o uso direto dele como índice de um vetor. Por esse motivo o mapa mantém um hashmap para mapear ID pra índice interno, e um vetor para transformar o índice interno em ID novamente caso necessário. Isso permite economizar memória, pois evitamos espaços enormes desperdiçados devido ao uso de long long.


## Trie (Busca + Autossugestão)
A trie foi implementada, e é utilizada para permitir a busca eficiente dos nomes das intersecções, permitindo autocompletar com múltiplos possíveis resultados para um mesmo prefixo. No código, cada palavra inserida pode estar associada a vários IDs de nós, refletindo ruas longas ou interseções complexas (1 label pode se referir a uma lista de nodes). A estrutura foi implementada com mapeamento direto de caracteres para índices, garantindo acesso O(1) por caractere e complexidade linear no tamanho da string, sem uso de mapas ou hashing.

## Dijkstra
Responsável por calcular o menor caminho entre dois nós do grafo, usando a distância física (lenght) como peso para as arestas. Ele foi implementado utilizando uma fila de prioridade, para mover para o nó com a menor distância conhecida sem precisar visitar todo vetor de vizinhos, garantido a eficiência. O vetor "prev" permite a reconstrução do caminho percorrido.

## Possíveis problemas conhecidos
### 1. Esquinas trocadas no dataset

Algumas interseções esperadas podem não existir com seus nomes exatos nos JSONs. Exemplo:

- "lobo da costa x goncalves chaves" não existe, porém "andreia fetter zambrano x goncalves chaves" existe.
- "andrade neves x tiradentes" não existe, porém "andrade neves x sete de julho" existe.

Também existem alguns IDs de nós sem label no dataset, que aparecerão no caminho descrito.