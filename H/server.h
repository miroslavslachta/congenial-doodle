//
// Created by miros on 26/12/2022.
//
#include <stdbool.h>

#ifndef SOKETY_SERVER_H
#define SOKETY_SERVER_H
int server(int argc, char *argv[]);
int rozhodca1(char buffer[256],char ** mat,int * staraX,int * staraY,int signal);
int rozhodca2(char buffer[256],char ** mat,int staraX,int staraY,int signal,int *pocet_x,int *pocet_o,bool* flag);
int kontrola_damy(char**mat,int staraX,int staraY,int x2,int y2,char c);
int kontrola_damy2(char**mat,int staraX,int staraY,int x2,int y2,char c);
int kontrola_damy3(char**mat,int *staraX,int *staraY,int x2,int y2,char c);
int kontrola_damy4(char**mat,int *staraX,int *staraY,int x2,int y2,char c,int n);
#endif //SOKETY_SERVER_H
