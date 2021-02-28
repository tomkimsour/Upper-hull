/*
 * upper.h
 * SR 		13/04/94
 * LL 		14/10/97
 *
 *  Definition du programme upper
 */

#ifndef POINT_H
#define POINT_H

#define XMAX (1024)
#define YMAX (1024)

#define P			5			/* nombre d'esclaves */
#define SLAVE 		"uppers"	/* nom du programme esclave */

#define MSG_PB		0
#define MSG_END	1

#define PB_UH 0
#define PB_MERGE 1


/* 
 * structure associee a chaque point 
 */
typedef struct st_point point;
struct st_point {
	int x, y;
	point *next;	/* liste chainee des points de l'enveloppe */
};

/*
 * dans point.c
 * utilitaire de calcul pour le TAD point
 */
extern point *point_alloc();
extern void point_free();
extern void point_print();
extern point *point_random();
extern point *point_UH();
extern int point_nb();
extern point *point_part();
extern point *point_merge_UH();

/*
    un problème est soit :
        - un calcul UH soit
        - un merge
*/


typedef struct {
    int taille1, taille2;
    int type;
    int id;
    int nb;
    point *data1;
    point *data2;
} pb_t;


void print_array(int **arr,int size);
int ** convert_point_to_array(point *p,int nbPoints);
point * convert_array_to_point(int** arr, int nbPoints);
void copy_int();
pb_t * pb_alloc();
void pb_free();
void send_pb();
pb_t *receive_pb();
void pb_print();
int ** data_alloc();
void data_free();
void upper_hull();

void point_print_gnuplot(point *pts, int solid);

#endif