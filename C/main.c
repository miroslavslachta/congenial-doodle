#include "../H/server.h"
#include "../H/client.h"
#include <string.h>
#include <stdio.h>


//main
int main(int argc,char*argv[]){
    if (strcmp(argv[1],"server")==0){
        server(argc-1,argv+1);
    }else if(strcmp(argv[1],"client")==0){
        client(argc-1,argv+1);
    }else{
        fprintf(stderr,"wrong arguments\n");
        return 1;
    }

    return 0;
}
