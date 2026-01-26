#include <iostream>
#include "../include/trie.h"
#include <fstream>
#include "fileModule.h"
#include "json.hpp"

using json = nlohmann::json;

namespace fileModule
{
  void processNodesToLabel(trie::TrieNode *trieRoot, void (*cb)(trie::TrieNode *trieRoot, std::string word, unsigned long nodeId))
  {
    std::ifstream inFile;
    inFile.open("./data/nodes_to_label.json");
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
            unsigned long nodeId = std::stoul(element.key());

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
