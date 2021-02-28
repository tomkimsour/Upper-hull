#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "../include/point.h"
#include "../include/upperm.h"
#include "../include/pb.h"
#include "pvm3.h"

void print_array(int **arr, int size){
    printf("array : ");
    for (int i = 0; i<size;i++){
        printf("(%d,%d) ",arr[i][0],arr[i][1]);
    }
    printf("\n");
}

int ** convert_point_to_array(point *p, int nbPoints){
    int **array;
    int i;
	array = (int**) malloc(nbPoints * sizeof(int*));
    for (i=0;i<nbPoints;i++){
		array[i] = (int*) malloc(2 * sizeof(int));
        array[i][0] = p->x;
        array[i][1] = p->y;
		p = p->next;
    }
    return array;
}

point * convert_array_to_point(int ** arr,int nbPoints){
    point *pts;
	point * cur;
    int i;
	if(nbPoints<1) exit(0); 
	pts = point_alloc();
	cur = pts;
	cur->x = arr[0][0];
	cur->y = arr[0][1];
	for (i=1;i<nbPoints;i++){
		cur->next = point_alloc();
		cur = cur->next;
		cur->x = arr[i][0];
		cur->y = arr[i][1];
	}
    return pts;
}

/*
 * (de)allocation d'un probleme
 */
pb_t *pb_alloc()
{
	pb_t *pb;

	pb = (pb_t *)malloc(sizeof(pb_t));
	pb->taille1 = pb->taille2 = pb->type = 0;
	pb->data1 = pb->data2 = NULL;

	return pb;
}

void pb_free(pb)
pb_t *pb;
{
	if (pb->data1) free(pb->data1);
	if (pb->data2) free(pb->data2);
	free(pb);
}

/*
 * affichage d'un probleme
 */
/*
void pb_print(pb)
pb_t *pb;
{
	int i;

	printf("type: %s\n", pb->type==PB_UH ? "Upper Hull" : "Merge points");
	printf("tai1: %2d\n", pb->taille1);
	for (i=0; i<pb->taille1; i++) {
		if (i % 8 == 0) printf("\n");
		printf("%3d	", pb->data1[i]);
	}
	printf("\n");
	printf("tai2: %2d\n", pb->taille2);
	for (i=0; i<pb->taille2; i++) {
		if (i % 8 == 0) printf("\n");
		printf("%3d	", pb->data2[i]);
	}
	printf("\n");
}
*/
/* 
 * envoi d'un probleme a un processus tid
 * le probleme est dealloue localement
 */
void send_pb(tid, pb)
int tid;
pb_t *pb;
{
	int ** tab1;
	int ** tab2;
	int i;

	pvm_initsend(PvmDataDefault);
	pvm_pkint(&(pb->taille1), 1, 1);
	pvm_pkint(&(pb->taille2), 1, 1);
	pvm_pkint(&(pb->type), 1, 1);
	pvm_pkint(&(pb->id),1,1);
	pvm_pkint(&(pb->nb),1,1);
	tab1 = convert_point_to_array(pb->data1,pb->taille1);
	for(i = 0;i<pb->taille1;i++){
		pvm_pkint(&(tab1[i][0]),(tab1[i][1]),1);
	}
	if (pb->taille2 > 0){
		tab2 = convert_point_to_array(pb->data2,pb->taille2);
		for(i = 0;i<pb->taille2; i++){
			pvm_pkint(&(tab2[i][0]),tab2[i][1],1);
		}
	}	
	pvm_send(tid, MSG_PB);

	data_free(tab1,pb->taille1);
	data_free(tab2,pb->taille2);
	pb_free(pb);
}


/*
 * copie N entiers de src vers dst
 */
void copy_int(dst, src, n)
int *dst, *src, n;
{
	int i;
	
	for (i=0;i<n; i++) dst[i] = src[i];
}

pb_t *receive_pb(tid, sender)
int tid;
int *sender;
{
    int bufid, bytes[1], msgtag[1];
    pb_t *pb;
    int i;
    int **tab1, **tab2;

    bufid = pvm_recv(tid, -1);
    pvm_bufinfo(bufid, bytes, msgtag, sender);
    if(msgtag[0] == MSG_END) return NULL;

    pb = pb_alloc();
	pb->data2 = (point *) 0;

    /* unpackage */
    pvm_upkint(&(pb->taille1), 1, 1);
    pvm_upkint(&(pb->taille2), 1, 1);
    pvm_upkint(&(pb->type), 1, 1);
    pvm_upkint(&(pb->id), 1, 1);
    pvm_upkint(&(pb->nb), 1, 1);

    tab1 = data_alloc(pb->taille1);
    for(i=0; i<pb->taille1; i++){
        pvm_upkint(tab1[i], 2, 1);
    }
    if (pb->taille2>0){
        tab2 = data_alloc(pb->taille2);
        for(i=0; i<pb->taille2; i++){
            pvm_upkint(tab2[i], 2, 1);
        }
    }

    /* conversion des tableaux recus en listes de points */
    pb->data1 = convert_array_to_point(tab1, pb->taille1);
    data_free(tab1, pb->taille1);
    if (pb->taille2>0){
        pb->data2 = convert_array_to_point(tab2, pb->taille2);
        data_free(tab2, pb->taille2);
    }

    return pb;
}

int ** data_alloc(int size){
	int ** data;
	data = (int**)malloc(size*sizeof(int*));
	for (int i = 0;i<size;i++){
		data[i] = (int*) malloc(2 * sizeof(int));
	}
	return data;
}

void data_free(int ** data, int size){
	if (size<1) return;
	for(int i = 0; i < size; i++)
	    free(data[i]);
	free(data);
}