#include "B-tree.h"






BTreeNode* createNode(int t, int leaf) {
    BTreeNode* node = (BTreeNode*)malloc(sizeof(BTreeNode));
    node->keys = (int*)malloc((2 * t - 1) * sizeof(int));
    node->children = (BTreeNode**)malloc(2 * t * sizeof(BTreeNode*));
    node->numKeys = 0;
    node->leaf = leaf;
    return node;
}

void insert(BTreeNode** root, int key, int t) {
    if (*root == NULL) {
        *root = createNode(t, 1);
        (*root)->keys[0] = key;
        (*root)->numKeys = 1;
    } else {
        if ((*root)->numKeys == 2 * t - 1) {
            BTreeNode *newRoot = createNode(t, 0);
            newRoot->children[0] = *root;
            splitChild(newRoot, 0, t);
            insertNonFull(newRoot, key, t);
            *root = newRoot;
        } else {
            insertNonFull(*root, key, t);
        }
    }
}

void splitChild(BTreeNode* parent, int index, int t) {
    BTreeNode* fullChild = parent->children[index];
    BTreeNode* newChild = createNode(t, fullChild->numKeys == 2 * t - 1);
    newChild->numKeys = t - 1;

    for (int i = 0; i < t - 1; i++) {
        newChild->keys[i] = fullChild->keys[i + t];
    }

    if (fullChild->numKeys == 2 * t - 1) {
        for (int i = 0; i < t; i++) {
            newChild->children[i] = fullChild->children[i + t];
        }
    }

    fullChild->numKeys = t - 1;

    for (int i = parent->numKeys; i >= index + 1; i--) {
        parent->children[i + 1] = parent->children[i];
    }
    parent->children[index + 1] = newChild;

    for (int i = parent->numKeys - 1; i >= index; i--) {
        parent->keys[i + 1] = parent->keys[i];
    }
    parent->keys[index] = fullChild->keys[t - 1];
    parent->numKeys++;
}

void insertNonFull(BTreeNode* node, int key, int t) {
    int i = node->numKeys - 1;

    if (node->children[0] == NULL) { // Leaf node
        while (i >= 0 && node->keys[i] > key) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->numKeys++;
    } else { // Internal node
        while (i >= 0 && node->keys[i] > key) {
            i--;
        }
        i++;
        if (node->children[i]->numKeys == 2 * t - 1) {
            splitChild(node, i, t);
            if (node->keys[i] < key) {
                i++;
            }
        }
        insertNonFull(node->children[i], key, t);
    }
}


