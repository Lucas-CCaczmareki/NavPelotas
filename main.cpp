#include <iostream>
#include "trie/trie.h"
#include "fileModule/fileModule.h"

/*
[X] - separate code with header files
[X] - read label to node file line by line
  [X] - callback function pointer to process line/node
[ ] - trie structure
  [ ] - insert + prefix search
  [ ] - delete?why?..
[ ] - insert data on trie "lugarA x lugarB"
  -- payload with nodes (id)
[ ] - CLI autocomplete // remove it just for test.
[ ] - GUI autocomplete
*/

void fakeInsertOnTrie(std::string label, std::string node)
{
  std::cout << label + ":" + node + "\n";
}

int main()
{
  trie::TrieNode *rootNode = trie::init();

  fileModule::processNodesToLabel(rootNode, trie::insertWord);

  trie::SearchResult results[300];

  // autocomplete logic
  int searchResultsCount = 0;
  trie::search(rootNode, "dez x laudelino g. ribeiro", results, &searchResultsCount);
  std::cout << "results->label: " << results->label << "\n";
  std::cout << "results->nodeIds[0]: " << results->nodeIds[0] << "\n";
  std::cout << "searchResultsCount: " << searchResultsCount << "\n";


  // while (searchResultsCount >= 0)
  // {
  //   std::cout << "searchResult label: " << results[searchResultsCount].label;
  //   searchResultsCount--;
  //   for (int i = 0; i < results[searchResultsCount].nodeIdsSize; i++)
  //   {
  //     std::cout << " -> nodeids: " << results[searchResultsCount].nodeIds[i];
  //   }
  //   std::cout << "\n";
  // }
  // autocomplete logic

  // std::cout << "rootNode->val " << rootNode->val << "\n";
  // std::cout << "rootNode->isEndOfWord " << rootNode->isEndOfWord << "\n";


  return 0;
}
