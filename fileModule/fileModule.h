#pragma once
#include <string>
#include "../include/trie.h"
namespace trie {
struct TrieNode;
}
namespace fileModule
{
  void processNodesToLabel(trie::TrieNode *trieRoot, void (*cb)(trie::TrieNode *trieRoot, std::string word, unsigned long long nodeId));
}
