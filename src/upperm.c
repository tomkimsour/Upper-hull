/* TP algorithmique parallele
 * maitrise
 * LL
 * 13/03/2018
 * utilise gnuplot au lieu de jgraph
 */

/*
 * upper.c
 *
 * programme principal en sequentiel
 */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "../include/point.h"
#include "../include/pb.h"
#include "../include/upperm.h"
#include "pvm3.h"
int nbPts;
static pb_t *Q[50];			/* la pile de problemes */
static int Q_nb;			/* l'index de tete de pile */

/*
 * initialise la file de problemes
 * chacun des PB problemes est un
 * probleme de tri de taille N
 * stocke dans le premier champ de
 * donnees de la structure.
 */

void init_queue(point * pts)
{
	point *upper, *pts2;
	int nb;

	nb = point_nb(pts);

	upper = point_UH(pts); 
	if (!upper) {
		pts2 = point_part(pts);
		upper_hull(pts);
		upper_hull(pts2);
		point_merge_UH(pts, pts2);
	}
	Q[0] = (pb_t *)malloc(sizeof(pb_t));
	Q[0]->taille1 = nbPts;
	Q[0]->taille2 = 0;
	Q[0]->data1 = pts;
	Q[0]->data2 = NULL;
	Q[0]->type = PB_UH;
	Q_nb = 1;	
}

/*
 * empile ou depile dans la 
 * pile globale Q un probleme
 */

pb_t *depile()
{
	if (Q_nb > 0)
		return Q[--Q_nb];
	else
		return NULL;
}

void empile(pb)
	pb_t *pb;
{
	Q[Q_nb++] = pb;
}

/*
*	modification de la structure de donnée pour pvm
*/
void set_data(int **data,point *pts){
	data = convert_point_to_array(pts,nbPts);
	print_array(data,nbPts);
}
/*
 * calcul recursif d'enveloppe
 * convexe par bissection
 */
void upper_hull(point *pts)
{
	//point *upper;
	// point *pts2;
	int i;
	int tids[P];		/* tids fils */
	pb_t *pb;
	int sender[1];

	//data = data_alloc(nbPts);
	//set_data(data,pts);	/* initialisation de tableau data*/
	init_queue(pts);		/* initialisation de la pile */
	/* lancement des P esclaves */
	pvm_spawn("/uppers", (char**)0, 0, "", P, tids);

	/* envoi d'un probleme (de tri) a un esclave*/
	for (i=0; Q_nb>0 && i<P; i++)
		send_pb(tids[i], depile());

	while (1) {
		pb_t *pb2;
//	
		/* reception d'une solution */
		// receive doit faire la différence entre UH et merge
		pb = receive_pb(-1, sender);
		empile(pb);
//	
	//		/* dernier probleme ? */
	//		if (pb->taille1 == nbPts)
	//			break;
	//		
		pb = depile();
		if (pb->type == PB_UH) 
			send_pb(*sender, pb);
		else { // PB_MERGE 
			pb2 = depile(); /* 2eme pb pour merge... */
			if (!pb2) {
				empile(pb); // rien a faire
			}
			else {
				if (pb2->type == PB_MERGE) { /* on fusionne pb et pb2 */
					pb->taille2 = pb2->taille1;
					pb->data2 = pb2->data1;
					send_pb(*sender, pb); /* envoi du probleme a l'esclave */
				}
				else { // PB_TRI
					empile(pb);
					send_pb(*sender, pb2); /* envoi du probleme a l'esclave */
				}
			}
		} 
	}

	pvm_mcast(tids, P, MSG_END); /* fin esclaves */
	//pb_print(pb);
	pb_free(pb);
	pvm_exit();
	exit(0);

}
/* ancienne enveloppe convexe*/
/*

void upper_hull(point *pts)
{
	point *upper, *pts2;

	upper = point_UH(pts); 
	if (!upper) {
		pts2 = point_part(pts);
		upper_hull(pts);
		upper_hull(pts2);
		point_merge_UH(pts, pts2);
	}
}

*/
/*
 * upper <nb points>
 * exemple :
 * % upper 200 
 * % evince upper_hull.pdf
 */

int main(int argc, char **argv)
{
	point *pts;

	if (argc != 2) {
		fprintf(stderr, "usage: %s <nb points>\n", *argv);
		exit(-1);
	}
	nbPts = atoi(argv[1]);
	pts = point_random(nbPts);
	point_print_gnuplot(pts, 0); /* affiche l'ensemble des points */
	upper_hull(pts);
	point_print_gnuplot(pts, 1); /* affiche l'ensemble des points restant, i.e
					l'enveloppe, en reliant les points */
	point_free(pts); 
	return 0;
}

