#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Alphabet size (# of symbols)
#define ALPHABET_SIZE (26)

// Converts key current character into index
// use only 'a' through 'z' and lower case
#define CHAR_TO_INDEX(c) ((int)c - (int)'a')

struct TrieNode { 
    struct TrieNode *children[ALPHABET_SIZE];

    // isEndOfWord is true if the node represents
    // end of a word
    bool isEndOfWord;
};

struct TrieNode *getNode(void);
void insert(struct TrieNode *root, const char *key);
bool search(struct TrieNode *root, const char *key);
