#include "../H/matrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
char ** mat2;
char** alokujMatrix(){
    mat2=(char**) calloc(8,sizeof(char*));
    if(!mat2) return NULL;
    for (int i = 0; i < 8; ++i) {
        mat2[i] = (char*) calloc(8,sizeof(char));
        if(!mat2[i]){
            return NULL;
        }
    }
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if((i==0 && j==1) || (i==0 && j==3) || (i==0 && j==5) || (i==0 && j==7) || (i==1 && j==0) || (i==1 && j==2) || (i==1 && j==4) || (i==1 && j==6) ){
                *(*(mat2+i)+j) = 'o';
            }else if((i==7 && j==0) || (i==7 && j==2) || (i==7 && j==4) || (i==7 && j==6 )||( i==6 && j==1) || (i==6 && j==3) || (i==6 && j==5) || (i==6 && j==7)){
                *(*(mat2+i)+j) = 'x';
            }else{
                *(*(mat2+i)+j) = ' ';
            }
        }
    }
    return mat2;
}
void free_matrix(char** mat){
    for (int i = 0; i < 8; ++i) {
        free(mat[i]);
    }
    free(mat);
    mat = NULL;

}
void uloz_matrix(char** mat,char *output){
    sprintf(output,"\n    0  1  2  3  4  5  6  7 \n");
    char pismena = 'A';
    for (int i = 0; i < 8; ++i) {
        sprintf(output,"%s%c: ",output, pismena);
        pismena++;
        for (int j = 0; j < 8; ++j) {
            sprintf(output,"%s[%c]",output, *(*(mat+i)+j)); //
        }
        sprintf(output,"%s\n",output);
    }
}