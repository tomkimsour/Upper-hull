/*
 *  point.c    
 *  SR 		13/04/94
 *  LL      13/03/18
 *
 *  Fonctions utiles dans le calcul de la
 *  enveloppe convexe
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <unistd.h>
#include "pvm3.h"
#include "point.h"

/*
 * (de)allocation
 */

point *point_alloc()
{
	point *pt;

	pt = (point *)malloc(sizeof(point));
	pt->x = pt->y = 0;
	pt->next = NULL;

	return pt;
}

/*
 * dealloue une liste de points
 */
void point_free(point *pts)
{
	if (!pts) return;
	point_free(pts->next);
	free(pts);
}

/*
   static int compareX(i,j)

   comparaison des abscisses de 2 points
   pour la fonction qsort
 */

static int compareX(point **i, point **j)
{ return (*i)->x - (*j)->x; }

/* 
   Construit un ensemble de points d'abscisse comprise
   entre 0 et XMAX et d'ordonnee entre 0 et YMAX. 
   Tous les points ont des abscisses et des ordonnees 
   differentes. Les points sont trie's par ordre croissant
   des abscisses.
 */

point *point_random(int nbPts)/* nombre de points a` tirer */
{
	int i,j;
	point **pts;

	pts = (point **)malloc(nbPts*sizeof(point *));
	for(i=0; i < nbPts; i++) {
		pts[i] = point_alloc();
		do {
			pts[i]->x = rand() % XMAX;
			for(j=0; j<i && pts[i]->x!=pts[j]->x; j++);
		} while( j != i );
		do {
			pts[i]->y = rand() % YMAX;
			for(j=0; j<i && pts[i]->y!=pts[j]->y; j++);
		} while( j != i );
	}

	qsort(pts, nbPts, sizeof(point *), (__compar_fn_t) compareX);
	for (i=0; i<nbPts-1; i++)
		pts[i]->next = pts[i+1];

	return (point *)*pts;
}

/*
 * genere une liste de points, avec ligne si solid == 1
 * generation au format jgraph
 */

void point_print(point *pts, int solid)
	/* solid: enveloppe ou non */
{
	if (!solid) {
		printf("newgraph\nlegend top\n");
	}

	printf("newcurve pts\n");
	while (pts) {
		printf("%d %d\n",pts->x, pts->y);
		pts=pts->next;
	}
	if (solid)
		printf("linetype solid\ntitle\n");
}

// au format gnuplot, dans 2 fichiers.
// appele ensuite gnuplot 
void point_print_gnuplot(point *pts, int solid)
	/* solid: enveloppe ou non */
{
	FILE *fp;
	if (!solid) 
		fp = fopen("points_gnuplot", "w");
	else
		fp = fopen("uh_gnuplot", "w");
	while (pts) {
		fprintf(fp, "%d %d\n",pts->x, pts->y);
		pts=pts->next;
	}
	fclose(fp);
	if (solid) {
		fp = fopen("cmd_gnuplot", "w");


		fprintf(fp, "set terminal pdf\n");
		fprintf(fp, "set output \"upper_hull.pdf\"\n");
		fprintf(fp, "plot \"points_gnuplot\" ps 2, \"uh_gnuplot\" with lines lw 4\n");

		fclose(fp);
		int systemRet;
		systemRet = system("gnuplot cmd_gnuplot");
		if(systemRet == -1){
			printf("gnuplot cmd_gnuplot failed\n");
			exit(-1);
		}
		sleep(1);
		//system("ps2pdf upper_hull.ps");
		systemRet = system("rm cmd_gnuplot points_gnuplot uh_gnuplot");
		if(systemRet == -1){
			printf("gnuplot cmd_gnuplot failed\n");
			exit(-1);
		}
		//system("rm cmd_gnuplot points_gnuplot uh_gnuplot upper_hull.ps");
		printf ("courbe dans upper_hull.pdf\n");
		printf ("evince upper_hull.pdf\n");
	}
}


/*
 * nombre de points dans une liste
 */
int point_nb(point *pts)
{
	int nb;
	point *cur;

	for (cur=pts, nb=0; cur != NULL; cur = cur->next, nb++);

	return nb;
}

/*
 * partitionne une liste en deux.
 * retourne le milieu de la liste 
 */

point *point_part(point *pts)
{
	int next;
	//int nb;
	point *cur, *mid, *midpred;

	midpred = NULL;
	for (mid=cur=pts, next=0; cur!=NULL; cur = cur->next, next=1-next)
		if (next) {
			midpred = mid;
			mid = mid->next;
		}
	if (midpred)
		midpred->next = NULL;
	return mid;
}


/* pente definie par 2 points */
#define PENTE(pt1,pt2) ( (float)((pt2)->y - (pt1)->y) / (float)((pt2)->x-(pt1)->x) )


/* 
   Calcule l'enveloppe convexe haute de 4 points au plus.
   Les points de l'enveloppe sont lies par une liste
   Les autres sont elimines de la liste.
 */

point *point_UH(point *pts) /* ensemble des points a` "envelopper" */
{
	int nb;
	point *cur, *tmp;

	nb = point_nb(pts);
	if (nb < 3) return pts;
	if (nb > 4) return NULL;

	if (nb == 3) {
		if (PENTE(pts, pts->next) < PENTE(pts, pts->next->next)) {
			tmp = pts->next;
			pts->next = tmp->next;
			free(tmp);
		}
		return pts;
	}

	cur = pts;
	if ( ( PENTE(cur, cur->next) < PENTE(cur, cur->next->next) ) ||
			( PENTE(cur, cur->next) < PENTE(cur, cur->next->next->next) ) ) {
		/* le deuxieme n'est pas sur l'enveloppe */
		tmp = cur->next;
		cur->next = tmp->next;
		free(tmp);
	}
	else
		cur = cur->next;

	if ( PENTE(cur, cur->next) < PENTE(cur, cur->next->next) ) {
		/* le troisieme point n'est pas sur l'enveloppe */
		tmp = cur->next;
		cur->next = tmp->next;
		free(tmp);
	}

	return pts;
}

/*
 * retourne 1 si la droite (pt1,pt2) est tangente
 * a l'enveloppe pts
 * (pt2 appartient a pts)
 */
int point_is_tangent(point *pt1, point *pt2, point *pts)
{
	point *next, *prev;
	float pente;

	for (prev=pts; prev!=NULL; prev=prev->next)
		if (prev->next == pt2) break;
	next = pt2->next;

	pente = PENTE(pt1, pt2);

	return 	((prev ? PENTE(prev,pt2)>=pente : 1) && 
			(next ? PENTE(pt2,next)<=pente : 1)) ? 1 : 0;
}

point *point_tangent(point *pt, point *pts)
{
	point *pt2;

	for (pt2=pts; pt2!=NULL; pt2=pt2->next)
		if (point_is_tangent(pt, pt2, pts)) return pt2;

	if (pt2==NULL) {
		printf("Warning : no tangent : \n");
		point_print(pt, 0);
		point_print(pts, 1);
		exit(-1);
	}
	return NULL;
}

/*
 * retourne dans ppt1 et ppt2 les points de tangence
 * de la tangeante commune des deux enveloppes pts1 et pts2
 */
void point_common_tangent(point *pts1, point *pts2, point **ppt1, point **ppt2)
{
	if(pts1 == NULL)
		exit(-1);
	point *pt1, *pt2;

	for (pt1=pts1; pt1!=NULL; pt1=pt1->next) {
		pt2 = point_tangent(pt1, pts2);
		if (point_is_tangent(pt2, pt1, pts1))
			break;
	}

	*ppt1 = pt1;
	*ppt2 = pt2;
}

/*
 * calcule l'enveloppe convexe commune de 
 * deux enveloppes convexes
 */
point *point_merge_UH(point *pts1, point *pts2)
{
	point *pt1, *pt2, *cur, *tmp;

	point_common_tangent(pts1, pts2, &pt1, &pt2);
	point_free(pt1->next);
	for (cur=pts2; cur!=pt2; cur=tmp) {
		tmp = cur->next;
		free(cur);
	}
	pt1->next = pt2;
	return pts1;
}

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
 * envoi d'un probleme a un processus tid
 * le probleme est dealloue localement
 */
void send_pb(tid, pb)
int tid;
pb_t *pb;
{
	int ** tab1;
	int ** tab2 = NULL;
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
	tab2 = NULL;

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
/* enveloppe convexe*/

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