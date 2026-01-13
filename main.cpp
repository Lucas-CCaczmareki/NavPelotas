#include <iostream>
#include "trie/trie.h"
#include "fileModule/fileModule.h"

/*
[X] - separate code with header files
[X] - read label to node file line by line
  [X] - callback function pointer to process line/node
[X] - trie structure with necessary methods for autocomplete
[ ] - GUI autocomplete
*/

int main()
{
  trie::TrieNode *rootNode = trie::init();

  fileModule::processNodesToLabel(rootNode, trie::insertWord);

  trie::SearchResult results[300];

  // autocomplete logic
  while (true)
  { // GUI
    std::cout << "manda o bagulho ai: ";
    std::string fodase;
    std::cin >> fodase;

    // "para parar escreve 0" n vou fazer fodase

    int searchResultsCount = 0;
    trie::search(rootNode, fodase, results, &searchResultsCount);
    for (int i = 0; i < searchResultsCount; i++)
    {
      std::cout << " -> label: " << results[i].label << "\n";
      std::cout << " -> nodeids: | ";
      for (int j = 0; j < results[i].nodeIdsSize; j++)
      {
        std::cout << results[i].nodeIds[j] << " | ";
      }
      std::cout << "\n";
      std::cout << "\n";
    }
  }
  // autocomplete logic

  return 0;
}
