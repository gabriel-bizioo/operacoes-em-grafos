#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include "grafo.h"
#define BUFFER 2048

typedef struct aresta {

    int peso;
    void *vertice;
} aresta;

typedef struct vertice {

    char *nome;
    int n_fronteira;
    aresta **fronteira;

    // Para BFS, DFS
    int estado;
    struct vertice *pai;
    int nivel;
} vertice;

typedef struct grafo {

    char *nome;
    int n_arestas;
    int n_vertices;
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
    v->nivel = 0;

    return v;
}

int add_vertice(grafo *g, vertice *v) {

    int tam = g->n_vertices;
    if(tam && tam % 5 == 0) {
        if(!(g->lista_adjacencia = realloc(g->lista_adjacencia, sizeof(void *)*(g->n_vertices+5)))) {
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

    return e;
}

int add_aresta(vertice *v, aresta *e) {

    int tam = v->n_fronteira;
    if(tam && tam % 5 == 0) {
        if(!(v->fronteira = realloc(v->fronteira, v->n_fronteira+5))) {
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
    int nivel = 0;
    vertice **fila = malloc(sizeof(void*)*g->n_vertices);

    fila[0] = r;
    tam_fila++;

    r->estado = 1;
    while(tam_fila) {
        vertice *v = fila[com_fila];
        tam_fila--;
        com_fila++;
        if(com_fila+tam_fila < fim_fila) {
            //fprintf(stderr, "Vai crashar aqui\n");
            fim_fila++;
        }

        if(v == NULL) {
            fprintf(stderr, "Voce eh BETA\n");
            fprintf(stderr, "tam: %d comeco: %d fim: %d\n", tam_fila, com_fila, fim_fila);
        }
        fprintf(stderr, "%s \n", v->nome);

        int tam = v->n_fronteira;
        for(int i = 0; i < v->n_fronteira; ++i) {
            aresta *e = v->fronteira[i];
            vertice *w = e->vertice;

            if(w->estado == 1) {
                fprintf(stderr, " processe {%s, %s} como fora da arvore\n", v->nome, w->nome);

            }
            else if(w->estado == 0) {

                w->pai = v;
                fim_fila++;

                fila[fim_fila] = w;
                tam_fila++;
                fprintf(stderr, "Vai crashar depois daqui (%s -> %s)\n", w->pai->nome, w->nome);
                w->estado = 1;
            }
        }
        v->estado = 2;
        v->nivel = nivel;
        nivel++;
    }
    fprintf(stderr, "Saindo do busca_largura\n");
    free(fila);

    return 1;
}

int init_busca_largura(grafo *g) {
    int tam = g->n_vertices;
    for(int i = 0; i < tam; ++i)
        g->lista_adjacencia[i]->estado = 0;

    for(int i = 0; i < tam; ++i)
        if(!g->lista_adjacencia[i]->estado)
            busca_largura(g, g->lista_adjacencia[i]);

    return 1;
}

int busca_profundidade(grafo *g, vertice *r) {
    r->estado = 1;
    
    for(int i = 0; i < r->n_fronteira; ++i) {
        vertice *v = r->fronteira[i]->vertice;
        if(v->estado == 1 && v != r->pai)
            fprintf(stderr, "Processe {r,v}\n");
        else if(v->estado == 0) {
            v->pai = r;
            // processe {r,v}
            busca_profundidade(g, v);
        }
    }
    
    return 0;
}

int init_busca_profundidade(grafo *g) {
    int tam = g->n_vertices;
    for(int i = 0; i < tam; ++i)
        g->lista_adjacencia[i]->estado = 0;

    for(int i = 0; i < tam; ++i)
        if(!g->lista_adjacencia[i]->estado)
            busca_profundidade(g, g->lista_adjacencia[i]);

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
                fprintf(stderr, "Criando vertice com nome \"%s\"\n", p1);
                u = cria_vertice(p1);
                fprintf(stderr, "Adicionando vertice com nome \"%s\"\n", u->nome);
                add_vertice(g, u);
            }

            p2[strcspn(p2, "\n")] = '\0';
            if(!(v = busca_vertice(g, p2))) {
                fprintf(stderr, "criando vertice com nome \"%s\"\n", p2);
                v = cria_vertice(p2);
                fprintf(stderr, "Adicionando vertice com nome \"%s\"\n", v->nome);
                add_vertice(g, v);
            }
            if(!u || !v)
                return NULL;

            int peso = INT_MAX;
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

unsigned int bipartido(grafo *g) {
    init_busca_largura(g);

    int tam = g->n_vertices;
    for(int i = 0; i < tam; ++i) {
        if(g->lista_adjacencia[i]->pai)
            fprintf(stderr, "vertice %s filho de %s\n", g->lista_adjacencia[i]->nome, g->lista_adjacencia[i]->pai->nome);
        else
            fprintf(stderr, "vertice %s eh raiz\n", g->lista_adjacencia[i]->nome);
    }
    return 0;
}

unsigned int n_vertices(grafo *g) {

    return g->n_vertices;
}

unsigned int n_arestas(grafo *g) {

    return g->n_arestas;
}


unsigned int n_componentes(grafo *g) {
    return 0;
}

char *diametros(grafo *g) {
    init_busca_profundidade(g);

    int tam = g->n_vertices;
    for(int i = 0; i < tam; ++i) {
        if(g->lista_adjacencia[i]->pai)
            fprintf(stderr, "vertice %s filho de %s\n", g->lista_adjacencia[i]->nome, g->lista_adjacencia[i]->pai->nome);
        else
            fprintf(stderr, "vertice %s eh raiz\n", g->lista_adjacencia[i]->nome);
    }
    return 0;
}

char *vertices_corte(grafo *g) {
    return 0;
}

char *arestas_corte(grafo *g) {
    return NULL;
}
