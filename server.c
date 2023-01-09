void * server_fun(void *parameters) {

    printf("hrac cislo(%d)\n", server->hrac_id);
    while (*server->spol_data->pocet_o != 0 && *server->spol_data->pocet_x != 0 ) {

        pthread_mutex_lock(server->spol_data->tah_mutex);
        if ((server->spol_data->signal == server->hrac_id )&& (*server->spol_data->pocet_o) != 0 && (*server->spol_data->pocet_x != 0)) {
            pthread_mutex_unlock(server->spol_data->tah_mutex);

            if(rozhodnutie ==2 ) {
                pthread_mutex_lock(server->spol_data->databaza_mutex);
                printf("typ %d signal %d,\n",rozhodnutie,server->spol_data->signal);
                while(*server->spol_data->index>5){
                    printf("server caka index:%d\n",*server->spol_data->index);
                    pthread_cond_wait(server->spol_data->server_caka,server->spol_data->databaza_mutex);
                }
                server->spol_data->databaza_signal[*server->spol_data->index]= server->spol_data->signal;
                printf("posielam rozhdonutie:%d\n",rozhodnutie);
                server->spol_data->databaza_typ[*server->spol_data->index]=rozhodnutie;
                (*server->spol_data->index)++;
                printf("index:%d\n",(*server->spol_data->index));
                pthread_cond_signal(server->spol_data->databaza_Caka);
                pthread_mutex_unlock(server->spol_data->databaza_mutex);

                int temp;
                if (flag== true && rozhodnutie==1){
                    temp=4;
                }else {
                    temp = rozhodnutie;
                }
                pthread_mutex_lock(server->spol_data->databaza_mutex);
                while(*server->spol_data->index>5){
                    printf("server caka index:%d\n",*server->spol_data->index);
                    pthread_cond_wait(server->spol_data->server_caka,server->spol_data->databaza_mutex);
                }
                printf("typ %d signal %d,\n",temp,server->spol_data->signal);
                server->spol_data->databaza_signal[*server->spol_data->index]= server->spol_data->signal;
                server->spol_data->databaza_typ[*server->spol_data->index]=temp;
                (*server->spol_data->index)++;
                printf("index:%d\n",(*server->spol_data->index));
                pthread_cond_signal(server->spol_data->databaza_Caka);
                pthread_mutex_unlock(server->spol_data->databaza_mutex);



                pthread_mutex_lock(server->spol_data->tah_mutex);
            }
            pthread_mutex_unlock(server->spol_data->tah_mutex);
            if (server->spol_data->signal == server->spol_data->pocet_hracov+1) { server->spol_data->signal = 1; }
        }
        server->spol_data->databaza_signal[*server->spol_data->index] = 0;
        server->spol_data->databaza_typ[*server->spol_data->index] = 2;
        (*server->spol_data->index)++;
        pthread_cond_signal(server->spol_data->databaza_Caka);
        pthread_mutex_unlock(server->spol_data->databaza_mutex);
    }
}