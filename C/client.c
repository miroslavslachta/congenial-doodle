#include "../H/client.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int client(int argc, char *argv[]) {
    printf("\n    0  1  2  3  4  5  6  7 \n");
    char pismeno = 'A';
    for (int i = 0; i < 8; ++i) {
        printf("%c: ",pismeno);
        pismeno++;
        for (int j = 0; j < 8; ++j) {
            if((i==0 && j==1) || (i==0 && j==3) || (i==0 && j==5) || (i==0 && j==7) || (i==1 && j==0) || (i==1 && j==2) || (i==1 && j==4) || (i==1 && j==6) ){
                printf("[o]");
            }else if((i==7 && j==0) || (i==7 && j==2) || (i==7 && j==4) || (i==7 && j==6 )||( i==6 && j==1) || (i==6 && j==3) || (i==6 && j==5) || (i==6 && j==7)){
                printf("[x]");
            }else{
                printf("[ ]");
            }
        }
        printf("\n");
    }

    int n;
    int sockfd_C;
    struct sockaddr_in serv_addr;
    struct hostent *server; //hostent struktura uchovava informacie o servery napr ake ip adresy su k serveru pripojene
    char buffer[256];
    char mapa2[1001];

    if (argc < 3) {fprintf(stderr, "usage %s hostname port\n", argv[0]);return 1;}

    server = gethostbyname(argv[1]); //pretransformuje localhost alebo frios2 na ip adresu
    if (server == NULL) {fprintf(stderr, "Error, no such host\n");return 2;}

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(
            (char *) server->h_addr,
            (char *) &serv_addr.sin_addr.s_addr,
            server->h_length
    );
    serv_addr.sin_port = htons(atoi(argv[2]));

    sockfd_C = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_C < 0) {perror("Error creating socket");return 3;}

    if (connect(sockfd_C, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) <0) //ked klient vyokna connect server je odblokovany od accept
    {perror("Error connecting to socket");return 4;}

    char hrac='x';
    while(1) {
        printf("na tahu hrac(%c): ",hrac);

        bzero(buffer, 256);
        fgets(buffer, 255, stdin);
        n = write(sockfd_C,buffer, strlen(buffer));
        if (n < 0) {perror("Error writing to socket");break;
        }else if(buffer[0]== 'k' ||buffer[0]== 'K'){
            if(hrac == 'x'){
                printf("vyhrali Ocka\n");
            }else{
                printf("vyhrali Xka\n");
            }
            printf("koniec hry\n");
            break;
        }

        bzero(buffer, 256);
        n = read(sockfd_C, buffer, 255);
        if (n < 0) {perror("Error reading from socket");break;}

        printf("%s\n",buffer);

        if (buffer[0]== 's'){
            printf("na druhom tahu hrac(%c): ",hrac);

            bzero(buffer, 256);
            fgets(buffer, 255, stdin);
            n = write(sockfd_C,buffer, strlen(buffer));
            if (n < 0) {perror("Error writing to socket");break;
            }else if(buffer[0]== 'k' ||buffer[0]== 'K'){
                if(hrac == 'x'){
                    printf("vyhrali Ocka\n");
                }else{
                    printf("vyhrali Xka\n");
                }
                printf("koniec hry\n");
                break;
            }

            bzero(mapa2, 1001);
            n = read(sockfd_C, mapa2, 1000);
            if (n < 0) {perror("Error reading from socket");break;}
            printf("%s\n",mapa2);

            bzero(buffer, 256);
            buffer[0]='O';
            buffer[1]='K';
            buffer[2]='\0';
            n = write(sockfd_C,buffer, strlen(buffer));
            if (n < 0) {perror("Error writing to socket");break;}

            bzero(buffer, 256);
            n = read(sockfd_C, buffer, 255);
            if (n < 0) {perror("Error reading from socket");break;}

            printf("%s\n",buffer);



            if(buffer[0]== 'k' ||buffer[0]== 'K'){
                if(hrac == 'x'){
                    printf("vyhrali Xka\n");
                }else{
                    printf("vyhrali Ocka\n");
                }
                printf("koniec hry\n");
                break;
            }
            if (buffer[0]== 's') {
                if (hrac == 'x') {
                    hrac = 'o';
                } else {
                    hrac = 'x';
                }
            }else{
                printf("hrac ide este raz\n");
            }
        }else{
            printf("hrac ide este raz\n");
        }
    }

    close(sockfd_C);
    return 0;
}
