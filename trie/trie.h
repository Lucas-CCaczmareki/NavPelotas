#pragma once // include only once same header

namespace trie
{
  constexpr int ACCEPTED_SYMBOLS_AMOUNT = 40; // 26abc, 10 digits, 4 symbols (- space . ')

  struct TrieNode
  {
    struct TrieNode *next[ACCEPTED_SYMBOLS_AMOUNT];
    std::string word;
    char val;
    int isEndOfWord;
    unsigned long *nodeIds;
    int nodeIdsSize;
  } typedef TrieNode;

  struct SearchResult
  {
    std::string label;
    unsigned long *nodeIds;
    int nodeIdsSize;
  };

  TrieNode *init();
  void insertWord(TrieNode *trieRoot, std::string word, unsigned long nodeId);
  void search(TrieNode *node, std::string word, SearchResult results[], int *resultsCount);
  void destroy(TrieNode *root);
}