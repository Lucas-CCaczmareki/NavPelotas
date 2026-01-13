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
  // trie::foo();

  fileModule::processNodesToLabel(fakeInsertOnTrie);


  return 0;
}
