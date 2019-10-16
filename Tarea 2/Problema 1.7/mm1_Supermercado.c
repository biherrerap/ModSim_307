
#include <stdio.h>
#include <math.h>
#include "lcgrand.h"
#define Q_LIMIT 100
#define BUSY      1
#define IDLE      0

int   next_event_type, num_custs_delayed, num_events, num_in_q, server_status, num_cliente,server_status_arr[Q_LIMIT+1];
float area_num_in_q, area_server_status, mean_interarrival, mean_service,
      sim_time, time_arrival[Q_LIMIT + 1],time_depart, time_last_event, time_next_event[3],
      total_of_delays,total_of_service, time_exit;
float num_services[Q_LIMIT+1];

void  initialize(int);
void  timing(void);
void  arrive(int);
void  depart(int);
void  arrive1(int,int);
void  depart1(int,int);
void  report(void);
void  update_time_avg_stats(void);
float expon(float,int);
float poison(float,int);
float* simulacionmain1(int);
float simulacionmain2(int,float);
int simulacionmain3(int,float);
main(){
    mean_interarrival = 3.0;
    mean_service = 4.0;
    time_exit = 100;
    printf("Problema 1.7\n");
    printf("1)\n");
    printf("Sistema de un supermercado\n\n");
    printf("Media de tiempo entre llegadas %.3f minutos\n\n",mean_interarrival);
    printf("Media de tiempo de servicio %.3f minutos\n\n",mean_service);
    printf("Limite de tiempo de la simulacion %.3f\n\n",time_exit);
    int i;
    float promedio = 0.0;
    float datos[3];
    datos[0] = 0;
    datos[1] = 0;
    datos[2] = 0;
    for (i = 1; i<16;i++){
        printf("\nITERACION %d\n",i);
        float* resultados = simulacionmain1(i);
        datos[0] = datos[0]+resultados[0];
        datos[1] = datos[1]+resultados[1];
        datos[2] = datos[2]+resultados[2];
    }
    promedio = datos[0] / i-1;
    printf("\nEn %d iteraciones\n",i-1);
    printf("El numero promedio de la demora de un cliente en la cola es %.3f\n",promedio);
    printf("El numero promedio de clientes en la cola es %.3f\n",datos[1]/i-1);
    printf("El numero promedio de la demora de un cliente en el sistema de servicio %.3f\n",datos[2]/i-1);
    printf("\n--------------\n2)\n");
    printf("\nLa demora promedio calculada es de: %f\n",promedio/2);
    printf("Se toma un error inferior a 0.0001\n");
    float promedio2 = 0.0;
    for (i = 1; i<16;i++){
        printf("\nITERACION %d\n",i);
        promedio2 = promedio2 + simulacionmain2(i,promedio/2);
    }
    printf("\nEl numero promedio de tiempo de servicio para tener un\n");
    printf("tiempo de demora en la cola de %.3f en %d iteraciones\n",promedio/2,i-1);
    printf("es de %.3f minutos\n",promedio2/i-1);
    printf("\n--------------\n3)\n");
    printf("\nLa demora promedio calculada es de: %f\n",promedio/3);
    printf("Se considera un numero igual o menor al promedio\n");
    int promedio3 = 0;
    for (i = 1; i<16;i++){
        printf("\nITERACION %d\n",i);
        promedio3 = promedio3 + simulacionmain3(i,promedio/3);
    }
    printf("\nEl numero promedio de cajas registradoras en servicio para tener un tiempo\n");
    printf("de demora en la cola de %.3f en %d iteraciones es de %d\n",promedio/3,i-1,promedio3/i-1);
}
float* simulacionmain1(int lc){
    num_events = 2;
    initialize(lc);
    time_depart=0;
    while (sim_time < time_exit)
    {
        timing();
        if(sim_time < time_exit){
            update_time_avg_stats();
            switch (next_event_type)
            {
                case 1:
                    arrive(lc);
                    break;
                case 2:
                    depart(lc);
                    break;
            }
        }
    }
    printf("\nDemora promedio de un cliente en la cola %.3f minutos\n",
            total_of_delays / num_custs_delayed);
    printf("Numero promedio de clientes en la cola %10.3f\n",
            area_num_in_q / time_exit);
    printf("Demora promedio de un cliente en el sistema de servicio %.3f minutos\n",area_server_status/time_depart);
    float datos[3];
    datos[0] =  total_of_delays / num_custs_delayed;
    datos[1] =  area_num_in_q / time_exit;
    datos[2] =  area_server_status/time_depart;
    return datos;
}
float simulacionmain2(int lc, float promedio){
    int terminar = 0;
    float dem_prom;
    mean_service = mean_service/2;
    while(terminar==0){
    num_events = 2;
    initialize(lc);
    while (sim_time < time_exit)
    {
        timing();
        if(sim_time < time_exit){
            update_time_avg_stats();
            switch (next_event_type)
            {
                case 1:
                    arrive(lc);
                    break;
                case 2:
                    depart(lc);
                    break;
            }
        }
    }
    dem_prom = total_of_delays / num_custs_delayed;
    float diference = promedio-dem_prom;
    if (fabs(diference)<0.0001){
        terminar = 1;
    }else{
        if (dem_prom>promedio){
            mean_service = mean_service/2;
        }else{
            mean_service = mean_service+ mean_service/2;
        }
    }
    }
    printf("Demora promedio de un cliente en el sistema de servicio: %.3f minutos con un tiempo de servicio de: %.3f\n",dem_prom,mean_service);
    return mean_service;
}
int simulacionmain3(int lc, float promedio){
    int terminar = 0;
    float dem_prom;
    int num_caja = 2;
    while(terminar==0){
    num_events = 2;
    initialize1(lc,num_caja);
    while (sim_time < time_exit)
    {
        timing();
        if(sim_time < time_exit){
            update_time_avg_stats();
            switch (next_event_type)
            {
                case 1:
                    arrive1(lc,num_caja);
                    int j =0;
                    break;
                case 2:
                    depart1(lc,num_caja);
                    break;
            }
        }
    }
    dem_prom = total_of_delays / num_custs_delayed;
    float diference = dem_prom-promedio;
    if (diference<=0){
        terminar = 1;
    }else{
        num_caja++;
    }
    }
    printf("Demora promedio de un cliente en el sistema de servicio: %.3f minutos se necesitan %d cajas registradoras\n",dem_prom,num_caja);
    return num_caja;
}
void initialize(int lc)
{
    sim_time = 0.0;

    server_status   = IDLE;
    num_in_q        = 0;
    time_last_event = 0.0;
    num_cliente = 0;

    num_custs_delayed  = 0;
    total_of_delays    = 0.0;
    area_num_in_q      = 0.0;
    area_server_status = 0.0;

    time_next_event[1] = sim_time + expon(mean_interarrival,lc);
    time_next_event[2] = 1.0e+30;
}
void initialize1(int lc, int n)
{
    sim_time = 0.0;
    int i;
    for(i=0;i<n;i++){
        server_status_arr[i] = IDLE;
    }
    num_in_q        = 0;
    time_last_event = 0.0;
    num_cliente = 0;

    num_custs_delayed  = 0;
    total_of_delays    = 0.0;
    area_num_in_q      = 0.0;
    area_server_status = 0.0;
    for(i=0;i<n;i++){
        num_services[i] = 1.0e+30;
    }
    time_next_event[1] = sim_time + expon(mean_interarrival,lc);
    time_next_event[2] = 1.0e+30;
}
void timing(void)
{
    int   i;
    float min_time_next_event = 1.0e+29;
    next_event_type = 0;
    for (i = 1; i <= num_events; ++i)
        if (time_next_event[i] < min_time_next_event)
        {
            min_time_next_event = time_next_event[i];
            next_event_type     = i;
        }
    if (next_event_type == 0)
    {
        printf("\nLista de eventos vacía %f", sim_time);
        exit(1);
    }
    sim_time = min_time_next_event;
}

void arrive(int lc)
{
    float delay;
    time_next_event[1] = sim_time + expon(mean_interarrival,lc);
    ++ num_cliente;
    if (server_status == BUSY)
    {

        ++num_in_q;
        if (num_in_q > Q_LIMIT)
        {
            printf("\n Limite de la cola alcanzado");
            printf(" time %f", sim_time);
            exit(2);
        }
        time_arrival[num_in_q] = sim_time;
    }
    else
    {
        delay            = 0.0;
        total_of_delays += delay;
        ++num_custs_delayed;
        server_status = BUSY;
        time_next_event[2] = sim_time + poison(mean_service,lc);
    }
}

void arrive1(int lc,int n)
{
    float delay;
    time_next_event[1] = sim_time + expon(mean_interarrival,lc);
    ++ num_cliente;
    int i = 0;
    int a = 0;
    while(i<n){
    if (server_status_arr[i]!= BUSY){
        delay            = 0.0;
        total_of_delays += delay;
        ++num_custs_delayed;
        server_status_arr[i] = BUSY;
        time_next_event[2] = sim_time + poison(mean_service,lc);
        num_services[i] = time_next_event[2];
        i = n;
        a = 1;

    }
    i++;
    }
    if (a==0){
        ++num_in_q;
        if (num_in_q > Q_LIMIT)
        {
            printf("\n Limite de la cola alcanzado");
            printf(" time %f", sim_time);
            exit(2);
        }
        time_arrival[num_in_q] = sim_time;
    }
}
void depart(int lc){
    int   i;
    float delay;
    ++time_depart;
    if (num_in_q == 0)
    {
        server_status      = IDLE;
        time_next_event[2] = 1.0e+30;
    }

    else
    {
        --num_in_q;
        delay            = sim_time - time_arrival[1];
        total_of_delays += delay;
        ++num_custs_delayed;
        time_next_event[2] = sim_time + poison(mean_service,lc);
        for (i = 1; i <= num_in_q; ++i)
            time_arrival[i] = time_arrival[i + 1];
    }
}
void depart1(int lc,int n){
    int   i;
    float delay;
    ++time_depart;
    float time = 1.0e+30;
    int caja = 0;
    for (i = 0; i <n; ++i){
        if (time>num_services[i]){
           caja = i;
        }
    }
    if (num_in_q == 0)
    {

        server_status_arr[caja] = IDLE;
        num_services[caja] = 1.0e+30;
        time_next_event[2] = 1.0e+30;
    }

    else
    {
        --num_in_q;
        delay            = sim_time - time_arrival[1];
        total_of_delays += delay;
        ++num_custs_delayed;
        time_next_event[2] = sim_time + poison(mean_service,lc);
        num_services[caja] = time_next_event[2];
        for (i = 1; i <= num_in_q; ++i)
            time_arrival[i] = time_arrival[i + 1];
    }
}
void report(void)
{
    printf("\nDemora promedio de un cliente en la cola %.3f minutos\n",
            total_of_delays / num_custs_delayed);
    printf("Numero promedio de clientes en la cola %10.3f\n",
            area_num_in_q / time_exit);
    printf("Demora promedio de un cliente en el sistema de servicio: %.3f minutos\n",area_server_status/time_depart);
}
void update_time_avg_stats(void)
{
    float time_since_last_event;
    time_since_last_event = sim_time - time_last_event;
    time_last_event = sim_time;
    area_num_in_q += num_in_q * time_since_last_event;
    area_server_status += server_status * time_since_last_event;
}
float expon(float mean,int lc)
{
    return -mean * log(lcgrand(1));
}
float poison(float mean,int lc)
{
    return -mean * log(lcgrand(lc));
}

