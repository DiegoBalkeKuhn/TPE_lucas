  #ifndef B_TREE_H
  #define B_TREE_H

  #include <stdio.h>
  #include <stdlib.h>

  typedef struct BTreeNode {
    int *keys; // Array of keys
    int numKeys; // Current number of keys
    struct BTreeNode *esq, *dir;
    int leaf;
    struct BTreeNode **children;
  } BTreeNode;


  BTreeNode* createNode(int t, int leaf);
  void insert(BTreeNode** root, int key, int t);
  void splitChild(BTreeNode* parent, int index, int t);
  void insertNonFull(BTreeNode* node, int key, int t);
  void registrar(BTreeNode *root, FILE *file);



  #endif // B_TREE_H
