#ifndef B_TREE_H
#define B_TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define T 3  // Grau mínimo da B-tree (cada nó tem entre T-1 e 2T-1 chaves)

typedef struct BTreeNode {
    int *keys;              // Array de chaves
    int numKeys;            // Número atual de chaves
    struct BTreeNode *esq, *dir;
    int leaf;               // 1 se for folha
    struct BTreeNode **children; // Filhos
} BTreeNode;


BTreeNode* createNode(int t, int leaf);
void insert(BTreeNode** root, int key, int t);
void splitChild(BTreeNode* parent, int index, int t);
void insertNonFull(BTreeNode* node, int key, int t);


BTreeNode* search(BTreeNode* root, int key);
void delete(BTreeNode** root, int key, int t);

void printInOrder(BTreeNode* root);

void saveTree(BTreeNode* root, FILE* file);
BTreeNode* loadTree(FILE* file, int t);
void freeTree(BTreeNode* node);

void registrar(BTreeNode **root, FILE *file, int t);
void lerRegistros(BTreeNode *root);
void apagarRegistro(BTreeNode **root, FILE *file, int t);

#endif // B_TREE_H
