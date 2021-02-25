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

#include "stdio.h"
#include "stdlib.h"
#include "point.h"
#include "pb.h"
#include "upper.h"
/*
 * calcul recursif d'enveloppe
 * convexe par bissection
 */
void upper_hull(point *pts)
{
	point *upper, *pts2;

	upper = point_UH(pts); /* retourne 0 si plus de 4 points */
	if (!upper) {
		pts2 = point_part(pts);
		upper_hull(pts);
		upper_hull(pts2);
		point_merge_UH(pts, pts2);
	}
	int i;
	int tids[P];		/* tids fils */
	int data[DATA];	/* donnees */
	int sender[1];

	set_random_data(data);	/* initialisation aleatoire */
	init_queue(data);				/* initialisation de la pile */
	/* lancement des P esclaves */
	pvm_spawn("/uppers", (char**)0, 0, "", P, tids);

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

main(int argc, char **argv)
{
	point *pts;

	if (argc != 2) {
		fprintf(stderr, "usage: %s <nb points>\n", *argv);
		exit(-1);
	}
	pts = point_random(atoi(argv[1]));
	point_print_gnuplot(pts, 0); /* affiche l'ensemble des points */
	upper_hull(pts);
	point_print_gnuplot(pts, 1); /* affiche l'ensemble des points restant, i.e
					l'enveloppe, en reliant les points */
	point_free(pts); 
}

