/* TP algorithmique parallele
 * maitrise
 * LL
 * 07/10/97
 */

/* 
 * sorts.c
 * algorithme esclave
 * -- tri fusion maitre esclave
 */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "pvm3.h"
#include "point.h"

/*
 * programme esclave
 */

int main(){
	extern pb_t *receive_pb();
	int parent, sender[1];	/* pere et envoyeur (non utilise) */
	pb_t *pb;					/* probleme courant */

	parent = pvm_parent();

	/* tant que l'on recoit un probleme a traiter ... */
	while ((pb = receive_pb(parent, sender)) != NULL) {
		if (pb->type == PB_UH){
            /* on traite suivant le cas */
            upper_hull(pb->data1);
            pb->taille1 = point_nb(pb->data1); 
        } 
        else {
            point_merge_UH(pb->data1,pb->data2);
            pb->taille1 = point_nb(pb->data1);
            pb->data2 = NULL;
            pb->taille2 = 0;
        }
		/* et on revoie la solution */
		send_pb(parent, pb);
	}

	pvm_exit();
	return 0;
}