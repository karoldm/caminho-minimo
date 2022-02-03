 #include <stdio.h>
#include <stdlib.h>
#include <string.h>

int Tempo; //variavel para marcar o tempo de descoberta/finalização de cada vértice
FILE *arqLog; //arquivo para gravar o log

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
void adicionarVertice(Grafo *grafo, int v); //adiciona um novo vertice ao grafo
void adicionarAdjacente(Grafo *grafo, int vertice, int v, int p); //adiciona um novo vertice a lista de vertices adjacentes de um vertice
void convertResult(int *resultInt, char *result); //converte a linha do arquivo txt em um array de argumentos inteiros (linha exemplo: 1 2 10)
void lerGrafo(Grafo *grafo); //leitura do grafo do arquivo txt e armazenamento em lista de adjacencia
void inicializa(int *d, Vertice* pi[], int verticeInicial, int numVertices); //incialização de d[] e pi[] para o algoritmo de dijkstra
Vertice* minimo(int *d, Vertice *vertices[], int numVertices); //encontra o vertice com menor valor de distancia
void relaxar(int* d, Vertice* pi[], int tam, Vertice* u, Vertice* v); //função para calcular menor caminho
void dijkstra(Grafo *grafo, int verticeInicial); //algoritmo de dijkstra para calcular o caminho minimo
void visita(Grafo* grafo, Vertice* u, char* cor, int* descoberta, int* finalizacao, PilhaVertice* K); //função recursiva para visitar todos os
                                                                                                      //vértices adjacentes de um vértice dado
PilhaVertice* ordenacaoTopologica(Grafo *grafo, int verticeInicial); //Algoritmo de busca profunda para fazer a ordenação topologica dos vértices
void caminhoMinimo_GAO(Grafo* grafo, int verticeInicial); //algoritmo para calcular o caminho minimo usando ordenação topologica

void logText(char str[]); //função para gravar string no arquivo de log
void LogImprimirCaminho(Grafo* grafo, Vertice* pi[]); //gravar no log caminho do vértice inicial até todos os outros
void logVetor(int vetor[], int tam); //função para gravar no log um vetor de inteiros (para gravar somente o vetor d por exemplo)
void logInt(int num); //gravar um numero inteiro no log
void logTabela(int d[], Vertice* pi[], int tam); //função para gravar no log a tabela composta por d e pi
void logVertices(Vertice* vertices[], int numVertices); //função para gravar no log um vetor de vértices
void logDadosOrdenacaoTopologica(char cor[], int descoberta[], int finalizacao[], int tam); //função para gravar no log dados da Ordenação Topológica
                                                                                            //(cor, tempo de descoberta, tempo de finalização)
/***************************************************************************************************************************************************/

void adicionarVertice(Grafo *grafo, int v){
    Vertice *novoVertice;
    novoVertice = malloc(sizeof(Vertice));

    if(novoVertice != NULL){

        novoVertice->v = v;
        novoVertice->p = 0; //o peso de um vértice até ele mesmo é zero
        novoVertice->prox = NULL;

        grafo->vertices[v] = novoVertice;
    }
    else {
        printf("\nERRO AO ALOCAR MEMÓRIA PARA VÉRTICE DO GRAFO");
    }
}

void adicionarAdjacente(Grafo *grafo, int vertice, int v, int p){
    Vertice *aux = grafo->vertices[vertice];
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
        while(!feof(file) && i < grafo->numVertices){

            /*sabemos que toda linha (após a primeira) do arquivo possui 3 informações:
            *número do vértico de origem (resultInt[0])
            *número do vértice de destino (resultInt[1])
            *custo/peso do vértice de origem ao de destino (resultInt[2])
            */


            result = fgets(linha, 100, file);

            if(result) {
                convertResult(resultInt, result);

                int verticeAtual = resultInt[0];

                adicionarVertice(grafo, verticeAtual);

                adicionarAdjacente(grafo, verticeAtual, resultInt[1], resultInt[2]);

                while(!feof(file)){
                    result = fgets(linha, 100, file);

                    //verricando se o vértice lido é adjacente ao vértice atual
                    if(result && atoi(&result[0]) == verticeAtual){

                        convertResult(resultInt, result);

                        //adicionando vértice a lista de vértices adjacentes do vértice atual
                        adicionarAdjacente(grafo, verticeAtual, resultInt[1], resultInt[2]);
                    }
                    else {
                        //como o próximo vértice já foi lido, voltamos uma linha
                        fseek(file, -strlen(result)-1, SEEK_CUR);
                        break;
                    }
                }

                i++; //indo para o próximo vértice do grafo
            }//if(result)
        } //while externo
    }
    else{
        printf("\nERRO AO ABRIR ARQUIVO");
        return;
    }

    fclose(file);
}

void inicializa(int d[], Vertice* pi[], int verticeInicial, int numVertices){

    logText("\n\nInicializando dados...");

    for(int i = 1; i < numVertices; i++){
        //valor próximo ao máximo para um int em C
        //Não utilizar o valor máximo, pois isso impede de realizar a soma da distância com o peso na etapa de relaxar as arestas
        d[i] =  2147000000;
        pi[i] = NULL;
    }

    d[verticeInicial] = 0;
}

Vertice* minimo(int d[], Vertice *vertices[], int numVertices){

/*********************** Log *****************************/
    logText("\n\nFunção Minimo");
    logText("\nd: ");
    logVetor(d, numVertices);
/*********************************************************/

    Vertice* verticeMinimo;
    int minimo;

    int i = 1;

    //procurando o primeiro vertice diferente de null dentro de Q
    while(vertices[i] == NULL) i++;

    minimo = d[i];
    verticeMinimo = vertices[i];
    i++;

    for(; i <= numVertices; i++){
        if(vertices[i] != NULL && d[i] < minimo){
            minimo = d[i];
            verticeMinimo = vertices[i];
        }
    }

/*********************** Log *****************************/
    logText("\nMinimo: ");
    logInt(verticeMinimo->v);
/*********************************************************/

    return verticeMinimo;
}

void relaxar(int* d, Vertice* pi[], int tam, Vertice* u, Vertice* v){

/*********************** Log *****************************/
    logText("\n\nFunção Relaxar: ");


    logText("\nu: ");
    logInt(u->v);

    logText("\nv: ");
    logInt(v->v);

    logText("\nw(u, v): ");
    logInt(v->p);

    logText("\n\nAntes da função relaxar: ");
    logTabela(d, pi, tam);
/*********************************************************/

    if(d[v->v] > d[u->v] + v->p){
        d[v->v] = (d[u->v] + v->p);
        pi[v->v] = u;
    }

/*********************** Log *****************************/
    logText("\n\nApos Função Relaxar: ");
    logTabela(d, pi, tam);
/*********************************************************/
}

void dijkstra(Grafo *grafo, int verticeInicial){
    int numVertices = grafo->numVertices;

    //lista de vertices ainda não visitados
    Vertice* Q[numVertices];
    int dimQ = numVertices-1;

    int d[numVertices];
    Vertice* pi[numVertices];
    Vertice* u;

    inicializa(d, pi, verticeInicial, numVertices);

    int i;
    for(i = 1; i < numVertices; i++) Q[i] = grafo->vertices[i];

    logTabela(d, pi, numVertices);
    logText("\nQ: ");
    logVertices(Q, numVertices);


    while(dimQ > 0){
        u = minimo(d, Q, numVertices);

        //marcar vertice como visitado
        dimQ--;
        Q[u->v] = NULL;

        logText("\nVertice atual: ");
        logInt(u->v);

        logText("\nQ: ");
        logVertices(Q, numVertices);

        Vertice* aux = u->prox;

        //percorrendo vertices adjacentes
        while(aux != NULL){

            logText("\n\nAdjacente: ");
            logInt(aux->v);

            relaxar(d, pi, numVertices, u, aux);
            aux = aux->prox;
        }
    }

    LogImprimirCaminho(grafo, pi);
}

void visita(Grafo* grafo, Vertice* u, char cor[], int descoberta[], int finalizacao[], PilhaVertice* K){

    cor[u->v] = 'c'; //vertice descoberto
    Tempo++;
    descoberta[u->v] = Tempo;

    logText("\n\nFunção Visita: ");
    logText("Visitando vértice ");
    logInt(u->v);
    logDadosOrdenacaoTopologica(cor, descoberta, finalizacao, grafo->numVertices);

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

    logText("\n\nFinalizando visita do vertice ");
    logInt(u->v);
    logDadosOrdenacaoTopologica(cor, descoberta, finalizacao, grafo->numVertices);

}

PilhaVertice* ordenacaoTopologica(Grafo *grafo, int verticeInicial){

    PilhaVertice *K = malloc(sizeof(PilhaVertice));

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
            descoberta[i] = -1;
            finalizacao[i] = -1;
        }

/*********************** Log *****************************/
    logText("\n\nFunção Ordenação Topológica: ");
    logText("\nDados iniciais: ");
    logDadosOrdenacaoTopologica(cor, descoberta, finalizacao, numVertices);
/*********************************************************/

        Tempo = 0;

        visita(grafo, grafo->vertices[verticeInicial], cor, descoberta, finalizacao, K);

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

void caminhoMinimo_GAO(Grafo* grafo, int verticeInicial){

    PilhaVertice* K = ordenacaoTopologica(grafo, verticeInicial);
    int numVertices = grafo->numVertices;

    int d[numVertices];
    Vertice* pi[numVertices];
    inicializa(d, pi, verticeInicial, numVertices);

    logTabela(d, pi, numVertices);
    logText("\nK (Ordenação Topológica): ");
    logVertices(K->vertices, numVertices);

    for(int i = 1; i < numVertices; i++){
        //percorrendo vértices da lista da ordenação topológica
        Vertice* u = K->vertices[i];
        Vertice* aux = u->prox;

        logText("\n\nVertice atual de K: ");
        logInt(u->v);

        //percorrendo vertices adjacentes
        while(aux != NULL){

            logText("\nAdjacente ");
            logInt(aux->v);

            relaxar(d, pi, numVertices, u, aux);
            aux = aux->prox;
        }

    }

    LogImprimirCaminho(grafo, pi);

}

void LogImprimirCaminho(Grafo* grafo, Vertice* pi[]){

    logText("\n\n");

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

            logText("\nCaminho do vértice ");
            logInt(primVertice->v);
            logText(" ao vértice ");
            logInt(destino->v);
            logText(": ");

            //traçando caminho
            int j = 0;
            Vertice* ant;
            ant = pi[destino->v];

            while(ant != primVertice){
                caminho[j] = ant;
                ant = pi[ant->v];
                j++;
            }

            logInt(primVertice->v);

            j--;
            while(j >= 0){
                logText(" -> ");
                logInt(caminho[j]->v);
                j--;
            }

            logText(" -> ");
            logInt(destino->v);
        }

        i++;
    }
}

void logDadosOrdenacaoTopologica(char cor[], int descoberta[], int finalizacao[], int tam){
    char ch[2];
    logText("\n\nCor: ");
    for(int i = 1; i < tam; i++){
        ch[0] = cor[i];
        ch[1] = '\0';
        logText(ch);
        logText(" | ");
    }
    logText("\nTempo de descoberta: ");
    logVetor(descoberta, tam);
    logText("\nTempo de finalização: ");
    logVetor(finalizacao, tam);
}

void logVetor(int vetor[], int tam){
    for(int i = 1; i < tam; i++){
        logInt(vetor[i]);
        logText(" | ");
    }
}

void logInt(int num){
    char result[50];
    itoa(num, result, 10);
    logText(result);
}

void logTabela(int d[], Vertice* pi[], int tam){

    logText("\nd: ");
    logVetor(d, tam);

    logText("\npi: ");
    logVertices(pi, tam);

}

void logVertices(Vertice* vertices[], int numVertices){
    char* result;
    for(int i = 1; i < numVertices; i++){
        if(vertices[i] != NULL){
            logInt(vertices[i]->v);
            logText(" | ");
        }
        else logText("NULL | ");
    }
}

void logText(char str[]){
    int result;
    puts("");

    arqLog = fopen("arqLog.txt", "at");

    if (arqLog != NULL){
        result = fprintf(arqLog, str);
        if (result == EOF) printf("\nERRO AO GRAVAR TEXTO NO ARQUIVO");
    }
    else {
        printf("\nERRO AO ABRIR O ARQUIVO");
    }

    fclose(arqLog);
}

int main(void){

    Grafo grafo;

    remove("arqLog.txt");

    logText("\nLendo grafo...");
    lerGrafo(&grafo);

    int vInicial;

    printf("\nDigite o vertice de origem: ");
    scanf("%d", &vInicial);

    logText("\n\nDijkstra: ");
    dijkstra(&grafo, vInicial);

    logText("\n\n\nCaminho Minimo GAO: ");
    caminhoMinimo_GAO(&grafo, vInicial);


    printf("\nProcessamento concluido, para ver o log do processamento e o resultado basta consultar o arquivo 'arqLog.txt'");
    printf("\n\n\n\n\n\n\n\n\n\n\n\n");


    system("PAUSE");
    return 0;
}
