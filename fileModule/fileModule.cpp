#include <iostream>
#include "../include/trie.h"
#include <fstream>
#include "fileModule.h"

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

    std::string line;
    while (std::getline(inFile, line))
    {
      int isReadingLabel = 0;
      std::string node;
      std::string label;
      for (int i = 0; i < line.length(); i++)
      {
        if (line[i] == '"' || line[i] == '{' || line[i] == '}' || line[i] == ',')
        {
          continue;
        }

        // move to read label moment
        if (line[i] == ':')
        {
          isReadingLabel = 1;
          // Skip space after :
          if (i + 1 < line.length() && line[i + 1] == ' ')
          {
            i++;
          }
          continue;
        }

        if (isReadingLabel)
        {
          label += line[i];
        }
        else
        {
          node += line[i];
        }
      }

      // we can have TWO or MORE nodes for same label
      // trie path -> label
      // each label has its payloads -> int nodes[] OR better std::vector<int> nodes;
      if (!(label.empty() && node.empty()))
      {
        unsigned long nodeId = std::stoul(node); // TODO: try catch here cpp feature avoid outofrange or anything else
        cb(trieRoot, label, nodeId);
      }
    }

    inFile.close();
  }

}
