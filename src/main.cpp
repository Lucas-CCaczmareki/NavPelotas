#include <iostream>
#include <fstream> //provavelmente pra acessar files
#include <vector>
#include <string>
// #include "Graph.h"
#include "json.hpp"
#include <unordered_map>


int main() {
    using json = nlohmann::json; //só pra escrever menos coisa. Faz json ser uma keywork que é nlohmann::json
    
    json j_file;
    std::ifstream file("data/nodes.json");

    if(!file.is_open()) {
        std::cout << "Erro ao abrir o arquivo!\n";
        exit(1);
    }   

    // Joga o fluxo de entrada de caracteres do arquivo pro tipo json da biblioteca.
    // Esse tipo se vira automaticamente pra saber oq tem no json, se um array ou objeto, ou array de objetos etc.
    // A partir daqui o j_file é representado de acordo com a estrutura dele, e eu acesso como array de objetos
        // no caso do nodes.json
    file >> j_file; 

    // Cria uma hashtable
    std::unordered_map<long long, int> idToIndex;
    
    // indica pra hashtable q eu vou botar +- esse tanto de elementos
    // isso evita várias realocações durante um loop, por exemplo. É tipo aquele tamanho inicial quando eu implementei
    idToIndex.reserve(j_file.size()); 


    int i = 0;
    for(auto& node : j_file) {
        // A ordem dos números do nodo pra mim não interessa. Me interessa que o id vire um número pequeno.
        long long id = node["id"];
        idToIndex[id] = i;  //isso aqui faz o id sempre mapear pro index i, na mesma ordem que eles tão escrito no json

        std::cout << "ID: " << id << "\tINDEX: " << idToIndex[id] << "\n";

        // faz só pros 10 primeiros pra testar
        i++;
        if(i >= 10) {
            break;
        }
    }

    

}