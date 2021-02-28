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
#include "pvm3.h"
#include "point.h"

int nbPts;
pb_t **Q;			/* la pile de problemes */
int Q_nb;			/* l'index de tete de pile */

/*
 * initialise la file de problemes
 * chacun des PB problemes est un
 * probleme de tri de taille N
 * stocke dans le premier champ de
 * donnees de la structure.
 */

void init_queue(point * pts)
{
	(void)pts;
	int nbPB = nbPts/2+1;
	Q = (pb_t **) malloc(nbPB * sizeof(pb_t *));
	Q_nb = 0;	
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

int decoupage_pts_rec(point * pts, int nb, int b_index){
	int e_index=b_index;
	point *pts2;
	pb_t *pb;
	int nbelem;
	if(nbPts>50){
		pts2 = point_part(pts);
		nbelem = nb/2 + nb%2;
		b_index = decoupage_pts_rec(pts, nb/2, b_index);
		e_index = decoupage_pts_rec(pts2, nbelem, e_index+1);
	} else {
		pb=pb_alloc();
		pb->type=PB_UH;
		pb->id=b_index;
		pb->nb=1;
		pb->taille1=nb;
		pb->data1=pts;
		empile(pb);
	}
	return e_index;
}

int decoupage_pts(point * pts){
	return decoupage_pts_rec(pts,nbPts, 0) +1;
}

void ajout_merge_pb(pb_t ** merge_pb, pb_t* pb){
	merge_pb[pb->id] = pb;
	merge_pb[pb->id+pb->nb-1] = pb;
}

void retrait_merge_pb(pb_t ** merge_pb, pb_t* pb){
	merge_pb[pb->id] = NULL;
	merge_pb[pb->id+pb->nb-1] = NULL;
} 
/*	
 *	cherche un ensemble de point voisin par id
 */
pb_t* pb_voisin(pb_t** merge_pb, int size, pb_t* pb){
	pb_t *res = NULL;
	if(pb->id>0) res = merge_pb[pb->id-1];
	if(res!=NULL){
		retrait_merge_pb(merge_pb, res);
		return res;
	}
	if(pb->id+pb->nb<size) res = merge_pb[pb->id+pb->nb];
	if(res!=NULL){
		retrait_merge_pb(merge_pb, res);
		return res;
	}
	return res;
}
/*
 * combine deux pb 
 */
void merge_pb_voisin(pb_t* pb1, pb_t* pb2){
	pb1->type = PB_MERGE;
	if(pb1->id>pb2->id){
		pb1->id = pb2->id;
		pb1->data2 = pb1->data1;
		pb1->data1 = pb2->data1;
		pb1->taille2 = pb1->taille1;
		pb1->taille1 = pb2->taille1;
	} 
	else {
		pb1->data2 = pb2->data1;
		pb1->taille2 = pb2->taille1;
	}
	pb1->nb = pb1->nb+pb2->nb;
}

/*
 * enveloppe convexe 
 */
void upper_hull_master(point *pts)
{
	//point *upper;
	// point *pts2;
	int i;
	int tids[P];		/* tids fils */
	pb_t *pb;
	int sender[1];
	int nbPb;
	printf("Initialisation des données\n");
	init_queue(pts);		/* initialisation de la pile */
	//decoupage des problemes en groupes de points < 4
	nbPb = decoupage_pts(pts);

	//tableau stockant les merges en fonction de leur indice
	printf("Creation tableau merge\n");
	pb_t ** merge_pb = (pb_t **) malloc(nbPb * sizeof(pb_t*));
	for(i=0;i<nbPb;i++) merge_pb[i] = NULL;
	/* lancement des P esclaves */
	printf("Lancement des fils\n");
	pvm_spawn(EPATH "/uppers", (char**)0, 0, "", P, tids);

	/* envoi d'un probleme UH a chaque esclave*/
	for (i=0; Q_nb>0 && i<P; i++)
		send_pb(tids[i], depile());

	printf("Commencement des échanges de données entre maitre/esclave\n");
	while (1) {
		pb_t *pb2;
		/* reception d'une solution */
		pb = receive_pb(-1, sender);

		/* dernier probleme ? */
		if (pb->id == 0 && pb->nb == nbPb)
			break;
		//traitement es problèmes UH	
		if (Q_nb>0){
			ajout_merge_pb(merge_pb,pb);
			pb = depile();
			send_pb(*sender, pb);
		} 
		else { // PB_MERGE 
			pb2 = pb_voisin(merge_pb, nbPb, pb);
			if(pb2!=NULL){
				merge_pb_voisin(pb, pb2);
				free(pb2);
				send_pb(sender[0], pb);
			} else{
				ajout_merge_pb(merge_pb, pb);
			}
		} 
	}
	printf("Fin de transmission\n");
	printf("Fin esclaves\n");
	pvm_mcast(tids, P, MSG_END); /* fin esclaves */
	pb_free(pb);
	free(merge_pb);
	
	pvm_exit();
	exit(0);

}


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
	printf("Début du programme\n");
	printf("Generation aleatoire des points\n");
	pts = point_random(nbPts);
	point_print_gnuplot(pts, 0); /* affiche l'ensemble des points */
	upper_hull_master(pts);
	point_print_gnuplot(pts, 1); /* affiche l'ensemble des points restant, i.e
					l'enveloppe, en reliant les points */
	point_free(pts); 
	free(Q);
	return 0;
}

