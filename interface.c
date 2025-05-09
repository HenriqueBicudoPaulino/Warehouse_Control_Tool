#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "interface.h"
#include "item.h"
#include "fila_localizacoes.h"

void menu(Lista *lista, Arvore *arvore) {
    int opcao, part_number, quantidade;
    clock_t ini, fim;
    Item novo;
    FilaLocalizacoes fila_loc;

    cria_fila_localizacoes(&fila_loc);

    do {	
        printf("========================================\n");
		printf("                 MENU                   \n");
		printf("========================================\n");
		printf("1 - Exibir estoque\n");
		printf("2 - Buscar\n");
		printf("3 - Inserir\n");
		printf("4 - Utilizar\n");
		printf("5 - Descartar\n");
		printf("0 - Sair\n");
		printf("Selecione uma opcao: ");

        scanf("%d", &opcao);

		NoArvore *resultado;

        switch (opcao) {
        	case 1:
        		system("cls");
            	printf("========================================\n");
				printf("                ESTOQUE                 \n");
				printf("========================================\n");
        		
				printf("\nExibindo Estoque na Lista:\n");
                exibe_lista(lista);
                
                printf("\nExibindo Estoque na Arvore:\n");
                exibe_arvore(*arvore);
                
                printf("\nExibindo Estoque por Localizacao:\n");
                exibe_fila_localizacoes(&fila_loc);
        	break;

            case 2:
            	system("cls");
            	printf("========================================\n");
				printf("                BUSCAR                  \n");
				printf("========================================\n");
                printf("\nDigite o Part Number: ");
                scanf("%d", &part_number);

                ini = clock();
                NoLista *resultado_lista = busca_lista(lista, part_number);
                fim = clock();
                printf("\nTempo Lista: %.5lfs\n", (double)(fim - ini) / CLOCKS_PER_SEC);

                ini = clock();
                NoArvore *resultado_arvore = busca_arvore(arvore, part_number);
                fim = clock();
                printf("Tempo Arvore: %.5lfs\n\n", (double)(fim - ini) / CLOCKS_PER_SEC);
                
                if (resultado_arvore && !fila_vazia(&resultado_arvore->fila_lotes)) {
                    printf("Item encontrado:\n");
                    Item result_arvore = resultado_arvore->fila_lotes.inicio->dado;
                    imprime_item(result_arvore);
                } else {
                    printf("Item nao encontrado na lista ou fila vazia.\n");
                    printf("Item nao encontrado ou fila vazia.\n");
                }
            break;

            case 3:
            	system("cls");
            	printf("========================================\n");
				printf("               INSERIR                  \n");
				printf("========================================\n");
                printf("\nPart Number: "); scanf("%d", &novo.part_number); getchar();

                NoLista *existente_lista = busca_lista(lista, novo.part_number);
                NoArvore *existente_arvore = busca_arvore(arvore, novo.part_number);
                
                if (existente_lista && existente_arvore) {
                    strcpy(novo.descricao, existente_lista->fila_lotes.inicio->dado.descricao);
                    strcpy(novo.localizacao, existente_lista->fila_lotes.inicio->dado.localizacao);
                    printf("Descricao (preenchida automaticamente): %s\n", novo.descricao);
                    printf("Localizacao (preenchida automaticamente): %s\n", novo.localizacao);
                } else {
                    printf("Descricao: "); scanf("%s", novo.descricao);
                    do {
                        printf("Localizacao (Ex: AB1): "); scanf("%s", novo.localizacao);
                        if (!valida_localizacao(novo.localizacao)) {
                            printf("\nLocalizacao invalida. Tente novamente.\n");
                        }
                    } while (!valida_localizacao(novo.localizacao));
                }
                resultado = busca_arvore(arvore, novo.part_number);
				if(resultado && !fila_vazia(&resultado->fila_lotes)){
					novo.shelf_life = resultado->fila_lotes.inicio->dado.shelf_life;
				}else{
					printf("Shelf Life (meses): "); scanf("%d", &novo.shelf_life);
				}
                
                printf("Quantidade: "); scanf("%d", &novo.quantidade);
                
				novo.flag = 1; // liberado

				// Data de entrada como data atual
				time_t t = time(NULL);
			    struct tm *data_atual = localtime(&t);
			    snprintf(novo.data_entrada, sizeof(novo.data_entrada), "%02d/%02d/%04d",
             	data_atual->tm_mday, data_atual->tm_mon + 1, data_atual->tm_year + 1900);

                ini = clock();
                insere_ordenado_lista(lista, novo.part_number, novo);
                fim = clock();
                printf("\nInserido na Lista em %.5lfs\n", (double)(fim - ini) / CLOCKS_PER_SEC);

                ini = clock();
                insere_arvore(arvore, novo.part_number, novo);
                fim = clock();
                printf("Inserido na Arvore em %.5lfs\n", (double)(fim - ini) / CLOCKS_PER_SEC);
                
                // Inserir na fila de localizações
                insere_fila_localizacoes(&fila_loc, novo.localizacao, novo);

            break;

            case 4:
            	system("cls");
            	printf("========================================\n");
				printf("               UTILIZAR                 \n");
				printf("========================================\n");
				
                printf("\nDigite Part Number:");
                scanf("%d", &part_number);
                resultado = busca_arvore(arvore, part_number);
				if (resultado && resultado->fila_lotes.inicio) {
					Item result = resultado->fila_lotes.inicio->dado;
					printf("Esse item possui %d unidades.",result.quantidade);

					printf("\nDigite a Quantidade a ser utilizada: ");
	                scanf("%d", &quantidade);

	                ini = clock();
	                utiliza_item_lista(lista, part_number, quantidade);
	                fim = clock();
	                printf("\nUtilizado na Lista em %.5lfs\n", (double)(fim - ini) / CLOCKS_PER_SEC);

	                ini = clock();
	                utiliza_item_arvore(arvore, part_number, quantidade);
	                fim = clock();
	                printf("Utilizado na Arvore em %.5lfs\n", (double)(fim - ini) / CLOCKS_PER_SEC);

	                result = resultado->fila_lotes.inicio->dado;
	                printf("Nova quantidade: %d\n", result.quantidade);
            	}
            	else{
            		printf("Item nao encontrado ou fila vazia.\n");
				}
            break;

            case 5:
            	system("cls");
            	printf("========================================\n");
				printf("               DESCARTAR                \n");
				printf("========================================\n");
                printf("\nDigite o Part Number: ");
                scanf("%d", &part_number);
                resultado = busca_arvore(arvore, part_number);
				if (resultado && resultado->fila_lotes.inicio) {
	    			Item result = resultado->fila_lotes.inicio->dado;
					printf("Esse item possui %d unidades.",result.quantidade);
                    printf("\nDigite a quantidade a ser descartada: ");
                	scanf("%d", &quantidade);
                	ini = clock();
	                descarta_item_lista(lista, part_number, quantidade);
	                fim = clock();
	                printf("\nDescartado na Lista em %.5lfs\n", (double)(fim - ini) / CLOCKS_PER_SEC);

	                ini = clock();
	                descarta_item_arvore(arvore, part_number, quantidade);
	                fim = clock();
	                printf("Descartado na Arvore em %.5lfs\n", (double)(fim - ini) / CLOCKS_PER_SEC);

	                result = resultado->fila_lotes.inicio->dado;
	                printf("Nova quantidade: %d\n", result.quantidade);
				}
				else{
					printf("Item nao encontrado ou fila vazia.\n");
				}

            break;

            case 0:
                printf("\n\nEncerrando...\n\n");
            break;
        }
    } while (opcao != 0);
}
