#include <iostream>
#include "trie.h"
#include <stdlib.h>

namespace trie
{
  namespace detail
  {
    TrieNode *createNode(char nodeChar)
    {
      TrieNode *node = (TrieNode *)malloc(sizeof(TrieNode));
      node->isEndOfWord = 0;
      node->val = nodeChar;
      node->nodeIds = NULL;
      node->nodeIdsSize = 0;
      for (int i = 0; i < ACCEPTED_SYMBOLS_AMOUNT; i++)
      {
        node->next[i] = NULL;
      }
      return node;
    }

    int getTrieCharIndex(char val)
    {
      if (val >= 'a' && val <= 'z')
        return val - 'a'; // 0 to 25

      if (val >= '0' && val <= '9')
        return 26 + (val - '0'); // 26 to 35

      switch (val)
      {
      case '-':
        return 36;
      case ' ':
        return 37;
      case '.':
        return 38;
      case '\'':
        return 39;
      default:
        return -1; // unsupported
      }
    }
  }

  TrieNode *init()
  {
    return detail::createNode('\0');
  }

  void insertWord(TrieNode *node, std::string word, unsigned long nodeId)
  {
    TrieNode *currNode = node;

    for (int i = 0; i < word.length(); i++)
    {
      char currCharToInsert = word[i];
      int trieCharIndex = detail::getTrieCharIndex(currCharToInsert);
      if (trieCharIndex == -1)
      {
        std::cout << "skipping char: " << currCharToInsert << "since it's not supported\n";
        continue;
      }

      if (currNode->next[trieCharIndex] == NULL)
      {
        currNode->next[trieCharIndex] = detail::createNode(currCharToInsert);
      }
      currNode = currNode->next[trieCharIndex];
    }

    currNode->isEndOfWord = 1;

    unsigned long *newIdsTemp = (unsigned long *)realloc(currNode->nodeIds, sizeof(unsigned long) * (currNode->nodeIdsSize + 1));
    if (!newIdsTemp)
    {
      std::cerr << "error reallocating currNode->nodeIds\n";
      exit(1);
    }
    currNode->nodeIds = newIdsTemp;
    currNode->nodeIds[currNode->nodeIdsSize] = nodeId;
    currNode->nodeIdsSize++;
  }

  void search(TrieNode *node, std::string word, SearchResult results[], int *outCount)
  {

    std::cout << "searching for " + word + "\n";
    *outCount = 0;

    TrieNode *currNode = node;
    for (int currWordIndex = 0; currWordIndex < word.length(); currWordIndex++)
    {
      char currChar = word[currWordIndex];
      int currTrieCharIndex = detail::getTrieCharIndex(currChar);

      if (currTrieCharIndex == -1)
      {
        std::cout << "skipping unsupported char: " << currChar << "\n";
        continue;
      }

      if (currNode->next[currTrieCharIndex] == NULL)
      {
        return;
      }
      currNode = currNode->next[currTrieCharIndex];
    }

    // can be end of word (user typed full word)
    if (currNode->isEndOfWord)
    {
      *outCount = 1;
      results[0].label = word;
      results[0].nodeIds = currNode->nodeIds;
      results[0].nodeIdsSize = currNode->nodeIdsSize;
      return;
    }
    else
    {
      // can be prefix of 1 or more words
      // iterate until all possible ends and return on searchResults
    }

    // if(!currNode->isEndOfWord) {
    //   return;
    // }
  };

  void destroy(TrieNode *currNode)
  {
    if (currNode == NULL)
    {
      return;
    }

    for (int i = 0; i < ACCEPTED_SYMBOLS_AMOUNT; i++)
    {
      if (currNode->next[i] != NULL)
      {
        // DFS because we can't lose references
        destroy(currNode->next[i]);
      }
    }
    if (currNode->nodeIds != NULL)
    {
      free(currNode->nodeIds);
    }
    free(currNode);
  }
}