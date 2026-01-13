#include <iostream>
#include "trie/trie.h"
#include "fileModule/fileModule.h"

/*
[X] - separate code with header files
[X] - read label to node file line by line
[ ] - trie structure
  [ ] - insert + prefix search
  [ ] - delete?why?..
[ ] - insert data on trie "lugarA x lugarB"
  -- payload with nodes (id)
[ ] - CLI autocomplete // remove it just for test.
[ ] - GUI autocomplete
*/

int main()
{
  fileModule::readNodesToLabel();

  trie::foo();

  return 0;
}
