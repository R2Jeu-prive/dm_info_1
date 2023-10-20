#include <assert.h>
#include <math.h>                  // Attention à compiler avec l'option -lm
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#define NBCOL 784
int nb_class_max = 10;

struct vector_s {
  int size;                      /* taille du tableau content */
  unsigned char* content;          /* tableau d'éléments de [[0, 255]] */
};
typedef struct vector_s vector;

struct classified_data_s {
    vector* vector; /* le vecteur */
    int class; /* sa classe */
};
typedef struct classified_data_s classified_data;

struct database_s {
    int size; /* taille du jeu de données */
    classified_data* datas; /* tableau contenant les données classifiées */
};
typedef struct database_s database;

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