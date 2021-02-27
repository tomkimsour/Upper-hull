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

extern void print_array(int (*arr)[2], int size);
int ** convert_point_to_array(point *p,int nbPoints);
extern point * convert_array_to_point(int** arr);
extern void copy_int();
extern void print_pb();
extern pb_t * pb_alloc();
extern void pb_free();
extern void send_pb();
extern pb_t receive_pb();
extern void pb_print();