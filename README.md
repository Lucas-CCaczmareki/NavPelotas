## Estrutura do arquivo `edges.json`

Cada entrada do arquivo `edges.json` representa **uma aresta do grafo** (um segmento da malha viária).

### Campos principais

- **`u`**  
  ID do nó de origem (node do OSM).

- **`v`**  
  ID do nó de destino (node do OSM).

- **`key`**  
  Identificador interno do OSM para diferenciar múltiplas arestas entre o mesmo par `(u, v)`.  
  *Não é necessário para o Dijkstra.*

- **`data`**  
  Metadados da aresta:
  - **`length`** (`double`)  
    Comprimento da aresta (peso do Dijkstra).
  - **`oneway`** (`bool`)  
    - `true` → via de mão única  
    - `false` → via de mão dupla
  - **`reversed`** (`bool` ou `array`)  
    Só importa quando `oneway = true`:
    - `false` → sentido `u → v`
    - `true` → sentido `v → u`
    - `array` → múltiplas lanes / geometria composta (ignorar)
  - Outros campos (nome, tipo de via, etc.)  
    *Não usados no algoritmo.*

### Regras usadas no código

- Se `oneway == false`  
  → adiciona arestas `u → v` **e** `v → u`
- Se `oneway == true`:
  - `reversed == false` → `u → v`
  - `reversed == true` → `v → u`
- `reversed` em formato de array é ignorado.

---

## Verificação do grafo

- Conferir número de vértices e arestas criadas
- Testar Dijkstra entre dois nós próximos
- Validar se a distância e o caminho fazem sentido
- Visualização no mapa é opcional
