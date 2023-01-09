void * databaza_fun(void * parameters){
    DATABAZA * databaza = (DATABAZA*)parameters;
    int signal;
    int typ;
    bool koniec=false;
    while(1){

        pthread_mutex_lock(databaza->spol_data->databaza_mutex);
        while(*databaza->spol_data->index==0){
            printf("databaza caka index:%d\n",*databaza->spol_data->index);
            pthread_cond_wait(databaza->spol_data->databaza_Caka,databaza->spol_data->databaza_mutex);
        }
        signal = databaza->spol_data->databaza_signal[*databaza->spol_data->index-1];
        typ= databaza->spol_data->databaza_typ[*databaza->spol_data->index-1];
        printf("index cislo:%d\n", *databaza->spol_data->index);
        (*databaza->spol_data->index)--;
        pthread_cond_signal(databaza->spol_data->server_caka);
        pthread_mutex_unlock(databaza->spol_data->databaza_mutex);

        printf("databaza si zobrala signal: %d typu %d\n",signal,typ);

        if(typ==3 ||typ==1){
            signal %2==1 ? (databaza->Xka[0])++:(databaza->Ocka[0])++;
            printf("nespravnyy typ\n");
        }else if(typ==0){
            signal %2==1 ? (databaza->Xka[1])++:(databaza->Ocka[1])++;
            printf("spravny typ\n");
            databaza->cas=3+rand()%3;
        }else if(typ==4){
            signal %2==1 ? (databaza->Xka[2])++:(databaza->Ocka[2])++;
            printf("polovicny typ\n");
            databaza->cas=1+rand()%3;
        }
        if(typ==2 || koniec==true){
            koniec=true;
            pthread_mutex_lock(databaza->spol_data->databaza_mutex);
            if(*databaza->spol_data->index==0){
                pthread_mutex_unlock(databaza->spol_data->databaza_mutex);
                printf("koniec je tru\n");
                break;
            }
            pthread_mutex_unlock(databaza->spol_data->databaza_mutex);
            printf("endik\n");
        }
        pthread_mutex_unlock(databaza->spol_data->databaza_mutex);
        sleep(databaza->cas);
        printf("databaza vyhodnotila tah za %d\n",databaza->cas);
    }
    printf("Xka spravne:%d, nespravne:%d, polovicne %d\n",databaza->Xka[1],databaza->Xka[0],databaza->Xka[2]);
    printf("Ocka spravne:%d, nespravne:%d, polovicne %d\n",databaza->Ocka[1],databaza->Ocka[0],databaza->Ocka[2]);
    return NULL;
}