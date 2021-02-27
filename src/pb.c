#include <stdio.h>
#include <malloc.h>
#include "../include/point.h"
#include "../include/upperm.h"
#include "../include/pb.h"
#include "pvm3.h"

void print_array(int (*arr)[2], int size){
    printf("array : ");
    for (int i = 0; i<size;i++){
        printf("(%d,%d) ",arr[i][0],arr[i][1]);
    }
    printf("\n");
}

int ** convert_point_to_array(point *p, int nbPoints){
    int array[nbPoints][2];
    point cur;
    int i = 0;
    for (cur=p; cur.next!=NULL; cur = cur.next){
        array[i][0] = cur.x;
        array[i][1] = cur.y;
        i++; 
    }
    return array;
}

point ** convert__array_to_point(int ** arr,int nbPoints){
    point **pts;
    int i;
    pts = (point **)malloc(nbPoints*sizeof(point *));
    for (i =0 ; i<nbPoints; i++){
        pts[i] = point_alloc();
        pts[i]->x = arr[i][0];
        pts[i]->y = arr[i][1];
    }
	for (i=0; i<nbPoints-1; i++)
		pts[i]->next = pts[i+1];
    return (point *)*pts;
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

/* 
 * envoi d'un probleme a un processus tid
 * le probleme est dealloue localement
 */
void send_pb(tid, pb)
int tid;
pb_t *pb;
{
	pvm_initsend(PvmDataDefault);
	pvm_pkint(&(pb->taille1), 1, 1);
	pvm_pkint(&(pb->taille2), 1, 1);
	pvm_pkint(&(pb->type), 1, 1);
	pvm_pkint(pb->data1, pb->taille1, 1);
	if (pb->taille2 > 0)	
		pvm_pkint(pb->data2, pb->taille2, 1);
	pvm_send(tid, MSG_PB);

	pb_free(pb);
}

/*
 * reception d'un probleme venant d'un processus tid
 * allocation locale pour le probleme
 * met a jour le le tid de l'envoyeur dans sender 
 * (utile dans le cas ou la reception venait 
 * d'un processus indifferent (tid == -1))
 * retourne NULL si le message n'est pas de type MSG_PB
 */

pb_t *receive_pb(tid, sender)
int tid;
int *sender;
{
	int bufid, bytes[1], msgtag[1];
	pb_t *pb;

	bufid = pvm_recv(tid, -1);
	pvm_bufinfo(bufid, bytes, msgtag, sender);
	if (*msgtag != MSG_PB) return NULL;

	pb = (pb_t *)malloc(sizeof(pb_t));

	pvm_upkint(&(pb->taille1), 1, 1);
	pvm_upkint(&(pb->taille2), 1, 1);
	pvm_upkint(&(pb->type), 1, 1);
	pb->data1 = (int *) malloc(pb->taille1*sizeof(int));
	pvm_upkint(pb->data1, pb->taille1, 1);
	pb->data2 = (int *)0;
	if (pb->taille2 > 0) {
		pb->data2 = (int *) malloc(pb->taille2*sizeof(int));
		pvm_upkint(pb->data2, pb->taille2, 1);
	}
	else
		pb->data2 = (int *) 0;

	return pb;
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