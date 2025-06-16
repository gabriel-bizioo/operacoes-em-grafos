#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include "grafo.h"
#define BUFFER 2048

typedef struct aresta {

    int peso;
    int retorno;
    void *vertice;
} aresta;

typedef struct vertice {

    char *nome;
    int n_fronteira;
    aresta **fronteira;

    // Para BFS, DFS
    int estado;
    struct vertice *pai;
    int distancia;
} vertice;

typedef struct grafo {

    char *nome;
    int n_arestas;
    int n_vertices;
    int bipartido;
    vertice **lista_adjacencia;
} grafo;


vertice *cria_vertice(char *nome) {

    vertice *v;
    if(!(v = malloc(sizeof(vertice))))
        return NULL;
    if(!(v->nome = malloc(BUFFER)))
        return NULL;
    // Realoca memoria de 5 em 5 para dimiunir operacoes
    if(!(v->fronteira = malloc(5*sizeof(aresta))))
        return NULL;

    strncpy(v->nome, nome, BUFFER);
    v->n_fronteira = 0;
    v->estado = 0;
    v->pai = 0;
    v->distancia = 0;

    return v;
}

int add_vertice(grafo *g, vertice *v) {

    int tam = g->n_vertices;
    if(tam && tam % 5 == 0) {
        if(!(g->lista_adjacencia = realloc(g->lista_adjacencia, sizeof(void *)*(size_t)(g->n_vertices+5)))) {
            fprintf(stderr, "'add_vertice' ERRO: falha ao realocar lista de adjacencia");
            return 0;
        }
    }

    g->lista_adjacencia[tam] = v;
    g->n_vertices++;
    return 1;
}

vertice *busca_vertice(grafo *g, char *nome) {
    int tam = g->n_vertices;

    for(int i = 0; i < tam; ++i) {
        if(!strcmp(g->lista_adjacencia[i]->nome, nome))
            return g->lista_adjacencia[i];
    }

    return NULL;
}

aresta *cria_aresta(vertice *v, int peso) {

    aresta *e;
    if(!(e = malloc(sizeof(aresta))))
        return NULL;

    e->vertice = v;
    e->peso = peso;
    e->retorno = 0;

    return e;
}

int add_aresta(vertice *v, aresta *e) {

    int tam = v->n_fronteira;
    if(tam && tam % 5 == 0) {
        if(!(v->fronteira = realloc(v->fronteira, sizeof(aresta*)*(size_t)(v->n_fronteira+5)))) {
            fprintf(stderr, "'add_aresta' ERRO: falha ao realocar fronteira");
            return 0;
        }
    }

    int i_aresta = v->n_fronteira;
    v->fronteira[i_aresta] = e;
    v->n_fronteira++;

    return 1;
}

int conecta(vertice *u, vertice *v, int peso) {
    aresta *e1, *e2;
    if(!(e1 = cria_aresta(v, peso)) || !(e2 = cria_aresta(u, peso)))
        return 0;

    add_aresta(u, e1);
    add_aresta(v, e2);

    return 1;
}

int busca_largura(grafo *g, vertice *r) {
    int tam_fila = 0;
    int com_fila = 0;
    int fim_fila = 0;
    vertice **fila = malloc(sizeof(void*)*(size_t)g->n_vertices);


    r->distancia = 0;
    fila[0] = r;
    tam_fila++;

    r->estado = 1;
    while(tam_fila) {
        vertice *v = fila[com_fila];
        tam_fila--;
        com_fila++;
        if(com_fila + tam_fila < fim_fila)
            fim_fila++;

        for(int i = 0; i < v->n_fronteira; ++i) {

            aresta *e = v->fronteira[i];
            vertice *w = e->vertice;

            if(w->estado == 1) 
                // Aqui poderia verificar se o grafo nao eh bipartido
                // (ver 'bipartido')
                continue;
            else if(w->estado == 0) {

                w->pai = v;
                w->distancia = v->distancia+1;

                fim_fila++;
                fila[fim_fila] = w;
                tam_fila++;

                w->estado = 1;
            }
        }
        v->estado = 2;
    }
    free(fila);

    return 1;
}

int init_busca_largura(grafo *g) {
    int tam = g->n_vertices;
    for(int i = 0; i < tam; ++i) {
        g->lista_adjacencia[i]->estado = 0;
        g->lista_adjacencia[i]->distancia = 0;
    }

    for(int i = 0; i < tam; ++i)
        if(!g->lista_adjacencia[i]->estado)
            busca_largura(g, g->lista_adjacencia[i]);

    return 1;
}

int low_point(vertice *v) {

    int min = v->distancia;
    vertice *w;
    for(int i = 0; i < v->n_fronteira; ++i) {
        w = v->fronteira[i]->vertice;
        if(v->fronteira[i]->retorno) {

            if(w->distancia < min)
                min = w->distancia;
        }
        else if(w->pai == v && w != v->pai){

            int lowp = low_point(w);
            if(lowp < min)
                min = lowp;
        }
    }

    return min;
}

int busca_profundidade(grafo *g, vertice *r) {
    r->estado = 1;

    for(int i = 0; i < r->n_fronteira; ++i) {
        vertice *v = r->fronteira[i]->vertice;
        if(v->estado == 1 && v != r->pai)
            r->fronteira[i]->retorno = 1;
        else if(v->estado == 0) {
            v->pai = r;
            v->distancia = r->distancia + 1;
            busca_profundidade(g, v);
        }
    }

    return 0;
}

int init_busca_profundidade(grafo *g) {
    int tam = g->n_vertices;

    for(int i = 0; i < tam; ++i) {
        g->lista_adjacencia[i]->estado = 0;
        g->lista_adjacencia[i]->distancia = 0;
    }


    for(int i = 0; i < tam; ++i) {
        if(!g->lista_adjacencia[i]->estado)
            busca_profundidade(g, g->lista_adjacencia[i]);
    }

    return 1;
}
int compara_vertice_distancia(const void *a, const void *b) {
    const vertice * const *va = (const vertice *const *)a;
    const vertice * const *vb = (const vertice *const *)b;
    return (*va)->distancia - (*vb)->distancia;
}

int dijkstra(grafo *g, vertice *r) {

    int tam_fila = 0;
    vertice **fila = malloc(sizeof(void*)*(size_t)g->n_vertices);

    fila[0] = r;
    tam_fila++;

    r->estado = 1;
    while(tam_fila > 0) {

        // Muito ineficiente, nao deu tempo de fazer uma heap
        qsort(fila, (size_t)tam_fila, sizeof(vertice*), compara_vertice_distancia);

        vertice *v = fila[0];
        for(int i = 0; i < tam_fila-1; ++i)
            fila[i] = fila[i+1];
        tam_fila--;

        for(int i = 0; i < v->n_fronteira; ++i) {

            aresta *e = v->fronteira[i];
            vertice *w = e->vertice;

            if(w->estado == 1) {
                if(v->distancia + e->peso < w->distancia) {
                    w->pai = v;
                    w->distancia = v->distancia + e->peso;

                }
            }
            else if(w->estado == 0) {
                w->pai = v;
                w->distancia = v->distancia + e->peso;

                fila[tam_fila] = w;
                tam_fila++;

                w->estado = 1;
            }
        }
        v->estado = 2;
    }

    int maior_distancia = INT_MIN;
    for(int i = 0; i < g->n_vertices; ++i) {
        if(g->lista_adjacencia[i]->estado == 2 && g->lista_adjacencia[i]->distancia > maior_distancia)
            maior_distancia = g->lista_adjacencia[i]->distancia;
    }
    free(fila);

    if(maior_distancia == INT_MIN)
        maior_distancia = 0;

    return maior_distancia;
}

int init_dijkstra(grafo *g) {
    int tam = g->n_vertices;

    for(int i = 0; i < tam; ++i)
        g->lista_adjacencia[i]->estado = 0;

    for(int i = 0; i < tam; ++i)
        if(!g->lista_adjacencia[i]->estado)
            dijkstra(g, g->lista_adjacencia[i]);


    return 1;
}

grafo *le_grafo(FILE *f) {
    int flag_nome;
    char *sep;
    char buffer[BUFFER];
    grafo *g;

    if(!(g = malloc(sizeof(grafo)))) {
        fprintf(stderr, "'le_grafo' ERRO: falha ao alocar grafo\n");
        return NULL;
    }
    if(!(g->nome = malloc(BUFFER))) {
        fprintf(stderr, "'le_grafo' ERRO: falha ao alocar string\n");
        return NULL;
    }
    if(!(g->lista_adjacencia = malloc(5*sizeof(vertice)))){
        fprintf(stderr, "'le_grafo' ERRO: falha ao alocar lista de adjacencia\n");
        return NULL;
    }

    flag_nome = 1;
    g->n_arestas = g->n_vertices = 0;
    while(fgets(buffer, BUFFER, f) != NULL) {
        if(buffer[0] == '/' && buffer[1] == '/')
            continue;
        if(buffer[0] == '\n')
            continue;
        if((sep = strstr(buffer, " -- ")) != NULL) {
            *sep = '\0';

            char *p1 = buffer;
            char *p2 = sep+4;
            strtok(p2, " "); char *p3 = strtok(NULL, " ");

            vertice *u, *v;
            if(!(u = busca_vertice(g, p1))) {
                u = cria_vertice(p1);
                add_vertice(g, u);
            }

            p2[strcspn(p2, "\n")] = '\0';
            if(!(v = busca_vertice(g, p2))) {
                v = cria_vertice(p2);
                add_vertice(g, v);
            }
            if(!u || !v)
                return NULL;

            int peso = 1;
            if(p3 && p3[0] != ' ')
                peso = atoi(p3);

            if(!conecta(u, v, peso)) {
                fprintf(stderr, "'le_grafo' ERRO: falha ao criar arestas e conectar %s e %s\n",
                        u->nome, v->nome);
                return NULL;
            }
            g->n_arestas++;
        }
        else if(flag_nome) {
            buffer[strcspn(buffer, "\n")] = '\0';
            strncpy(g->nome, buffer, BUFFER);
            flag_nome = 0;
        }
        else {
            vertice *v;
            buffer[strcspn(buffer, "\n")] = '\0';
            if(!(v = cria_vertice(buffer))) {
                fprintf(stderr, "'le_grafo' ERRO: falha ao criar vertice isolado\n");
                return NULL;
            }
            add_vertice(g, v);
        }
    }

    return g;
}

unsigned int destroi_grafo(grafo *g) {

    if(!g || !g->nome || !g->lista_adjacencia) {
        fprintf(stderr, "'destroi_grafo' ERRO: free em grafo mal inicializado\n");
        return 0;
    }
    free(g->nome);

    for(int i = 0; i < g->n_vertices; ++i) {
        vertice *v = g->lista_adjacencia[i];
        if(!v) {
            fprintf(stderr, "'destroi_grafo' ERRO: free em vertice nulo\n");
            return 0;
        }

        int fronteira = v->n_fronteira;
        for(int j = 0; j < fronteira; ++j)
            free(v->fronteira[j]);
        if(v->fronteira)
            free(v->fronteira);
        free(v->nome);
        free(v);
    }

    free(g->lista_adjacencia);
    free(g);
    return 1;
}

char *nome(grafo *g) {

    return g->nome;
}

/*
 * Testa se existem arestas que conectam vertices no mesmo nivel (distancia de r).
 * Pelo funcionamento do busca em largura, isso nao ocorre em grafos bipartidos.
 * O teste poderia ter sido feito ja na busca em largura, mas preferi deixar a
 * verificacao mais explicita na funcao.
 */
unsigned int bipartido(grafo *g) {
    init_busca_largura(g);

    int tam = g->n_vertices;
    for(int i = 0; i < tam; ++i) {

        int fronteira = g->lista_adjacencia[i]->n_fronteira;
        for(int j = 0; j < fronteira; ++j) {

            vertice *u = g->lista_adjacencia[i];
            vertice *v = g->lista_adjacencia[i]->fronteira[j]->vertice;

            if(u->distancia == v->distancia)
                return 0;
        }
    }

    return 1;
}

unsigned int n_vertices(grafo *g) {
    if(g->n_vertices < 0) {
        fprintf(stderr, "'n_vertices' ERRO: numero de vertices menor que 0\n");
        return 0;
    }

    return (unsigned int)g->n_vertices;
}

unsigned int n_arestas(grafo *g) {
    if(g->n_arestas < 0) {
        fprintf(stderr, "'n_arestas' ERRO: numero de arestas menor que 0\n");
        return 0;
    }

    return (unsigned int)g->n_arestas;
}


unsigned int n_componentes(grafo *g) {
    int comp = 0;

    for(int i = 0; i < g->n_vertices; ++i)
        g->lista_adjacencia[i]->estado = 0;

    for(int i = 0; i < g->n_vertices; ++i) {
        if(!g->lista_adjacencia[i]->estado) {
            busca_largura(g, g->lista_adjacencia[i]);
            comp++;
        }
    }

    if(comp < 0) {
        fprintf(stderr, "'n_componentes' ERRO: numero de componentes menor que 0\n");
        return 0;
    }

    return (unsigned int)comp;
}

int compara_int(const void *a, const void *b) {
    return (*(const int*)a - *(const int*)b);
}

/*
 * Identifica um componente  atraves de busca em largura,
 * roda dijkstra para todos os vertices do componente,
 * e faz o mesmo para os demais componentes
 */
char *diametros(grafo *g) {
    int *diam = calloc((size_t)g->n_vertices+1, sizeof(int));
    int *visitado = calloc((size_t)g->n_vertices, sizeof(int));
    int ncomp = 0;

    char *resposta = calloc(6*(size_t)g->n_vertices, sizeof(char));

    for(int i = 0; i < g->n_vertices; ++i) {
         // Caso seja um novo componente
        if(!visitado[i]) {
            int max_diametro = 0;

            for(int k = 0; k < g->n_vertices; ++k) {
                g->lista_adjacencia[k]->estado = 0;
                g->lista_adjacencia[k]->distancia = 0;
                g->lista_adjacencia[k]->pai = NULL;
            }

            // Marca todos os vertices do componente
            busca_largura(g, g->lista_adjacencia[i]);

            for(int j = 0; j < g->n_vertices; ++j) {
                if(g->lista_adjacencia[j]->estado == 2) {
                    visitado[j] = 1;

                    // Perde a informacao de qual vertice eh do componente
                    for(int k = 0; k < g->n_vertices; ++k) {
                        g->lista_adjacencia[k]->estado = 0;
                        g->lista_adjacencia[k]->distancia = 0;
                        g->lista_adjacencia[k]->pai = NULL;
                    }

                    // Roda dijkstra deste vertice (recupera a informacao de qual vertice eh do componente)
                    int dist_max = dijkstra(g, g->lista_adjacencia[j]);
                    if(dist_max > max_diametro)
                        max_diametro = dist_max;
                }
            }

            diam[ncomp] = max_diametro;
            ncomp++;
        }
    }

    if(ncomp < 0) {
        fprintf(stderr, "'diametros' ERRO: falha ao processar numero de componentes\n");
    }
    qsort(diam, (size_t)ncomp, sizeof(int), compara_int);

    int fim_string = 0;
    char *buffer = malloc(BUFFER);
    for(int i = 0; i < ncomp; ++i) {
        fim_string += sprintf(resposta + fim_string, "%d ", diam[i]);
    }

    if(fim_string > 0)
        resposta[fim_string-1] = '\0';

    free(buffer);
    free(diam);
    free(visitado);

    return resposta;
}

int compara_string(const void *a, const void *b) {
    return strcmp(*(const char *const *)a, *(const char *const *)b);
}


/*
 * Dada a string de entrada, ordena alfabeticamente
 * os valores da string separados por
 * "sep", de n em n onde n = ntokens_item
 */
void ordena_tokens(char *str, int ntokens_item, const char *sep) {
    if (!str || strlen(str) == 0)
        return;
    if(ntokens_item > 1000) {
        fprintf(stderr, "'ordena_tokens' ERRO: muitos tokens por ordenacao");
        return;
    }

    char **items = malloc(BUFFER);
    int count = 0;

    char *token = strtok(str, sep);
    while (token && count < BUFFER) {
        char *tokens_temp[ntokens_item];
        size_t tamanho_item = 0;
        int coletados = 0;

        for (int i = 0; i < ntokens_item && token; i++) {
            tokens_temp[i] = token;
            tamanho_item += strlen(token) + 1;
            coletados++;
            token = strtok(NULL, sep);
        }

        if (coletados == ntokens_item) {
            items[count] = malloc((size_t)tamanho_item + 1);
            int pos = 0;
            for (int i = 0; i < ntokens_item; i++) {
                pos += sprintf(items[count] + pos, "%s", tokens_temp[i]);
                if (i < ntokens_item - 1)
                    pos += sprintf(items[count] + pos, sep);
            }
            count++;
        }
    }

    if (count == 0)
        return;

    if(count < 0) {
        fprintf(stderr, "'ordena_tokens' ERRO: falha ao obter numero de tokens");
        return;
    }

    qsort(items, (size_t)count, sizeof(char*), compara_string);

    char *temp = malloc(strlen(str) + (size_t)count * 10);
    int pos = 0;
    for (int i = 0; i < count; i++) {
        pos += sprintf(temp + pos, "%s ", items[i]);
        free(items[i]);
    }

    strcpy(str, temp);
    free(temp);
    free(items);

    if (pos > 0)
        str[pos - 1] = '\0';
}

char *vertices_corte(grafo *g) {
    init_busca_profundidade(g);
    char *resposta = calloc(BUFFER, sizeof(char));
    int fim_string, add;

    int n = g->n_vertices;
    vertice *v, *w;
    fim_string = add = 0;
    for(int i = 0; i < n; ++i) {
        v = g->lista_adjacencia[i];

        if(v->distancia == 0) {
            int children_count = 0;
            for(int j = 0; j < v->n_fronteira; ++j) {
                w = v->fronteira[j]->vertice;
                if(w->pai == v)
                    children_count++;
            }

            if(children_count > 1)
                    fim_string += sprintf(resposta+fim_string, "%s ", v->nome);
        }
        else {
            int nfronteira = v->n_fronteira;
            int nivel = v->distancia;
            for(int j = 0; j < nfronteira; ++j) {
                w = v->fronteira[j]->vertice;
                if(w->pai == v && nivel <= low_point(w)) {
                    fim_string += sprintf(resposta+fim_string, "%s ", v->nome);
                }
            }
        }
    }

    ordena_tokens(resposta, 1, " ");
    return resposta;
}

char *arestas_corte(grafo *g) {
    init_busca_profundidade(g);
    char *resposta = calloc((size_t)g->n_arestas*BUFFER, sizeof(char));
    char buffer[BUFFER];
    int fim_string, add;

    int n = g->n_vertices;
    vertice *u, *v;
    fim_string = add = 0;
    for(int i = 0; i < n; ++i) {
        u = g->lista_adjacencia[i];
        int nfronteira = u->n_fronteira;

        for(int j = 0; j < nfronteira; ++j) {
            v = u->fronteira[j]->vertice;
            if(u == v->pai) {
                if(u->distancia < low_point(v)) {
                    sprintf(buffer, "%s %s ", u->nome, v->nome);
                    ordena_tokens(buffer, 1, " ");
                    fim_string += sprintf(resposta+fim_string, "%s ", buffer);
                }
            }
        }
    }

    ordena_tokens(resposta, 2, " ");
    return resposta;
}
