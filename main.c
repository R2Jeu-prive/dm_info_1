#include <assert.h>
#include <math.h>                  // Attention à compiler avec l'option -lm
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#define VECTOR_DIM 784
int nb_class_max = 10;

//ATTENTION : je n'ai pas respecté les typedef de l'énoncé
//par exemple un pointeur vers une structure vecteur est de type vector* 

struct s_vector {
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

unsigned char next_char(FILE* f) {
  unsigned char partial = 0;
  char c = fgetc(f);
  while (true) {
    if (c == ',' || c == '\n') {
      return partial;
    } else {
      partial = partial * 10 + (c - 48);//'0' -> 0
      c = fgetc(f);
    }
  }
}

vector* create_zero_vector(){
    int i;
    vector* v = malloc(sizeof(vector));
    assert(v != NULL);

    v->content = malloc(VECTOR_DIM*sizeof(unsigned char));
    assert(v->content != NULL);
    for(i = 0; i < VECTOR_DIM; i++){
        v->content[i] = 0;
    }
    return v;
}

void delete_vector(vector* v){
    free(v->content);
    free(v);
}

void print_vector(vector* v, bool newline){
    int i;

    printf("(");
    for(i = 0; i < VECTOR_DIM; i++){
        if(i!=0){
            printf(", ");
        }
        printf("%d", v->content[i]);
    }
    newline ? printf(")") : printf(")\n");
}

double sq(double a){return a*a;}

double distance(vector* u, vector* v){
    double sum = 0;
    int i;

    for(i = 0; i < VECTOR_DIM; i++){
        sum += sq(u->content[i] - v->content[i]);
    }
    return sqrt(sum);
}

database* create_empty_database(int n){
    database* db = malloc(sizeof(database));
    assert(db != NULL);
    db->size = n;
    db->datas = malloc(n*sizeof(classified_data));
    assert(db->datas != NULL);
    return db;
}

void fill_db(char* filename, database* db){
    int i, j;
    vector* currentVector;

    FILE* f = fopen(filename, "r");
    if(f == NULL){
        printf("fichier %s introuvable", filename);
    }

    for(i = 0; i < db->size; i++){
        db->datas[i].class = (int)next_char(f);
        currentVector = create_zero_vector();
        for(j = 0; j < VECTOR_DIM; j++){
            currentVector->content[j] = next_char(f);
        }
        db->datas[i].vector = currentVector;
    }

    fclose(f);
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

    printf("{\n");
    for(i = 0; i < db->size; i++){
        printf("\t");
        print_vector(db->datas[i].vector, false);
        printf(" ~> %d\n", db->datas[i].class);
    }
    printf("}\n");
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

void main(){
    printf("Algo KNN - GUEGUEN Pierre\n\n");
    database* train_db = create_empty_database(10);
    //printf("%d", train_db->datas[0]);
    fill_db("./data/train_1", train_db);
    print_database(train_db);
}
