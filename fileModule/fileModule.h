#pragma once
#include <string>
namespace trie {
struct TrieNode;
}
namespace fileModule
{
  void processNodesToLabel(trie::TrieNode *trieRoot, void (*cb)(trie::TrieNode *trieRoot, std::string word, unsigned long nodeId));
}
