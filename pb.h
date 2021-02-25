/*
    un problème est soit :
        - un calcul UH soit
        - un merge
*/
struct st_pb {
    int taille1, taille2;
    int type;
    int *data1;
    int *data2;
};

typedef struct st_pb pb_t;

extern void print_array(int ** arr, int size);
extern int ** convert_point_to_array(point p);
extern point * convert_array_to_point(int** arr);