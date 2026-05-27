#include "B-tree.h"

/* ─── Criação de nó ──────────────────────────────────────────────────── */

BTreeNode* createNode(int t, int leaf) {
    BTreeNode* node = (BTreeNode*)malloc(sizeof(BTreeNode));
    node->keys     = (int*)calloc((2 * t - 1), sizeof(int));
    node->children = (BTreeNode**)calloc(2 * t,  sizeof(BTreeNode*));
    node->numKeys  = 0;
    node->leaf     = leaf;
    node->esq      = NULL;
    node->dir      = NULL;
    return node;
}

/* ─── Inserção ───────────────────────────────────────────────────────── */

void splitChild(BTreeNode* parent, int index, int t) {
    BTreeNode* fullChild = parent->children[index];
    BTreeNode* newChild  = createNode(t, fullChild->leaf);
    newChild->numKeys    = t - 1;

    for (int i = 0; i < t - 1; i++)
        newChild->keys[i] = fullChild->keys[i + t];

    if (!fullChild->leaf)
        for (int i = 0; i < t; i++)
            newChild->children[i] = fullChild->children[i + t];

    fullChild->numKeys = t - 1;

    for (int i = parent->numKeys; i >= index + 1; i--)
        parent->children[i + 1] = parent->children[i];
    parent->children[index + 1] = newChild;

    for (int i = parent->numKeys - 1; i >= index; i--)
        parent->keys[i + 1] = parent->keys[i];
    parent->keys[index] = fullChild->keys[t - 1];
    parent->numKeys++;
}

void insertNonFull(BTreeNode* node, int key, int t) {
    int i = node->numKeys - 1;

    if (node->leaf) {
        while (i >= 0 && node->keys[i] > key) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->numKeys++;
    } else {
        while (i >= 0 && node->keys[i] > key) i--;
        i++;
        if (node->children[i]->numKeys == 2 * t - 1) {
            splitChild(node, i, t);
            if (node->keys[i] < key) i++;
        }
        insertNonFull(node->children[i], key, t);
    }
}

void insert(BTreeNode** root, int key, int t) {
    if (*root == NULL) {
        *root = createNode(t, 1);
        (*root)->keys[0] = key;
        (*root)->numKeys = 1;
    } else {
        if ((*root)->numKeys == 2 * t - 1) {
            BTreeNode* newRoot    = createNode(t, 0);
            newRoot->children[0] = *root;
            splitChild(newRoot, 0, t);
            insertNonFull(newRoot, key, t);
            *root = newRoot;
        } else {
            insertNonFull(*root, key, t);
        }
    }
}

/* ─── Busca ─────────────────────────────────────────────────────────── */

BTreeNode* search(BTreeNode* root, int key) {
    if (root == NULL) return NULL;
    int i = 0;
    while (i < root->numKeys && key > root->keys[i]) i++;
    if (i < root->numKeys && root->keys[i] == key) return root;
    if (root->leaf) return NULL;
    return search(root->children[i], key);
}

/* ─── Remoção ────────────────────────────────────────────────────────── */

/* Retorna o predecessor (maior chave na sub-árvore esquerda do filho) */
static int getPredecessor(BTreeNode* node, int idx) {
    BTreeNode* cur = node->children[idx];
    while (!cur->leaf) cur = cur->children[cur->numKeys];
    return cur->keys[cur->numKeys - 1];
}

/* Retorna o sucessor (menor chave na sub-árvore direita do filho) */
static int getSuccessor(BTreeNode* node, int idx) {
    BTreeNode* cur = node->children[idx + 1];
    while (!cur->leaf) cur = cur->children[0];
    return cur->keys[0];
}

/* Empurra a chave do pai para o filho da esquerda e pega do irmão direito */
static void borrowFromNext(BTreeNode* node, int idx) {
    BTreeNode* child  = node->children[idx];
    BTreeNode* sibling = node->children[idx + 1];

    child->keys[child->numKeys] = node->keys[idx];
    if (!child->leaf)
        child->children[child->numKeys + 1] = sibling->children[0];

    node->keys[idx] = sibling->keys[0];

    for (int i = 1; i < sibling->numKeys; i++) sibling->keys[i - 1] = sibling->keys[i];
    if (!sibling->leaf)
        for (int i = 1; i <= sibling->numKeys; i++) sibling->children[i - 1] = sibling->children[i];

    child->numKeys++;
    sibling->numKeys--;
}

/* Empurra a chave do pai para o filho da direita e pega do irmão esquerdo */
static void borrowFromPrev(BTreeNode* node, int idx) {
    BTreeNode* child  = node->children[idx];
    BTreeNode* sibling = node->children[idx - 1];

    for (int i = child->numKeys - 1; i >= 0; i--) child->keys[i + 1] = child->keys[i];
    if (!child->leaf)
        for (int i = child->numKeys; i >= 0; i--) child->children[i + 1] = child->children[i];

    child->keys[0] = node->keys[idx - 1];
    if (!child->leaf)
        child->children[0] = sibling->children[sibling->numKeys];

    node->keys[idx - 1] = sibling->keys[sibling->numKeys - 1];

    child->numKeys++;
    sibling->numKeys--;
}

/* Funde filho[idx] com filho[idx+1] usando keys[idx] como separador */
static void merge(BTreeNode* node, int idx, int t) {
    BTreeNode* child  = node->children[idx];
    BTreeNode* sibling = node->children[idx + 1];

    child->keys[t - 1] = node->keys[idx];

    for (int i = 0; i < sibling->numKeys; i++) child->keys[i + t] = sibling->keys[i];
    if (!child->leaf)
        for (int i = 0; i <= sibling->numKeys; i++) child->children[i + t] = sibling->children[i];

    for (int i = idx + 1; i < node->numKeys; i++) node->keys[i - 1] = node->keys[i];
    for (int i = idx + 2; i <= node->numKeys; i++) node->children[i - 1] = node->children[i];

    child->numKeys += sibling->numKeys + 1;
    node->numKeys--;

    free(sibling->keys);
    free(sibling->children);
    free(sibling);
}

/* Garante que o filho[idx] tem pelo menos t chaves antes de descer */
static void fill(BTreeNode* node, int idx, int t) {
    if (idx != 0 && node->children[idx - 1]->numKeys >= t)
        borrowFromPrev(node, idx);
    else if (idx != node->numKeys && node->children[idx + 1]->numKeys >= t)
        borrowFromNext(node, idx);
    else {
        if (idx != node->numKeys) merge(node, idx, t);
        else merge(node, idx - 1, t);
    }
}

static void deleteFromNode(BTreeNode** rootRef, BTreeNode* node, int key, int t);

static void deleteFromLeaf(BTreeNode* node, int idx) {
    for (int i = idx + 1; i < node->numKeys; i++) node->keys[i - 1] = node->keys[i];
    node->numKeys--;
}

static void deleteFromInternal(BTreeNode** rootRef, BTreeNode* node, int idx, int t) {
    int key = node->keys[idx];
    if (node->children[idx]->numKeys >= t) {
        int pred = getPredecessor(node, idx);
        node->keys[idx] = pred;
        deleteFromNode(rootRef, node->children[idx], pred, t);
    } else if (node->children[idx + 1]->numKeys >= t) {
        int succ = getSuccessor(node, idx);
        node->keys[idx] = succ;
        deleteFromNode(rootRef, node->children[idx + 1], succ, t);
    } else {
        merge(node, idx, t);
        deleteFromNode(rootRef, node->children[idx], key, t);
    }
}

static void deleteFromNode(BTreeNode** rootRef, BTreeNode* node, int key, int t) {
    int idx = 0;
    while (idx < node->numKeys && node->keys[idx] < key) idx++;

    if (idx < node->numKeys && node->keys[idx] == key) {
        if (node->leaf) deleteFromLeaf(node, idx);
        else            deleteFromInternal(rootRef, node, idx, t);
    } else {
        if (node->leaf) { printf("Chave %d nao encontrada.\n", key); return; }

        int isLast = (idx == node->numKeys);
        if (node->children[idx]->numKeys < t) fill(node, idx, t);

        if (isLast && idx > node->numKeys) deleteFromNode(rootRef, node->children[idx - 1], key, t);
        else                               deleteFromNode(rootRef, node->children[idx], key, t);
    }
}

void delete(BTreeNode** root, int key, int t) {
    if (*root == NULL) { printf("Arvore vazia.\n"); return; }

    deleteFromNode(root, *root, key, t);

    /* Se a raiz ficou sem chaves após fusão, o filho torna-se a nova raiz */
    if ((*root)->numKeys == 0) {
        BTreeNode* tmp = *root;
        if ((*root)->leaf) *root = NULL;
        else               *root = (*root)->children[0];
        free(tmp->keys);
        free(tmp->children);
        free(tmp);
    }
}

/* ─── Impressão em ordem ─────────────────────────────────────────────── */

void printInOrder(BTreeNode* root) {
    if (root == NULL) return;
    for (int i = 0; i < root->numKeys; i++) {
        printInOrder(root->children[i]);
        printf("%d ", root->keys[i]);
    }
    printInOrder(root->children[root->numKeys]);
}

/* ─── Liberação de memória ───────────────────────────────────────────── */

void freeTree(BTreeNode* node) {
    if (node == NULL) return;
    if (!node->leaf)
        for (int i = 0; i <= node->numKeys; i++)
            freeTree(node->children[i]);
    free(node->keys);
    free(node->children);
    free(node);
}

/* ─── Persistência (serialização binária) ────────────────────────────── */
/*
   Formato de cada nó no arquivo:
   [int leaf][int numKeys][int keys[numKeys]]
   Depois, recursivamente, os filhos (se não for folha).
   Um marcador -1 indica ponteiro nulo.
*/

void saveTree(BTreeNode* root, FILE* file) {
    if (root == NULL) {
        int marker = -1;
        fwrite(&marker, sizeof(int), 1, file);
        return;
    }
    int marker = 1;
    fwrite(&marker,        sizeof(int), 1,            file);
    fwrite(&root->leaf,    sizeof(int), 1,            file);
    fwrite(&root->numKeys, sizeof(int), 1,            file);
    fwrite(root->keys,     sizeof(int), root->numKeys, file);

    if (!root->leaf)
        for (int i = 0; i <= root->numKeys; i++)
            saveTree(root->children[i], file);
}

BTreeNode* loadTree(FILE* file, int t) {
    int marker;
    if (fread(&marker, sizeof(int), 1, file) != 1 || marker == -1)
        return NULL;

    int leaf, numKeys;
    fread(&leaf,    sizeof(int), 1, file);
    fread(&numKeys, sizeof(int), 1, file);

    BTreeNode* node = createNode(t, leaf);
    fread(node->keys, sizeof(int), numKeys, file);
    node->numKeys = numKeys;

    if (!leaf)
        for (int i = 0; i <= numKeys; i++)
            node->children[i] = loadTree(file, t);

    return node;
}

/* ─── Funções de interface ───────────────────────────────────────────── */

void registrar(BTreeNode **root, FILE *file, int t) {
    int key;
    printf("Digite o valor a inserir: ");
    scanf("%d", &key);

    if (search(*root, key) != NULL) {
        printf("Valor %d ja existe na arvore.\n", key);
        return;
    }

    insert(root, key, t);

    /* Reescreve o arquivo com a árvore atualizada */
    rewind(file);
    ftruncate(fileno(file), 0);
    saveTree(*root, file);
    fflush(file);

    printf("Valor %d inserido com sucesso.\n", key);
}

void lerRegistros(BTreeNode *root) {
    if (root == NULL) {
        printf("Nenhum registro encontrado.\n");
        return;
    }
    printf("Registros (em ordem): ");
    printInOrder(root);
    printf("\n");
}

void apagarRegistro(BTreeNode **root, FILE *file, int t) {
    int key;
    printf("Digite o valor a apagar: ");
    scanf("%d", &key);

    if (search(*root, key) == NULL) {
        printf("Valor %d nao encontrado.\n", key);
        return;
    }

    delete(root, key, t);

    rewind(file);
    ftruncate(fileno(file), 0);
    saveTree(*root, file);
    fflush(file);

    printf("Valor %d removido com sucesso.\n", key);
}
