#include <stdio.h>
#include <math.h>
#include "lcgrand.h"
#define Q_LIMIT 100  /* Limit on queue length. */
#define BUSY      1  /* Mnemonics for server's being busy */
#define IDLE      0  /* and idle. */

int   next_event_type, num_custs_delayedA,num_custs_delayedB,num_custs_delayedC,
      num_events,num_in_qA,num_in_qB,num_in_qC,server_statusA,server_statusB,
      server_statusC, max_trabajos,num_clientes,num_clientesC;
float area_num_in_qA,area_num_in_qB,area_num_in_qC, area_server_statusA,area_server_statusB,area_server_statusC,
      mean_interarrival, mean_service, sim_time, time_arrivalA[Q_LIMIT + 1],time_arrivalB[Q_LIMIT + 1],
      time_arrivalC[Q_LIMIT + 1], time_last_event, time_next_event[5],total_of_delaysA,total_of_delaysB,
      total_of_delaysC;

FILE  *infile, *outfile;

void  initialize(int);
void  timing(void);
void  arrive(int);
void  depart(void);
void  departB(int);
void  departC(int);
void  arr_dep(int);
void  report(void);
void  update_time_avg_stats(void);
float expon(float mean);
float uniform(float a, float b,int lc);

int main(){
    max_trabajos = 60;
    int i = 1;
    int suma = 0;
    float promedio = 0.0;
    printf("Grupo ModSim_307\n");
    printf("PUNTO 1.9\n");
    while(i<16){
        printf("ITERACION %d\n",i);
        suma = suma + simulacionmain(i, max_trabajos);
        i++;
    }
    promedio = suma / (i-1);
    printf("\nEl numero promedio de trabajos que fueron atendidos por el\nempleado C en %d iteraciones es: %.3f de %d clientes\n",i-1,promedio,max_trabajos);
}

int simulacionmain(int lc, int max_trabajos){
    num_clientes=0;
    num_clientesC=0;
    int m = 0;
    num_events = 4;
    initialize(lc);
    while (num_clientes < max_trabajos)
    {
        timing();
        update_time_avg_stats();
        switch (next_event_type)
        {
            case 1:
                arrive(lc);
                break;
            case 2:
                departB(lc);
                break;
            case 3:
                departC(lc);
                break;
            case 4:
                arr_dep(lc);
                break;
        }
    }
    printf("El numero de procesos que fueron antendidos por el\nempleado C es: %d de %d \n",num_clientesC,num_clientes);
    return num_clientesC;
}


void initialize(int lc){
    sim_time = 0.0;
    server_statusA   = IDLE;
    server_statusB   = IDLE;
    server_statusC   = IDLE;
    num_in_qA        = 0;
    num_in_qB        = 0;
    num_in_qC        = 0;
    time_last_event = 0.0;
    //DATOS
    num_custs_delayedA  = 0;
    num_custs_delayedB  = 0;
    num_custs_delayedC  = 0;
    total_of_delaysA    = 0.0;
    total_of_delaysB    = 0.0;
    total_of_delaysC    = 0.0;
    area_num_in_qA      = 0.0;
    area_num_in_qB      = 0.0;
    area_num_in_qC      = 0.0;
    area_server_statusA = 0.0;
    area_server_statusB = 0.0;
    area_server_statusC = 0.0;


    time_next_event[1] = sim_time + uniform(15,5,lc);
    time_next_event[2] = 1.0e+30;
    time_next_event[3] = 1.0e+30;
    time_next_event[4] = 1.0e+30;
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
        fprintf(outfile, "\nEvent list empty at time %f", sim_time);
        exit(1);
    }
    sim_time = min_time_next_event;
}


void arrive(int lc){
    ++ num_clientes;
    float delay;
    time_next_event[1] = sim_time + uniform(15,5,lc);
    if (server_statusB == BUSY){
            //C
            ++ num_clientesC;
            if (server_statusC == BUSY){
                ++num_in_qC;
                if (num_in_qC > Q_LIMIT){
                        fprintf(outfile, "\nOverflow of the array time_arrival at");
                        fprintf(outfile, " time %f", sim_time);
                        exit(2);
                }
            time_arrivalC[num_in_qC] = sim_time;
            }else{
                delay = 0.0;
                total_of_delaysC += delay;
                ++num_custs_delayedC;
                server_statusC = BUSY;
                time_next_event[3] = sim_time + uniform(20,10,lc);
            }
    }else{
        //A
        if (server_statusA == BUSY){
                ++num_in_qA;
                if (num_in_qA > Q_LIMIT){
                        fprintf(outfile, "\nOverflow of the array time_arrival at");
                        fprintf(outfile, " time %f", sim_time);
                        exit(2);
                }
            time_arrivalA[num_in_qA] = sim_time;
            }else{
                delay = 0.0;
                total_of_delaysA += delay;
                ++num_custs_delayedA;
                server_statusA = BUSY;
                time_next_event[4] = sim_time + uniform(10,3,lc);
            }

    }
}


void departB(int lc){
    int   i;
    float delayB;
    if (num_in_qB == 0){
        server_statusB = IDLE;
        time_next_event[2] = 1.0e+30;
    }
    else{
        --num_in_qB;
        delayB = sim_time - time_arrivalB[1];
        total_of_delaysB += delayB;
        ++num_custs_delayedB;
        time_next_event[2] = sim_time + uniform(5,2,lc);
        for (i = 1; i <= num_in_qB; ++i)
            time_arrivalB[i] = time_arrivalB[i + 1];
    }
}

void departC(int lc){
    int   i;
    float delayC;
    if (num_in_qC == 0){
        server_statusC = IDLE;
        time_next_event[3] = 1.0e+30;
    }
    else{
        --num_in_qC;
        delayC = sim_time - time_arrivalC[1];
        total_of_delaysC += delayC;
        ++num_custs_delayedC;
        time_next_event[3] = sim_time + uniform(20,10,lc);
        for (i = 1; i <= num_in_qC; ++i)
            time_arrivalC[i] = time_arrivalC[i + 1];
    }
}

void arr_dep(int lc){
    int   i;
    float delayA;
    if (num_in_qA == 0){
        server_statusA = IDLE;
        time_next_event[4] = 1.0e+30;
    }
    else{
        --num_in_qA;
        delayA = sim_time - time_arrivalA[1];
        total_of_delaysA += delayA;
        ++num_custs_delayedA;
        time_next_event[4] = sim_time + uniform(10,3,lc);
        for (i = 1; i <= num_in_qA; ++i)
            time_arrivalA[i] = time_arrivalA[i + 1];
    }

    float delayB;

    if (server_statusB == BUSY){
        ++num_in_qB;
        if (num_in_qB > Q_LIMIT){
            fprintf(outfile, "\nOverflow of the array time_arrival at");
            fprintf(outfile, " time %f", sim_time);
            exit(2);
        }
        time_arrivalB[num_in_qB] = sim_time;
    }
    else
    {
        delayB = 0.0;
        total_of_delaysB += delayB;
        ++num_custs_delayedB;
        server_statusB = BUSY;
        time_next_event[2] = sim_time + uniform(5,2,lc);
    }
}

void update_time_avg_stats(void){
    float time_since_last_event;

    time_since_last_event = sim_time - time_last_event;
    time_last_event       = sim_time;

    area_num_in_qA += num_in_qA * time_since_last_event;
    area_num_in_qB += num_in_qB * time_since_last_event;
    area_num_in_qC += num_in_qC * time_since_last_event;

    area_server_statusA += server_statusA * time_since_last_event;
    area_server_statusB += server_statusB * time_since_last_event;
    area_server_statusC += server_statusC * time_since_last_event;

}

float uniform(float a, float b,int lc){
    return (a-b) + lcgrand(lc) * ((a+b) - (a-b));
}
