#include <iostream>
#include <fstream>

namespace fileModule
{
  void readNodesToLabel()
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
        std::cout << "node: " + node << "\n";
        std::cout << "label: " + label << "\n";
        std::cout << "\n";
        std::cout << "\n";
      }
    }

    inFile.close();
  }

}