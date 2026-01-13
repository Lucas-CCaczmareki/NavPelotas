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
      std::cout << "line: " << line << "\n";
    }
    inFile.close();
  }

}