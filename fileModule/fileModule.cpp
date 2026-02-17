/*
#include <iostream>
#include "../include/trie.h"
#include <fstream>
#include "fileModule.h"
#include "json.hpp"

using json = nlohmann::json;

namespace fileModule
{
  void processNodesToLabel(trie::TrieNode *trieRoot, void (*cb)(trie::TrieNode *trieRoot, std::string word, unsigned long long nodeId))
  {
    std::string path = "data/nodes_to_label_formatado.json";
    std::ifstream inFile(path);
    // Se falhar, tenta voltar uma pasta (igual no Graph)
    if (!inFile.is_open()) {
        inFile.clear();
        path = "../data/nodes_to_label_formatado.json";
        inFile.open(path);
    }
    if (!inFile)
    {
      throw std::runtime_error("Cannot open nodes_to_label.json");
    }
    std::cout << "inFile.is_open: " << (inFile.is_open() ? "yes" : "not") << "\n";

    json j_file;
    try {
        inFile >> j_file;
    } catch (const json::parse_error& e) {
        std::cerr << "Sintaxe error in JSON: " << e.what() << "\n";
        return;
    }

    for (auto& element : j_file.items()) {
        try {
            // convert to long
            unsigned long long nodeId = std::stoul(element.key());

            // value is the street name
            std::string label = element.value();

            if (!label.empty()) {
                // calling calback
                cb(trieRoot, label, nodeId);
            }

        } catch (const std::exception& e) {
            // continue if error;
            continue;
        }
    }

    std::cout << "done!";
    inFile.close();
  }
  }
*/
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "fileModule.h"
#include "../include/trie.h"

namespace fileModule
{
// Função manual para ler JSON sem travar memória
void processNodesToLabel(trie::TrieNode *trieRoot, void (*cb)(trie::TrieNode *trieRoot, std::string word, unsigned long long nodeId))
{
    std::setvbuf(stdout, NULL, _IONBF, 0);

    std::string filename = "nodes_to_label_formatado.json";
    std::ifstream inFile("data/" + filename);

    if (!inFile.is_open()) {
        inFile.clear();
        inFile.open("../data/" + filename);
    }

    if (!inFile.is_open()) {
        std::cerr << "ERRO FATAL: Nao foi possivel abrir '" << filename << "'.\n";
        return;
    }

    std::cout << "Carregando ruas ";

    std::string line;
    int count = 0;

    while (std::getline(inFile, line))
    {
        if (line.length() < 5) continue; // Pula linhas inúteis

        // Parser manual simplificado
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) continue;

        // Pega ID (esquerda)
        std::string idStr = line.substr(0, colonPos);
        // Remove aspas e espaços
        idStr.erase(std::remove(idStr.begin(), idStr.end(), '"'), idStr.end());
        idStr.erase(std::remove(idStr.begin(), idStr.end(), ' '), idStr.end());
        idStr.erase(std::remove(idStr.begin(), idStr.end(), '{'), idStr.end());
        idStr.erase(std::remove(idStr.begin(), idStr.end(), ','), idStr.end());

        // Pega Nome (direita)
        size_t firstQuote = line.find('"', colonPos);
        size_t lastQuote = line.rfind('"');
        std::string label = "";

        if (firstQuote != std::string::npos && lastQuote > firstQuote) {
            label = line.substr(firstQuote + 1, lastQuote - firstQuote - 1);
        }

        if (!idStr.empty() && !label.empty())
        {
            try {
                // stoull = String to Unsigned Long Long (64 bits)
                unsigned long long nodeId = std::stoull(idStr);
                cb(trieRoot, label, nodeId);
                count++;
            } catch (...) {
                continue;
            }
        }

        if (count % 100000 == 0) std::cout << ".";
    }

    std::cout << "\nSUCESSO! " << count << " nomes de ruas carregados.\n";
    inFile.close();
}
}
