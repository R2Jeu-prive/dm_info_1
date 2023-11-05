#include <assert.h>
#include <math.h>                  // Attention à compiler avec l'option -lm
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#define NBCOL 784
int nb_class_max = 10;

struct vector_s {
  int taille;                      /* taille du tableau content */
  unsigned char* content;          /* tableau d'éléments de [[0, 255]] */
};
typedef struct vector_s *vector;

struct classified_data_s {
  vector vector;                   /* le vecteur */
  int class;                       /* sa classe */
};
struct database_s {
  int size;                        /* taille du jeu de données */
  struct classified_data_s *datas; /* tableau contenant les données classifiées */
};
typedef struct database_s *database;

typedef struct s_cellule *candidats;
struct s_cellule {
  int indice;                   /* indice du point dans le jeu de donnée */
  double distance;              /* distance au point de recherche */
  candidats next;
};

/* prototypes pour que le code ci-après fonctionne */
database create_empty_database(int n);
vector create_zero_vector(int n);

/* Code permettant l'import de la base de donnée MNIST */
int int_of_char(char c) {
  switch (c) {
  case '0': return 0;
  case '1': return 1;
  case '2': return 2;
  case '3': return 3;
  case '4': return 4;
  case '5': return 5;
  case '6': return 6;
  case '7': return 7;
  case '8': return 8;
  default:  return 9;
  }
}

unsigned char next_char(FILE* f) {
  unsigned char partial = 0;
  char c = fgetc(f);
  while (true) {
    if (c == ',' || c == '\n') {
      return partial;
    } else {
      partial = partial * 10 + int_of_char(c);
      c = fgetc(f);
    }
  }
}

void mnist_input(int n, database *p_db_train, int m, database *p_db_test) {
  // hyp : le fichier ./MNIST-txt/MNIST_train.txt est présent
  // crée une bd d'entrainement avec les n premières données du fichier
  // remplit * p_db_train avec cette base
  // crée une bd de test avec les m données suivantes du fichier
  // remplit * p_db_test avec cette base
  FILE *f = fopen("./MNIST-txt/MNIST_train.txt", "r");
  if (f == NULL) {
    printf("le fichier ./MNIST-txt/MNIST_train.txt n'a pas pu être ouvert\n");
  }
  database db_train = create_empty_database(n);
  database db_test = create_empty_database(m);
  int class;
  for (int j = 0; j < n; j++) {
    class = next_char(f);
    vector v = create_zero_vector(NBCOL);
    for (int i = 0; i < NBCOL; i++) {
      unsigned char c = next_char(f);
      v->content[i] = c;
    }
    (db_train->datas)[j].vector = v;
    (db_train->datas)[j].class = class;
  }
  for (int j = 0; j < m; j++) {
    class = next_char(f);
    vector v = create_zero_vector(NBCOL);
    for (int i = 0; i < NBCOL; i++) {
      unsigned char c = next_char(f);
      v->content[i] = c;
    }
    (db_test->datas)[j].vector = v;
    (db_test->datas)[j].class = class;
  }
  fclose(f);
  *p_db_test = db_test;
  *p_db_train = db_train;
  return;
}
