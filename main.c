#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct VerticeAdj{
    int v; //inteiro que representa o vértice
    int d; //inteiro que representa a distância
    struct VerticeAdj *prox; //próximo vértices adjacente
};

struct Vertice{
    int v; //inteiro que representa o vértice
    struct VerticeAdj *primAdj; //primeiro vértice adjacente
    struct VerticeAdj *ultAdj; //último vértice adjacente (usado para inserir um vértice adjacente ao final da lista)
};

struct Grafo{
    int numVertices; //quantidade de vértices do grafo
    struct Vertice *vertices[]; //Array de vertices pertencentes ao grafo
};


/************************************************************ Protótipo das Funções ****************************************************************/
void adicionarAdjacente(struct Vertice *vertice, int v, int d); //adiciona um vertice ao final da lista de vértices adjacentes
void lerGrafo(struct Grafo *grafo);
/***************************************************************************************************************************************************/


void adicionarAdjacente(struct Vertice *vertice, int v, int d){
    struct VerticeAdj *verticeAdj;
    verticeAdj = (struct VerticeAdj*)malloc(sizeof(struct VerticeAdj));
    if(verticeAdj != NULL){
        verticeAdj->v = v;
        verticeAdj->d = d;
        verticeAdj->prox = NULL;

        if(vertice->primAdj == NULL){
            vertice->primAdj = verticeAdj;
        }
        else {
            vertice->ultAdj->prox = verticeAdj;
        }
        vertice->ultAdj = verticeAdj;
    }
    else {
        printf("\nERRO AO ALOCAR MEMÓRIA PARA VÉRTICE ADJACENTE");
    }
}

void lerGrafo(struct Grafo *grafo){
    char *result;
    char linha[100];
    FILE *file;

    file = fopen("grafo.txt", "rt");
    if(file != NULL){
        //lendo a primeira linha do arquivo que contém o número de vértices do grafo
        result = fgets(linha, 100, file);
        if(result){
            int numVertices = atoi(result);
            grafo->numVertices = numVertices;
        }

        int i = 0;
        //percorrendo arquivo
        while(!feof(file) && i < grafo->numVertices){

            /*sabemos que toda linha (após a primeira) do arquivo possui 3 informações:
            *número do vértico de origem (result[0])
            *número do vértice de destino result[1])
            *custo do vértice de origem ao de destino result[2])
            */

            result = fgets(linha, 100, file);

            if(result) {
                struct Vertice *vertice;
                vertice = (struct Vertice*)malloc(sizeof(struct Vertice));

                if(vertice != NULL){

                    vertice->v = atoi(&result[0]);
                    vertice->primAdj = NULL;
                    vertice->ultAdj = NULL;

                    *(grafo->vertices+i) = vertice;

                    adicionarAdjacente(vertice, atoi(&result[1]), atoi(&result[2]));

                    //enquanto estamos lendo vertices que sejam adjacentes ao vertice lido acima
                    while(!feof(file)){
                        result = fgets(linha, 100, file);

                        //verricando se o vértice lido é adjacente ao vértice atual
                        if(result && atoi(&result[0]) == vertice->v){
                            //adicionando vértice a lista de vértices adjacentes do vértice atual
                            adicionarAdjacente(vertice, atoi(&result[1]), atoi(&result[2]));
                        }
                        else {
                            //como o próximo vértice já foi lido, voltamos uma linha
                            fseek(file, -strlen(result)-1, SEEK_CUR);
                            break;
                        }
                    }

                    i++; //indo para o próximo vértice do grafo
                }
                else {
                    printf("\nERRO AO ALOCAR MEMÓRIA PARA VÉRTICE DO GRAFO");
                }
            }//if(result)
        } //while externo
    }
    else{
        printf("\nERRO AO ABRIR ARQUIVO");
        return;
    }
}

int main(void){
    struct Grafo grafo;
    lerGrafo(&grafo);

    //struct Vertice *vertice;
    //vertice = *(grafo.vertices+4);

    //printf("\nVertice: %d", vertice->v);

    //if(vertice->primAdj == NULL) printf("\nNULL");

    //printf("\nprimeiro adjacente: %d", vertice->primAdj->v);
    //printf("\nsegundo adjacente: %d", vertice->primAdj->prox->v);
    //printf("\nsegundo adjacente: %d", vertice->primAdj->prox->prox->v);
}
