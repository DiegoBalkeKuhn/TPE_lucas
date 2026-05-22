#include "B-tree.h"


int main() {
    FILE *file = fopen("memoria.dat", "rb+");
    if (!file) {
        perror("Error opening file");
        return 1;
    }
    TreeR root;

    while (1) {
        printf("O que gostaria de fazer?");
        printf("1 - Registrar");
        printf("2 - Ler Registros");
        printf("3 - Apagar Registros");
        printf("0 - Sair");
        int r = 0;
        scanf("%d", &r);

        switch (r) {
            case 1 : ;
                break;

            case 2 :  ;
                break;

            case 3 :  ;
                break;

            case 0 :  fclose(file);
                return 0;

            default : printf("opção invalida");



        }

    }


    fclose(file);
    return 0;
}
