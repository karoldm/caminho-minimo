#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct vertice{
    int v; //inteiro que representa o vértice
    int p; //inteiro que representa o peso da aresta
    //int d; //distância para o algoritmo de dijkstra
    //struct vertice* pi; //precedente para o algoritmo de dijksra
    struct vertice *prox; //vértices adjacente
}Vertice;

typedef struct grafo{
    int numVertices; //quantidade de vértices do grafo
    Vertice *vertices[]; //Array de vertices pertencentes ao grafo
}Grafo;


/************************************************************ Protótipo das Funções ****************************************************************/
Vertice* adicionarVertice(Grafo *grafo, int v); //adiciona um novo vertice ao grafo
void adicionarAdjacente(Grafo *grafo, Vertice *vertice, int v, int p); //adiciona um novo vertice a lista de vertices adjacentes de um vertice
void convertResult(int *resultInt, char *result); //converte a linha do arquivo txt em um array de argumentos inteiros (linha exemplo: 1 2 10)
void lerGrafo(Grafo *grafo); //leitura do grafo do arquivo txt e armazenamento em lista de adjacencia
void inicializa(int *d, Vertice* pi[], int verticeInicial, int numVertices); //incialização de d[] e pi[] para o algoritmo de dijkstra
Vertice* minimo(int *d, Vertice *vertices[], int numVertices); //encontra o vertice com menor valor de distancia
void dijkstra(Grafo *grafo, int verticeInicial); //algoritmo de dijkstra para calcular o caminho minimo
/***************************************************************************************************************************************************/

Vertice* adicionarVertice(Grafo *grafo, int v){
    Vertice *vertice;
    vertice = (Vertice*)malloc(sizeof(Vertice));

    if(vertice != NULL){

        vertice->v = v;
        vertice->p = 0; //o peso de um vértice até ele mesmo é zero
        vertice->prox = NULL;

        grafo->vertices[v] = vertice;

        return vertice;
    }
    else {
        printf("\nERRO AO ALOCAR MEMÓRIA PARA VÉRTICE DO GRAFO");
    }
}

void adicionarAdjacente(Grafo *grafo, Vertice *vertice, int v, int p){
    Vertice *aux = grafo->vertices[vertice->v];
    Vertice *verticeAdj  = (Vertice*)malloc(sizeof(Vertice));

    if(verticeAdj != NULL){

        verticeAdj->v = v;
        verticeAdj->p = p;
        verticeAdj->prox = NULL;

        //inserir vértice no final da lista de vértices adjacentes
        while(aux->prox != NULL){
            aux = aux->prox;
        }
        aux->prox = verticeAdj;
    }
    else {
        printf("\nERRO AO ALOCAR MEMÓRIA PARA O VÉRTICE ADJACENTE");
    }
}

void convertResult(int *resultInt, char *result){
    int tam = strlen(result)-1;
    int j = 0;

    resultInt[j] = atoi(&result[0]);
    j++;

    //os argumentos são separados por espaço, logo quando encontramos um espaço na string temos o começo de um argumento
    for(int i = 0; i < tam; i++){
        if(result[i] == ' '){
            resultInt[j] = atoi(&result[i+1]);
            j++;
            i++;
        }
    }
}

void lerGrafo(Grafo *grafo){
    char *result; //para armazenar a string lida
    char linha[100]; //para ler cada linha do arquivo
    FILE *file;

    //brindo arquivo
    file = fopen("grafo.txt", "rt");

    if(file != NULL){

        //lendo a primeira linha do arquivo que contém o número de vértices do grafo
        result = fgets(linha, 100, file);
        if(result){
            int numVertices = atoi(result);
            grafo->numVertices = numVertices;
        }

        int i = 0;
        int resultInt[3]; //para converter o resultado em int
        //percorrendo arquivo
        while(!feof(file) && i < grafo->numVertices){

            /*sabemos que toda linha (após a primeira) do arquivo possui 3 informações:
            *número do vértico de origem (resultInt[0])
            *número do vértice de destino (resultInt[1])
            *custo/peso do vértice de origem ao de destino (resultInt[2])
            */

            result = fgets(linha, 100, file);

            if(result) {
                convertResult(resultInt, result);

                Vertice *vertice;
                vertice = (Vertice*)malloc(sizeof(Vertice));

                if(vertice != NULL){

                    vertice = adicionarVertice(grafo, resultInt[0]);

                    adicionarAdjacente(grafo, vertice, resultInt[1], resultInt[2]);

                    while(!feof(file)){
                        result = fgets(linha, 100, file);

                        //verricando se o vértice lido é adjacente ao vértice atual
                        if(result && atoi(&result[0]) == vertice->v){

                            convertResult(resultInt, result);

                            //adicionando vértice a lista de vértices adjacentes do vértice atual
                            adicionarAdjacente(grafo, vertice, resultInt[1], resultInt[2]);
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

    fclose(file);
}

void inicializa(int *d, Vertice* pi[], int verticeInicial, int numVertices){

    for(int i = 1; i <= numVertices; i++){
        *(d+i) = 2147483647; //valor máximo para um int em C
        pi[i] = NULL;
    }

    *(d+verticeInicial) = 0;
}

Vertice* minimo(int *d, Vertice *vertices[], int numVertices){
    Vertice* verticeMinimo;
    int minimo;

    int i = 1;

    //procurando o primeiro vertice diferente de null dentro de Q
    while(vertices[i] == NULL) i++;

    minimo = *(d+i);
    verticeMinimo = vertices[i];
    i++;

    for(; i <= numVertices; i++){
        if(vertices[i] != NULL && *(d+i) < minimo){
            minimo = *(d+i);
            verticeMinimo = vertices[i];
        }
    }

    return verticeMinimo;
}

void dijkstra(Grafo *grafo, int verticeInicial){
    int numVertices = grafo->numVertices;

    //lista de vertices ainda não visitados
    Vertice* Q[numVertices];
    int dimQ = numVertices;

    int d[numVertices];
    Vertice* pi[numVertices];
    Vertice* u;

    inicializa(d, pi, verticeInicial, numVertices);

    int i;
    for(i = 1; i <= numVertices; i++) *(Q+i) = grafo->vertices[i];

    while(dimQ > 0){
        u = minimo(d, Q, numVertices);
        dimQ--;
        //marcar vertice como visitado
        Q[u->v] = NULL;

        Vertice* aux = u->prox;

        //percorrendo vertices adjacentes
        while(aux != NULL){
            if(d[aux->v] > d[u->v] + aux->p){
                d[aux->v] = (d[u->v] + aux->p);
                pi[aux->v] = u;
            }
            aux = aux->prox;
        }
    }

printf("\n d: %d %d %d %d %d", d[1], d[2], d[3], d[4], d[5]);
printf("\n v: NULL %d %d %d %d", pi[2]->v, pi[3]->v, pi[4]->v, pi[5]->v);
}

int main(void){
    Grafo grafo;
    lerGrafo(&grafo);

    dijkstra(&grafo, 1);

    return 0;
}
