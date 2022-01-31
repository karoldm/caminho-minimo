#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int Tempo; //variavel para marcar o tempo de descoberta/finalização de cada vértice

typedef struct vertice{
    int v; //inteiro que representa o vértice
    int p; //inteiro que representa o peso da aresta
    struct vertice *prox; //vértices adjacente
}Vertice;

typedef struct grafo{
    int numVertices; //quantidade de vértices do grafo
    Vertice *vertices[]; //Array de vertices pertencentes ao grafo
}Grafo;

typedef struct pilha{
    int indice;
    Vertice *vertices[];
}PilhaVertice;


/************************************************************ Protótipo das Funções ****************************************************************/
Vertice* adicionarVertice(Grafo *grafo, int v); //adiciona um novo vertice ao grafo
void adicionarAdjacente(Grafo *grafo, Vertice *vertice, int v, int p); //adiciona um novo vertice a lista de vertices adjacentes de um vertice
void convertResult(int *resultInt, char *result); //converte a linha do arquivo txt em um array de argumentos inteiros (linha exemplo: 1 2 10)
void lerGrafo(Grafo *grafo); //leitura do grafo do arquivo txt e armazenamento em lista de adjacencia
void inicializa(int *d, Vertice* pi[], int verticeInicial, int numVertices); //incialização de d[] e pi[] para o algoritmo de dijkstra
Vertice* minimo(int *d, Vertice *vertices[], int numVertices); //encontra o vertice com menor valor de distancia
void relaxar(int* d, Vertice* pi[], Vertice* u, Vertice* v); //função para calcular menor caminho
void dijkstra(Grafo *grafo, int verticeInicial); //algoritmo de dijkstra para calcular o caminho minimo
void imprimir(Grafo* grafo, Vertice* pi[]); //imprimir caminho do vértice inicial até todos os outros
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
            grafo->numVertices = numVertices+1;
        }

        int i = 1;
        int resultInt[3]; //para converter o resultado em int
        //percorrendo arquivo
        while(!feof(file) && i <= grafo->numVertices){

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

    for(int i = 1; i < numVertices; i++){
        //valor próximo ao máximo para um int em C
        //Não utilizar o valor máximo, pois isso impede de realizar a soma da distância com o peso na etapa de relaxar as arestas
        *(d+i) =  2147000000;
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

void relaxar(int* d, Vertice* pi[], Vertice* u, Vertice* v){
    if(d[v->v] > d[u->v] + v->p){
        d[v->v] = (d[u->v] + v->p);
        pi[v->v] = u;
    }
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
            relaxar(d, pi, u, aux);
            aux = aux->prox;
        }
    }

    printf("\nDijkstra: \n");
    imprimir(grafo, pi);
}

void visita(Grafo* grafo, Vertice* u, char* cor, int* descoberta, int* finalizacao, PilhaVertice* K){

    cor[u->v] = 'c'; //vertice descoberto
    Tempo++;
    descoberta[u->v] = Tempo;

    Vertice* aux = u->prox;


    //percorrendo vertices adjacentes
    while(aux != NULL){
        if(cor[aux->v] == 'b'){
            visita(grafo, grafo->vertices[aux->v], cor, descoberta, finalizacao, K);
        }
        aux = aux->prox;
    }

    cor[u->v] = 'p';
    Tempo++;
    finalizacao[u->v] = Tempo;

    K->vertices[K->indice] = u;
    K->indice++;
}

PilhaVertice* ordenacaoTopologica(Grafo *grafo){
    PilhaVertice *K = (PilhaVertice*)malloc(sizeof(PilhaVertice));
    if(K != NULL){
        K->indice = 1;
        int numVertices = grafo->numVertices;
        char cor[numVertices];   //array para armazenar a cor de cada vértice
        int descoberta[numVertices]; //array para armazenar o tempo de descoberta de cada vértice
        int finalizacao[numVertices]; //array para armazenar o tempo de finalização de cada vértice
        /*
        *b = branco
        *c = cinza
        *p = preto
        */
        Vertice* pi[numVertices]; //array para armazenar antecessor de cada vértice

        for(int i = 1; i < numVertices; i++){
            cor[i] = 'b';
        }

        Tempo = 0;

        for(int i = 1; i < numVertices; i++){
            if(cor[i] == 'b') visita(grafo, grafo->vertices[i], cor, descoberta, finalizacao, K);
        }

        int i = 1;
        int j = K->indice-1;
        while(i <= j){
            Vertice* aux = K->vertices[i];
            K->vertices[i] = K->vertices[j];
            K->vertices[j] = aux;
            i++; j--;
        }
    }

    return K;
}

void caminhoMinimo_GAO(Grafo* grafo){
    PilhaVertice* K = ordenacaoTopologica(grafo);
    int numVertices = grafo->numVertices;

    int d[numVertices];
    Vertice* pi[numVertices];
    inicializa(d, pi, K->vertices[1]->v, numVertices);

    for(int i = 1; i < numVertices; i++){
        //percorrendo vértices da lista da ordenação topológica
        Vertice* u = K->vertices[i];
        Vertice* aux = u->prox;

        //percorrendo vertices adjacentes
        while(aux != NULL){
            relaxar(d, pi, u, aux);
            aux = aux->prox;
        }

    }

    printf("\nCaminhoMinimo_GAO: \n");
    imprimir(grafo, pi);

}

void imprimir(Grafo* grafo, Vertice* pi[]){
    int indPrim = 1;
    Vertice* primPi;
    primPi = pi[indPrim];

    //procurando o primeiro vertice
    while(primPi != NULL){
        indPrim++;
        primPi = pi[indPrim];
    }

    Vertice* primVertice;
    primVertice = grafo->vertices[indPrim];

    int i = 1;
    while(i < grafo->numVertices){

        if(primVertice != grafo->vertices[i]){

            Vertice* destino = grafo->vertices[i];
            Vertice* caminho[grafo->numVertices];
            printf("\nCaminho do vertice %d ao %d: ", primVertice->v, destino->v);

            //traçando caminho
            int j = 0;
            Vertice* ant;
            ant = pi[destino->v];

            while(ant != primVertice){
                caminho[j] = ant;
                ant = pi[ant->v];
                j++;
            }

            printf("%d ", primVertice->v);

            j--;
            while(j >= 0){
                printf("-> %d ", caminho[j]->v);
                j--;
            }

            printf("-> %d", destino->v);
        }

        i++;
    }
}


int main(void){
    Grafo grafo;
    lerGrafo(&grafo);

    dijkstra(&grafo, 1);

    printf("\n\n\n");

    caminhoMinimo_GAO(&grafo);

    printf("\n\n\n");

    return 0;
}
