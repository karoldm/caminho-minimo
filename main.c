#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct VerticeAdj{
    int v; //inteiro que representa o v�rtice
    int d; //inteiro que representa a dist�ncia
    struct VerticeAdj *prox; //pr�ximo v�rtices adjacente
};

struct Vertice{
    int v; //inteiro que representa o v�rtice
    struct VerticeAdj *primAdj; //primeiro v�rtice adjacente
    struct VerticeAdj *ultAdj; //�ltimo v�rtice adjacente (usado para inserir um v�rtice adjacente ao final da lista)
};

struct Grafo{
    int numVertices; //quantidade de v�rtices do grafo
    struct Vertice *vertices[]; //Array de vertices pertencentes ao grafo
};


/************************************************************ Prot�tipo das Fun��es ****************************************************************/
void adicionarAdjacente(struct Vertice *vertice, int v, int d); //adiciona um vertice ao final da lista de v�rtices adjacentes
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
        printf("\nERRO AO ALOCAR MEM�RIA PARA V�RTICE ADJACENTE");
    }
}

void lerGrafo(struct Grafo *grafo){
    char *result;
    char linha[100];
    FILE *file;

    file = fopen("grafo.txt", "rt");
    if(file != NULL){
        //lendo a primeira linha do arquivo que cont�m o n�mero de v�rtices do grafo
        result = fgets(linha, 100, file);
        if(result){
            int numVertices = atoi(result);
            grafo->numVertices = numVertices;
        }

        int i = 0;
        //percorrendo arquivo
        while(!feof(file) && i < grafo->numVertices){

            /*sabemos que toda linha (ap�s a primeira) do arquivo possui 3 informa��es:
            *n�mero do v�rtico de origem (result[0])
            *n�mero do v�rtice de destino result[1])
            *custo do v�rtice de origem ao de destino result[2])
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

                        //verricando se o v�rtice lido � adjacente ao v�rtice atual
                        if(result && atoi(&result[0]) == vertice->v){
                            //adicionando v�rtice a lista de v�rtices adjacentes do v�rtice atual
                            adicionarAdjacente(vertice, atoi(&result[1]), atoi(&result[2]));
                        }
                        else {
                            //como o pr�ximo v�rtice j� foi lido, voltamos uma linha
                            fseek(file, -strlen(result)-1, SEEK_CUR);
                            break;
                        }
                    }

                    i++; //indo para o pr�ximo v�rtice do grafo
                }
                else {
                    printf("\nERRO AO ALOCAR MEM�RIA PARA V�RTICE DO GRAFO");
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
