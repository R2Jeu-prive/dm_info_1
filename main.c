#include <assert.h>
#include <math.h>                  // Attention à compiler avec l'option -lm
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#define VECTOR_DIM 784
#define ASCII_GRAYSCALE " .'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%%B@$"
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

void show_vector(vector* v){
    assert(VECTOR_DIM == 784);
    int x, y;
    int val;
    for(x = 0; x < 28; x++){
        for(y = 0; y < 28; y++){
            val = (int)v->content[28*x + y];
            val = 68*val/255;
            printf("%c",ASCII_GRAYSCALE[val]);
        }
        printf("\n");
    }
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
        show_vector(db->datas[i].vector);
        printf(" ~> %d\n", db->datas[i].class);
    }
    printf("}\n");
}

candidate* create_candidate(int dataIndex, double distToNeedle){
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
        show_vector(db->datas[current->dataIndex].vector);
        printf(" at dist: %f\n", current->distToNeedle);
        current = current->next;
    }
}

int insert_in_candidate_list(candidate** firstCandidatePointer, int len, int k, database* db, int index, vector* needle){
    vector* inserted_vector = db->datas[index].vector;
    double distToNeedle = distance(needle, inserted_vector);
    candidate* new_candidate = create_candidate(index, distToNeedle);
    candidate* current_candidate = (*firstCandidatePointer);
    candidate* candidate_to_delete;

    if((*firstCandidatePointer)->distToNeedle < distToNeedle && len == k){
        //worse than all vectors in list and list is full
        delete_candidate_list(new_candidate);
        return k;
    }
    if((*firstCandidatePointer)->distToNeedle < distToNeedle){
        //worse than all vectors in list but list is not full;
        new_candidate->next = (*firstCandidatePointer);
        (*firstCandidatePointer) = new_candidate;
        return len+1;
    }

    //new_candidate is better than worst candidate so order it
    while(current_candidate->next != NULL && current_candidate->next->distToNeedle >= distToNeedle){
        current_candidate = current_candidate->next;
    }
    new_candidate->next = current_candidate->next;
    current_candidate->next = new_candidate;

    //if we were full than remove worse candidate (first candidate in list)
    if(len == k){
        candidate_to_delete = (*firstCandidatePointer);
        (*firstCandidatePointer) = (*firstCandidatePointer)->next;
        candidate_to_delete->next = NULL;
        delete_candidate_list(candidate_to_delete);
        return k;
    }

    return len+1;
}

candidate* knn(database* db, int k, vector* needle){
    assert(db->size > 0);
    candidate* list = create_candidate(0, distance(db->datas[0].vector, needle));
    int i;
    int len = 1;

    for(i = 1; i < db->size; i++){
        len = insert_in_candidate_list(&list, len, k, db, i, needle);
    }

    return list;
}

void main(){
    printf("Algo KNN - GUEGUEN Pierre\n\n");
    database* train_db = create_empty_database(1000);
    database* test_db = create_empty_database(10);
    fill_db("./data/train_1", train_db);
    fill_db("./data/test", test_db);
    vector* needle = test_db->datas[1].vector;
    show_vector(needle);
    printf("\n\n\n\n");
    candidate* closestVectors = knn(train_db, 10, needle);
    print_candidate_list(closestVectors, train_db);
}
