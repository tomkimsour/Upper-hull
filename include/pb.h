/*
    un problème est soit :
        - un calcul UH soit
        - un merge
*/
typedef struct st_pb {
    int taille1, taille2;
    int type;
    int id;
    int nb;
    point *data1;
    point *data2;
} pb_t;


extern void print_array(int **arr,int size);
extern int ** convert_point_to_array(point *p,int nbPoints);
extern point * convert_array_to_point(int** arr, int nbPoints);
extern void copy_int();
extern void print_pb();
extern pb_t * pb_alloc();
extern void pb_free();
extern void send_pb();
extern pb_t *receive_pb();
extern void pb_print();
extern int ** data_alloc();
extern void data_free();