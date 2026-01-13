#pragma once

namespace fileModule
{
  void processNodesToLabel(trie::TrieNode *trieRoot, void (*cb)(trie::TrieNode *trieRoot, std::string word, unsigned int nodeId));
}