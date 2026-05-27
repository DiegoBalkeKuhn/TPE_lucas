#include "B-tree.h"

int main() {
    const int t = T;  /* Grau mínimo definido em B-tree.h */

    /* Abre (ou cria) o arquivo de persistência */
    FILE *file = fopen("memoria.dat", "rb+");
    if (!file) {
        /* Arquivo não existe ainda – cria vazio */
        file = fopen("memoria.dat", "wb+");
        if (!file) {
            perror("Erro ao abrir/criar memoria.dat");
            return 1;
        }
    }

    /* Carrega a árvore persistida (pode retornar NULL se arquivo vazio) */
    BTreeNode *root = loadTree(file, t);

    printf("=== Mini Banco de Dados com B-tree (grau %d) ===\n\n", t);

    while (1) {
        printf("\nO que gostaria de fazer?\n");
        printf("  1 - Registrar valor\n");
        printf("  2 - Ler registros\n");
        printf("  3 - Buscar valor\n");
        printf("  4 - Apagar registro\n");
        printf("  0 - Sair\n");
        printf("Opcao: ");

        int opcao = 0;
        if (scanf("%d", &opcao) != 1) {
            /* Limpa buffer em caso de entrada inválida */
            while (getchar() != '\n');
            printf("Entrada invalida.\n");
            continue;
        }

        switch (opcao) {
            case 1:
                registrar(&root, file, t);
                break;

            case 2:
                lerRegistros(root);
                break;

            case 3: {
                int key;
                printf("Digite o valor a buscar: ");
                scanf("%d", &key);
                BTreeNode *found = search(root, key);
                if (found)
                    printf("Valor %d encontrado na arvore.\n", key);
                else
                    printf("Valor %d NAO encontrado.\n", key);
                break;
            }

            case 4:
                apagarRegistro(&root, file, t);
                break;

            case 0:
                printf("Encerrando. Dados salvos em memoria.dat.\n");
                fclose(file);
                freeTree(root);
                return 0;

            default:
                printf("Opcao invalida. Tente novamente.\n");
                break;
        }
    }

    fclose(file);
    freeTree(root);
    return 0;
}
