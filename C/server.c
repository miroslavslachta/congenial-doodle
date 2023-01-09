#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../H/server.h"
#include <pthread.h>
#include <unistd.h>
#include "../H/matrix.h"
#include <stdbool.h>

typedef struct spol_data{
    char **mat;
    int * pocet_x;
    int * pocet_o;
    int signal;
    int *stara_X;
    int *stara_Y;
    int *databaza_signal;
    int *databaza_typ;
    int *index;
    int cakanie;
    int pocet_hracov;
    pthread_cond_t *databaza_Caka;
    pthread_cond_t *server_caka;
    pthread_mutex_t *databaza_mutex;
    pthread_mutex_t * tah_mutex;
}SPOL_DATA;

typedef struct server{
    int hrac_id;
    char *argv;
    int newsockfd;
    SPOL_DATA *spol_data;
}SERVER;

typedef struct databaza{
    int cas;
    int * Xka;
    int * Ocka;
    SPOL_DATA *spol_data;
}DATABAZA;

int porovnanie(char c){
    if (c == 'A' || c == 'a') {
        return 0;
    } else if (c == 'B' || c == 'b') {
        return 1;
    } else if (c == 'C' || c == 'c') {
        return  2;
    } else if (c == 'D' || c == 'd') {
        return 3;
    } else if (c == 'E' || c == 'e') {
        return 4;
    } else if (c == 'F' || c == 'f') {
        return 5;
    } else if (c == 'G' || c == 'g') {
        return 6;
    } else if (c == 'H' || c == 'h') {
        return 7;
    } else if (c == 'K' || c == 'k') {
        return 10;
    } else {
        return 8;
    }
}


void * server_fun(void *parameters) {
    SERVER *server = (SERVER *) parameters;
    int n;
    bool flag=false;
    const char *msg;
    char buffer[256];
    char *mapa = calloc(1000,sizeof(char));

    printf("hrac cislo(%d)\n", server->hrac_id);
    while (*server->spol_data->pocet_o != 0 && *server->spol_data->pocet_x != 0 ) {

        pthread_mutex_lock(server->spol_data->tah_mutex);
        if ((server->spol_data->signal == server->hrac_id )&& (*server->spol_data->pocet_o) != 0 && (*server->spol_data->pocet_x != 0)) {
            pthread_mutex_unlock(server->spol_data->tah_mutex);
            while (1) {
                printf("hop som tu\n");
                bzero(buffer, 256);
                n = read(server->newsockfd, buffer, 255);
                if (n < 0) {
                    perror("Error reading from socket");
                    break;
                }
                printf("tah hraca cislo(%d): %s\n", server->hrac_id, buffer);

                printf("az sem\n");
                int rozhodnutie;
                rozhodnutie = rozhodca1(buffer, server->spol_data->mat,server->spol_data->stara_X,
                                        server->spol_data->stara_Y,server->spol_data->signal);
                printf("server starx:%d stary:%d\n",*server->spol_data->stara_X,*server->spol_data->stara_X);


                if(rozhodnutie ==2 ) {

                    *server->spol_data->pocet_x = 0;
                    printf("prehrali4\n");
                    server->spol_data->signal++;
                    break;
                }
                if (n != 3 ) {
                    msg = "nespravna sprava";
                    n = write(server->newsockfd, msg, strlen(msg) + 1);
                    if (n < 0) { perror("Error writing to socket"); }break;}

                if (rozhodnutie == 0) {
                    msg = "spravna volba";
                    n = write(server->newsockfd, msg, strlen(msg) + 1);
                    if (n < 0) {perror("Error writing to socket");break;}

                    bzero(buffer, 256);
                    n = read(server->newsockfd, buffer, 255);
                    if (n < 0) {perror("Error reading from socket");break;}

                    printf("druhy tah hraca cislo(%d): %s\n", server->hrac_id, buffer);

                    rozhodnutie =rozhodca2(buffer,server->spol_data->mat,*server->spol_data->stara_X,
                      *server->spol_data->stara_Y,server->spol_data->signal,server->spol_data->pocet_x,server->spol_data->pocet_o,&flag);


                    printf("krasa\n");

                    uloz_matrix(server->spol_data->mat,mapa);

                    n = write(server->newsockfd, mapa, strlen(mapa) + 1);
                    if (n < 0) {
                        perror("Error writing to socket");
                    }
                    bzero(buffer, 256);
                    n = read(server->newsockfd, buffer, 255);
                    if (n < 0) {
                        perror("Error reading from socket");
                        break;
                    }
                    printf("%s\n",buffer);
                    if (rozhodnutie == 2) {
                        printf("prehrali2\n");
                        *server->spol_data->pocet_x = 0;
                        server->spol_data->signal++;
                        break;
                    }
                        if (rozhodnutie ==0 || (rozhodnutie ==1 && flag==true)) {
                            printf("oo:%d xx:%d\n",*server->spol_data->pocet_o,*server->spol_data->pocet_x);
                            if(*server->spol_data->pocet_o != 0 && *server->spol_data->pocet_x != 0) {
                                printf("krasa2\n");
                                if(rozhodnutie ==0) {
                                    msg = "spravny tah\n";
                                }else{
                                    msg = "spravny tah len z polovice\n";
                                }
                                n = write(server->newsockfd, msg, strlen(msg) + 1);
                                server->spol_data->signal++;
                                if (n < 0) {
                                    perror("Error writing to socket");
                                    break;
                                }
                            }else{
                                printf("krasa\n");
                                msg = "koniec\n";
                                n = write(server->newsockfd, msg, strlen(msg) + 1);
                                if (n < 0) {
                                    perror("Error writing to socket");
                                }
                            }
                            printf("davam brake\n");
                            break;
                        } else if (rozhodnutie == 1){
                        msg = "nespravny tah\n";
                        n = write(server->newsockfd, msg, strlen(msg) + 1);
                        if (n < 0) { perror("Error writing to socket"); }
                        }

                    } else {
                    msg = "nespravna suradnica";
                    n = write(server->newsockfd, msg, strlen(msg) + 1);
                    if (n < 0) { perror("Error writing to socket"); }
                }

                if (server->spol_data->signal == server->spol_data->pocet_hracov+1) { server->spol_data->signal = 1; }

            }


            pthread_mutex_lock(server->spol_data->tah_mutex);
        }
        pthread_mutex_unlock(server->spol_data->tah_mutex);
        if (server->spol_data->signal == server->spol_data->pocet_hracov+1) { server->spol_data->signal = 1; }
    }
    server->spol_data->cakanie++;
    while(server->spol_data->cakanie != server->spol_data->pocet_hracov){}
    free(mapa);
    close(server->newsockfd);
    printf("koniec\n");
    return NULL;
}
int rozhodca1(char buffer[256],char ** mat,int *staraX,int *staraY,int signal) {
    *staraY=atoi(&buffer[1]);

    *staraX=porovnanie(buffer[0]);
    if(*staraX==10) {
        printf("hraci skonicili\n");
        return 2;
    }

    if (*staraX == 8 || *staraY < 0 || *staraY > 7) {
        printf("nespravna sprava\n");
        return 1;
    }

    char c=mat[*staraX][*staraY];
    printf("x-:%d y-:%d\n",*staraX,*staraY);
    printf("c2:%c\n",c);
    if ((signal % 2 == 1 && c != 'x' && c != 'X')|| (signal % 2 == 0 && c != 'o'  && c != '0')) {
        return 1;
    }

    printf("x:%d\n",*staraX);
    printf("y:%d\n",*staraY);
    for (int i = 0; i < 2; ++i) {
        printf("buf[%d]:%c\n",i,buffer[i]);
    }

    printf("rozhodca vyhodnitil spravnu volbu panacika\n");
    return 0;
}
int rozhodca2(char buffer[256],char ** mat,int staraX,int staraY,int signal,int *pocet_x,int *pocet_o,bool *flag) {
    int x;
    int y;
    char c;
    int return_kontrola;
    *flag = false;
    int pocet_opakovani = 3;
    for (int i = 0; buffer[i] != '\0'; ++i) {
        if (buffer[i] == '+') {
            pocet_opakovani += 3;
        }
    }
    printf("opakovani %d\n", pocet_opakovani);

    x = porovnanie(buffer[0]);
    if (x == 10) {
        printf("hraci skonicili\n");
        return 2;
    }
    for (int i = 0; i < pocet_opakovani; i += 3) {

        printf("pocet opakocani %d a i %i\n", pocet_opakovani, i);
        y = atoi(&buffer[i + 1]);
        x = porovnanie(buffer[i]);

        if (x == 8 || y < 0 || y > 7) {
            printf("nespravna sprava\n");
            return 1;
        }
        printf("starax:%d  rozhodca x:%d staraY:%d  rozhodca y:%d\n", staraX, x, staraY, y);

        c = mat[staraX][staraY];
        if ((signal % 2 == 1 && c == 'X') || (signal % 2 == 0 && c == '0')) {
            if (pocet_opakovani == 3) {
                return_kontrola = kontrola_damy(mat, staraX, staraY, x, y, c);
                if (return_kontrola == 1) {
                    printf("chyba 1\n");
                    return 1;
                } else if (return_kontrola == 0) {
                    return 0;
                }
            }
            //zistujeme ktorym smerom sa chce hrac pohnut
            printf("c1:%c\n", c);
            return_kontrola = kontrola_damy3(mat, &staraX, &staraY, x, y, c);
            if (return_kontrola == 1) {
                printf("chyba 3\n");
                return 1;
            } else if (return_kontrola == 0) {
                (*pocet_x)--;
                *flag = true;
                printf("vymazal som x\n");
            } else if (return_kontrola == 2) {
                (*pocet_o)--;
                *flag = true;
                printf("vymazal som o\n");
            }
            return 0;
        } else if ((signal % 2 == 1 && staraX < x) || (signal % 2 == 0 && staraX > x)) {
            printf("tah do zlej strany\n");
            return 1;
            //kontrola o pohyb jednym polickom
        } else if ((pocet_opakovani == 3 && signal % 2 == 1 && staraX - 1 == x && staraY - 1 == y) ||
                   (pocet_opakovani == 3 && signal % 2 == 0 && staraX + 1 == x && staraY - 1 == y)) {
            if (mat[x][y] == ' ') {
                printf("hrac sa pohol o jednu poziciu1\n");
                mat[staraX][staraY] = ' ';
                if (signal % 2 == 1) {
                    if (x == 0) {
                        mat[x][y] = 'X';
                    } else {
                        mat[x][y] = 'x';
                    }
                } else {
                    if (x == 7) {
                        mat[x][y] = '0';
                    } else {
                        mat[x][y] = 'o';;
                    }
                }
                return 0;
            } else {
                printf("zly tah2\n");
                return 1;
            }
        } else if ((pocet_opakovani == 3 && signal % 2 == 1 && staraX - 1 == x && staraY + 1 == y) ||
                   (pocet_opakovani == 3 && signal % 2 == 0 && staraX + 1 == x && staraY + 1 == y)) {
            if (mat[x][y] == ' ') {
                printf("hrac sa pohol o jednu poziciu2\n");
                mat[staraX][staraY] = ' ';
                if (signal % 2 == 1) {
                    if (x == 0) {
                        mat[x][y] = 'X';
                    } else {
                        mat[x][y] = 'x';
                    }
                } else {
                    if (x == 7) {
                        mat[x][y] = '0';
                    } else {
                        mat[x][y] = 'o';;
                    }
                }
                return 0;
            } else {
                printf("zly tah4\n");
                return 1;
            }
        }
        //zistujeme ktorym smerom sa chce hrac pohnut
        if (staraY > y) {
            printf("dolava\n");
            //zistujeme ci v v okoli hraca je super ktoreho moze preskocit
            if ((signal % 2 == 1 && mat[staraX - 1][staraY - 1] == 'o') ||
                (signal % 2 == 1 && mat[staraX - 1][staraY - 1] == '0') ||
                (signal % 2 == 0 && mat[staraX + 1][staraY - 1] == 'x') ||
                (signal % 2 == 0 && mat[staraX + 1][staraY - 1] == 'X')) {
                if ((mat[x][y] == ' ' && signal % 2 == 1 && x == staraX - 2 && y == staraY - 2) ||
                    (mat[x][y] == ' ' && signal % 2 == 0 && x == staraX + 2 && y == staraY - 2)) {

                    printf("spravny tah preskocil si supera1\n");
                    *flag = true;
                    mat[staraX][staraY] = ' ';

                    if (signal % 2 == 1) {
                        if (x == 0) {
                            mat[x][y] = 'X';
                        } else {
                            mat[x][y] = 'x';
                        }
                        (*pocet_o)--;
                        printf("vymazal som o\n");
                        mat[staraX - 1][staraY - 1] = ' ';
                    } else {
                        if (x == 7) {
                            mat[x][y] = '0';
                        } else {
                            mat[x][y] = 'o';
                        }
                        (*pocet_x)--;
                        printf("vymazal som x\n");
                        mat[staraX + 1][staraY - 1] = ' ';
                    }
                    staraX = x;
                    staraY = y;
                } else {
                    printf("zly tah7\n");
                    return 1;
                }
            } else {
                printf("zly tahh12\n");
                return 1;
            }
        } else if (staraY < y) {
            printf("doprava\n");
            if ((signal % 2 == 1 && mat[staraX - 1][staraY + 1] == 'o') ||
                (signal % 2 == 1 && mat[staraX - 1][staraY + 1] == '0') ||
                (signal % 2 == 0 && mat[staraX + 1][staraY + 1] == 'x') ||
                (signal % 2 == 0 && mat[staraX + 1][staraY + 1] == 'X')) {
                if ((mat[x][y] == ' ' && signal % 2 == 1 && x == staraX - 2 && y == staraY + 2) ||
                    (mat[x][y] == ' ' && signal % 2 == 0 && x == staraX + 2 && y == staraY + 2)) {
                    printf("spravny tah preskocil si supera2\n");
                    *flag = true;
                    mat[staraX][staraY] = ' ';
                    if (signal % 2 == 1) {
                        if (x == 0 || mat[staraX][staraY] == 'X') {
                            mat[x][y] = 'X';
                            printf("davam velke X\n");
                        } else {
                            mat[x][y] = 'x';
                        }
                        (*pocet_o)--;
                        printf("vymazal som o\n");
                        mat[staraX - 1][staraY + 1] = ' ';
                    } else {
                        if (x == 7 || mat[staraX][staraY] == '0') {
                            mat[x][y] = '0';
                        } else {
                            mat[x][y] = 'o';
                        }
                        (*pocet_x)--;
                        printf("vymazal som x\n");
                        mat[staraX + 1][staraY + 1] = ' ';
                    }
                    staraX = x;
                    staraY = y;
                } else {
                    printf("zly tah5\n");
                    return 1;
                }
            } else {
                printf("zly tah0\n");
                return 1;
            }
        } else {
            printf("zly tah\n");
            return 1;
        }
    }

        printf("rozhodca vyhodnitil spravnu tah panacika20\n");

        return 0;

        printf("koniec rozhodca\n");

}

int kontrola_damy4(char**mat,int *staraX,int *staraY,int x2,int y2,char c,int n) {
    printf("som tu8\n");
    if (mat[x2][y2] == ' ') {
        printf("spravny tah preskocil si supera3\n");
        mat[*staraX][*staraY] = ' ';

        if (n==0) {
            mat[x2][y2] = c;
            mat[*staraX - 1][*staraY - 1] = ' ';
        } else if(n==1){
            mat[x2][y2] = c;
            mat[*staraX - 1][*staraY + 1] = ' ';
        } else if(n==2){
            mat[x2][y2] = c;
            mat[*staraX + 1][*staraY - 1] = ' ';
        }else {
            mat[x2][y2] = c;
            mat[*staraX + 1][*staraY + 1] = ' ';
        }

        *staraX = x2;
        *staraY = y2;
        if(c=='X'){
            return 2;
        }else{
            return 0;
        }

    } else {
        printf("zly tah0\n");
        return 1;
    }
}
int kontrola_damy3(char**mat,int *staraX,int *staraY,int x2,int y2,char c) {
    int n;
        if ((c == 'X' && mat[*staraX - 1][*staraY - 1] == 'o') ||(c == '0' && mat[*staraX - 1][*staraY - 1] == 'x')){
            if(x2==*staraX - 2 && y2 == *staraY - 2) {
                printf("som tu10\n");
                n=0;
                return kontrola_damy4(mat, staraX, staraY, x2, y2, c,n);
            }
        }

        if ((c == 'X' && mat[*staraX - 1][*staraY + 1] == 'o') || (c == '0' && mat[*staraX - 1][*staraY + 1] == 'x')) {
            if (x2 == *staraX - 2 && y2 == *staraY + 2) {
                printf("som tu11\n");
                n = 1;
                return kontrola_damy4(mat, staraX, staraY, x2, y2, c, n);
            }
        }

        if ((c == 'X' && mat[*staraX + 1][*staraY - 1] == 'o') || (c == '0' && mat[*staraX + 1][*staraY - 1] == 'x')) {
            if (x2 == *staraX + 2 && y2 == *staraY + 2) {
                printf("som tu12\n");
                n = 2;
                return kontrola_damy4(mat, staraX, staraY, x2, y2, c, n);
            }
        }

        if ((c == 'X' && mat[*staraX + 1][*staraY + 1] == 'o') || (c == '0' && mat[*staraX + 1][*staraY + 1] == 'x')) {
            if (x2 == *staraX + 2 && y2 == *staraY + 2) {
                printf("som tu13\n");
                n = 3;
                return kontrola_damy4(mat, staraX, staraY, x2, y2, c, n);
            }
        }

    printf("zly tah10\n");
    return 1;
}
int kontrola_damy2(char**mat,int staraX,int staraY,int x2,int y2,char c){
    if (mat[x2][y2] == ' ') {
        printf("som tu23\n");
        printf("hrac sa pohol o jednu poziciu3\n");
        mat[staraX][staraY] = ' ';
        if (c=='X') {
            mat[x2][y2] = c;
        } else {
            mat[x2][y2] = c;
        }
        return 0;
    } else {
        printf("zly tah8\n");
        return 1;
    }
}
int kontrola_damy(char**mat,int staraX,int staraY,int x2,int y2,char c){
    if(staraX-1 == x2 && staraY-1 == y2 ) {
        printf("som tu1\n");
        return kontrola_damy2(mat,staraX,staraY,x2,y2,c);
    }
    if(staraX-1 == x2 && staraY+1 == y2 ) {
        printf("som tu2\n");
        return kontrola_damy2(mat,staraX,staraY,x2,y2,c);
    }
    if(staraX+1 == x2 && staraY-1 == y2 ) {
        printf("som tu3\n");
        return kontrola_damy2(mat,staraX,staraY,x2,y2,c);
    }
    if(staraX+1 == x2 && staraY+1 == y2 ) {
        printf("som tu4\n");
        return kontrola_damy2(mat,staraX,staraY,x2,y2,c);
    }
    printf("som tu5\n");
    return 3;
}



int server(int argc, char *argv[]){

    int newsockfd ,sockfd_S;
    socklen_t cli_len;
    struct sockaddr_in serv_addr, cli_addr;


    char **matrix;
    matrix=alokujMatrix();




    if (argc < 2)
    {
        fprintf(stderr,"usage %s port\n", argv[0]);
        return 1;
    }

    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(argv[1]));

    sockfd_S = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_S < 0)
    {
        perror("Error creating socket");
        return 1;
    }

    if (bind(sockfd_S, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Error binding socket address");
        return 2;
    }

    listen(sockfd_S, 5);
    cli_len = sizeof(cli_addr);

    newsockfd = accept(sockfd_S, (struct sockaddr *) &cli_addr, &cli_len);
    if (newsockfd < 0) {
        perror("ERROR on accept");
        return 3;
    } else {
        printf("connect S\n");
    }

        int pocet_hracov = 4;
        int x = 8, o = 8;
        int S_X=0,S_Y=0;
        int signal=1;
        int *databaza_signal = calloc(5,sizeof(int));
        int *databaza_typ = calloc(5,sizeof(int));
        int *Xka = calloc(5,sizeof(int));
        int *Ocka = calloc(5,sizeof(int));
        int index=0,cas=0,cakanie=0;
        pthread_t server_vlakna[pocet_hracov],databaza_valkno;
        pthread_mutex_t tah_mutex,databaza_mutex;
        pthread_cond_t databaza_caka,server_caka;
        pthread_mutex_init(&tah_mutex,NULL);
        pthread_mutex_init(&databaza_mutex,NULL);
        pthread_cond_init(&server_caka,NULL);
        pthread_cond_init(&databaza_caka,NULL);

        SPOL_DATA spol_data = {matrix,&x, &o,signal,&S_X,&S_Y,databaza_signal,databaza_typ,&index,cakanie,pocet_hracov,&databaza_caka,&server_caka,&databaza_mutex,&tah_mutex};
        SERVER server_data[pocet_hracov];
        DATABAZA databaza_data={cas,Xka,Ocka,&spol_data};

        pthread_create(&databaza_valkno, NULL, databaza_fun, &databaza_data);
        for (int i = 0; i < pocet_hracov; ++i) {
            server_data[i].hrac_id=i+1;
            server_data[i].argv=argv[1];
            server_data[i].newsockfd= newsockfd;
            server_data[i].spol_data = &spol_data;
            pthread_create(&server_vlakna[i], NULL, server_fun, &server_data[i]);
        }

        for (int i = 0; i < pocet_hracov; i++) {
            pthread_join(server_vlakna[i], NULL);
        }
        pthread_join(databaza_valkno, NULL);

        printf("koniec end\n");
        free_matrix(matrix);
        close(sockfd_S);
        free(databaza_signal);
        free(databaza_typ);
        free(Xka);
        free(Ocka);
        pthread_mutex_destroy(&databaza_mutex);
        pthread_mutex_destroy(&tah_mutex);
        pthread_cond_destroy(&databaza_caka);
        pthread_cond_destroy(&server_caka);

    return 0;
}