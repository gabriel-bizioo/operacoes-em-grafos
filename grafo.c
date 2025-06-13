#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include "grafo.h"
#define BUFFER 2048

typedef struct aresta {

    int peso;
    void *vertice;
    struct aresta *prox;
} aresta;

typedef struct vertice {

    char *nome;
    int n_fronteira;
    aresta *fronteira;

    // Para BFS, DFS
    int estado;
    int pai;
    int nivel;
} vertice;


typedef struct grafo {

    char *nome;
    int n_arestas;
    int n_vertices;
    vertice *lista_adjacencia;
} grafo;

vertice *cria_vertice(char *nome) {

    vertice *v;
    if(!(v = malloc(sizeof(vertice))))
        return NULL;
    if(!(v->nome = malloc(BUFFER)))
        return NULL;

    strncpy(v->nome, nome, BUFFER);
    v->n_fronteira = 0;
    v->estado = 0;
    v->pai = 0;
    v->nivel = 0;

    return v;
}

aresta *cria_aresta(vertice *v, int peso) {

    aresta *e;
    if(!(e = malloc(sizeof(aresta))))
        return NULL;

    e->vertice = v;
    e->peso = peso;
    e->prox = NULL;

    return e;
}

void add_aresta(vertice *v, aresta *e) {

    aresta *iter = v->fronteira;
    if(iter == NULL) {
        v->fronteira = e;
        return;
    }

    while(iter->prox != NULL)
        iter = v->fronteira->prox;
    iter->prox = e;
    v->n_fronteira++;
}

int conecta(vertice *u, vertice *v, int peso) {

    aresta *e1, *e2; // Note que ambos sao a mesma aresta
    if(!(e1 = cria_aresta(v, peso)) || !(e2 = cria_aresta(u, peso)))
        return 0;

    add_aresta(u, e1);
    add_aresta(v, e2);

    return 1;
}

grafo *le_grafo(FILE *f) {

    int flag_nome;
    int arestas;
    int vertices;
    char *sep;
    char buffer[BUFFER];
    char *nome;
    grafo *g;

    flag_nome = 1;
    arestas = 0;
    vertices = 0;
    g = malloc(sizeof(grafo));
    nome = malloc(BUFFER);
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

            vertice *u = cria_vertice(p1);
            vertice *v = cria_vertice(p2);
            if(!u || !v) {
                fprintf(stderr, "'le_grafo' ERRO: falha na criacao de vertices\n");
                return NULL;
            }

            int peso = INT_MAX;
            if(p3 && p3[0] != ' ')
                peso = atoi(p3);

            if(!conecta(u, v, peso)) {
                fprintf(stderr, "'le_grafo' ERRO: falha ao criar arestas e conectar %s e %s\n",
                        u->nome, v->nome);
                return NULL;
            }
            add_vertice(u, g);
            add_vertice(v, g);
        }
        else if(flag_nome) {
            vertice *v;
            if(!(v = cria_vertice(buffer)))
                return NULL;
            vertices++;
            flag_nome = 0;
        }
        else
            printf("%s", buffer);
    }

    printf("%s", nome);
    g->nome = nome;
    return g;
}

unsigned int destroi_grafo(grafo *g) {

    if(!g || !g->nome || !g->lista_adjacencia) {
        fprintf(stderr, "'destroi_grafo' ERRO: free em grafo mal inicializado\n");
        return 0;
    }
    free(g->nome);

    for(int i = 0; i < g->n_vertices; ++i) {
        vertice *v = &(g->lista_adjacencia[i]);
        if(!v) {
            fprintf(stderr, "'destroi_grafo' ERRO: free em vertice nulo\n");
            return 0;
        }

        if(v->fronteira)
            free(v->fronteira);
        free(v);
    }

    free(g->lista_adjacencia);
    free(g);
    return 1;
}

char *nome(grafo *g) {
    return g->nome;
}

unsigned int bipartido(grafo *g) {

    return 0;
}

unsigned int n_vertices(grafo *g) {

    return 0;
}

unsigned int n_arestas(grafo *g) {

    return 0;
}


unsigned int n_componentes(grafo *g) {
    return 0;
}

char *diametros(grafo *g) {
    return 0;
}

char *vertices_corte(grafo *g) {
    return 0;
}

char *arestas_corte(grafo *g) {
    return NULL;
}
