#include <assert.h>
#include <math.h>                  // Attention à compiler avec l'option -lm
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#define NBCOL 784
int nb_class_max = 10;

//ATTENTION : je n'ai pas respecté les typedef de l'énoncé
//par exemple un pointeur vers une structure vecteur est de type vector* 

struct s_vector {
  int size;                      /* taille du tableau content */
  unsigned char* content;          /* tableau d'éléments de [[0, 255]] */
};
typedef struct s_vector vector;

struct s_classified_data {
    vector* vector; /* le vecteur */
    int class; /* sa classe */
};
typedef struct s_classified_data classified_data;

struct s_database {
    int size; /* taille du jeu de données */
    classified_data* datas; /* tableau contenant les données classifiées */
};
typedef struct s_database database;

struct s_candidate {
    int dataIndex; /* indice du point dans le jeu de donnée */
    double distToNeedle; /* distance au point de recherche */
    struct s_candidate* next;
};
typedef struct s_candidate candidate;

void main(){
    printf("Algo KNN - GUEGUEN Pierre\n\n");
}

vector* create_zero_vector(int n){
    int i;
    vector* v = malloc(sizeof(vector));
    assert(v != NULL);
    v->size = n;
    for(i = 0; i < n; i++){
        v->content[i] = 0;
    }
    return v;
}

void delete_vector(vector* v){
    free(v);
}

void print_vector(vector* v, bool newline){
    int i;

    printf("(");
    for(i = 0; i < v->size; i++){
        if(i!=0){
            printf(", ");
        }
        printf("%c", v->content[i]);
    }
    newline ? printf(")") : printf(")\n");
}

double sq(double a){return a*a;}

double distance(vector* u, vector* v){
    assert(u->size == v->size);
    double sum = 0;
    int i;

    for(i = 0; i < u->size; i++){
        sum += sq(u->content[i] - v->content[i]);
    }
    return sqrt(sum);
}

database* create_empty_database(int n){
    database* db = malloc(sizeof(database));
    assert(db != NULL);
    db->size = n;
    db->datas = malloc(n*sizeof(classified_data));
    return db;
}

void delete_database(database* db){
    int i;
    for(i = 0; i < db->size; i++){
        delete_vector(db->datas[i].vector);
    }
    free(db->datas);
    free(db);
}

void print_database(database* db){
    int i;

    printf("{");
    for(i = 0; i < db->size; i++){
        printf("\t");
        print_vector(db->datas[i].vector, false);
        printf(" ~> %d\n", db->datas[i].class);
    }
    printf("}");
}

candidate* create_candidate_list(int dataIndex, double distToNeedle){
    candidate* list = malloc(sizeof(candidate));
    assert(list != NULL);
    list->dataIndex = dataIndex;
    list->distToNeedle = distToNeedle;
    list->next = NULL;
    return list;
}

void delete_candidate_list(candidate* list){
    candidate* current = list;
    candidate* next;
    while(current != NULL){
        next = current->next;
        free(current);
        current = next;
    }
}

void print_candidate_list(candidate* list, database* db){
    candidate* current = list;
    printf("Candidate List :");
    while(current != NULL){
        printf("\t");
        print_vector(db->datas[current->dataIndex].vector, false);
        printf(" at dist: %f\n", current->distToNeedle);
        current = current->next;
    }
}
