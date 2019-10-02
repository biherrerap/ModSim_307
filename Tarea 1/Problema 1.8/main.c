/* External definitions for single-server queueing system. */

#include <stdio.h>
#include <math.h>
#include "lcgrand.h"  /* Header file for random-number generator. */

#define Q_LIMIT 100000  /* Limit on queue length. */
#define Q1_LIMIT 10
#define BUSY      1  /* Mnemonics for server's being busy */
#define IDLE      0  /* and idle. */

int   type_next_event, num_custs_delayedq1, num_custs_delayedq2, number_events,
      num_q_1, num_q_2, status_server1, status_server2, limit_time;
double area_num_in_q1, area_num_in_q2, area_server_status1, area_server_status2, mean_interarrival, mean_service,
      mean_service_2, time_arrival[Q_LIMIT + 1], arrival_time2[Q_LIMIT + 1], time_last_event, time_next_event[3],time_sim,
      total_of_delays, total_of_delays_2, min_arr, max_arr, min_arr2, max_arr2;
FILE  *entrada, *salida;

void  initialize(void);
void  timing(void);
void  arrive(void);
void  depart(void);
void  arrive_2(void);
void  depart_2(void);
void  report(void);
void  update_time_avg_stats(void);
double expon(double mean);
double uniform(double a, double b);

int main(void)
{
    /* Open input and output files. */
    entrada  = fopen("server.in",  "r");
    salida = fopen("server.out", "w");
    /* Specify the number of events for the timing function. */
    number_events = 4;
    /* Read input parameters. */
    fscanf(entrada, "%f %f %d %f %f %f %f", &mean_service, &mean_service_2, &limit_time, &min_arr, &max_arr, &min_arr2, &max_arr2);
    /* Write report heading and input parameters. */
    printf("%i\n",limit_time);
    fprintf(salida, "Sistema de dos canales con su propia cola\n\n");
    fprintf(salida, "Media de servicio canal 1 %f minutes\n\n", mean_service);
    fprintf(salida, "Media de servicio canal 2 %f minutes\n\n", mean_service_2);
    fprintf(salida, "Tiempo de simulacion %d horas\n\n", limit_time);

    /* Initialize the simulation. */
    initialize();
    /* Run the simulation while more delays are still needed. */

int time_stop =limit_time*60;

    while (time_sim < time_stop )
    {
     /* Determine the next event. */
     printf("hola %f\n",time_sim);
        time_last_event = time_sim;

        timing();

        /* Update time-average statistical accumulators. */
        update_time_avg_stats();
        /* Invoke the appropriate event function. */
        switch (type_next_event)
        {
            case 1:
                arrive();
                break;
            case 2://sale cola 1
                depart();
                break;
            case 3://ingresa de cola2
                arrive_2();
                break;
            case 4://ingresa a cola 2
                depart_2();
                break;
        }
    };
    /* Invoke the report generator and end the simulation. */

    report();
    printf("hasta aqui bien");
    fclose(entrada);
    fclose(salida);
    return 0;
}


void initialize(void)  /* Initialization function. */
{
    /* Initialize the simulation clock. */
    time_sim = 0.0;
    /* Initialize the state variables. */
    status_server1   = IDLE;
    status_server1   = IDLE;
    num_q_1        = 0;
    num_q_2        = 0;
    time_last_event = 0.0;
    /* Initialize the statistical counters. */
    num_custs_delayedq1  = 0;
    num_custs_delayedq2  = 0;
    total_of_delays    = 0.0;
    area_num_in_q1      = 0.0;
    area_num_in_q2      = 0.0;
    area_server_status1 = 0.0;
    area_server_status2 = 0.0;
    /* Initialize event list.  Since no customers are present, the departure
       (service completion) event is eliminated from consideration. */
    time_next_event[1] = uniform(min_arr,max_arr);
    time_next_event[2] = 1.0e+30;
    time_next_event[3] = 1.0e+30;
    time_next_event[4] = 1.0e+30;
    time_next_event[5] = 1.0e+30;
}


void timing(void)  /* Timing function. */
{
    int   i;
    double min_time_next_event = 1.0e+29;
    type_next_event = 0;

    /* Determine the event type of the next event to occur. */

    for (i = 1; i <= number_events; ++i)
        if (time_next_event[i] < min_time_next_event)
        {
            min_time_next_event = time_next_event[i];
            type_next_event     = i;
        }
    /* Check to see whether the event list is empty. */
    if (type_next_event == 0)
    {
        /* The event list is empty, so stop the simulation. */
        fprintf(salida, "\nLista de eventos vacia en el tiempo: %f", time_sim);
        //exit(1);
    }
    /* The event list is not empty, so advance the simulation clock. */
    time_sim = min_time_next_event;
}


void arrive(void)  /* Arrival event function. */
{
    double delay;
    /* Schedule next arrival. */
    time_next_event[1] = time_sim + uniform(min_arr,max_arr);
    /* Check to see whether server is busy. */
    if (status_server1 == BUSY)
    {
        /* Server is busy, so increment number of customers in queue. */
        ++num_q_1;
        /* Check to see whether an overflow condition exists. */
        if (num_q_1 > Q1_LIMIT)
        {
            /* The queue has overflowed, so stop the simulation. */
            fprintf(salida, "\nOverflow en el arreglo time_arrival en el");
            fprintf(salida, " tiempo %f", time_sim);
            //exit(2);
            ++num_q_2;
        }
        /* There is still room in the queue, so store the time of arrival of the
           arriving customer at the (new) end of time_arrival. */
        time_arrival[num_q_1] = time_sim;
    }
    else
    {
        /* Server is idle, so arriving customer has a delay of zero.  (The
           following two statements are for program clarity and do not affect
           the results of the simulation.) */
        delay            = 0.0;
        total_of_delays += delay;
        /* Increment the number of customers delayed, and make server busy. */
        ++num_custs_delayedq1;
        status_server1 = BUSY;
        /* Schedule a departure (service completion). */
        time_next_event[2] = time_sim + expon(mean_service);
    }
}


void depart(void)  /* Departure event function. */
{
    int   i;
    double delay;
    /* Check to see whether the queue is empty. */
    if (num_q_1 == 0)
    {
        /* The queue is empty so make the server idle and eliminate the
           departure (service completion) event from consideration. */
        status_server1      = IDLE;
        time_next_event[2] = 1.0e+30;
    }
    else
    {
        /* The queue is nonempty, so decrement the number of customers in
           queue. */
        --num_q_1;
        /* Compute the delay of the customer who is beginning service and update
           the total delay accumulator. */
        delay            = time_sim - time_arrival[1];
        total_of_delays += delay;
        /* Increment the number of customers delayed, and schedule departure. */
        ++num_custs_delayedq1;
        time_next_event[2] = time_sim + expon(mean_service);
        /* Move each customer in queue (if any) up one place. */
        for (i = 1; i <= num_q_1; ++i)
            time_arrival[i] = time_arrival[i + 1];

        time_next_event[3] = time_sim + uniform(min_arr2,max_arr2);
    }
}


void arrive_2(void)  /* Arrival event function. */
{
    double delay;
    /* Check to see whether server is busy. */
    if (status_server2 == BUSY)
    {
        /* Server is busy, so increment number of customers in queue. */
        ++num_q_2;
        /* Check to see whether an overflow condition exists. */
        if (num_q_2 > Q_LIMIT)
        {
            /* The queue has overflowed, so stop the simulation. */
            //fprintf(salida, "Overflow en el arreglo time_arrival en el tiempo %fn", time_sim);
            printf("error overflow en el arreglo time arrival 2");
            exit(2);
        }
        /* There is still room in the queue, so store the time of arrival of the
           arriving customer at the (new) end of time_arrival. */
        arrival_time2[num_q_2] = time_sim;
    }
    else
    {
        /* Server is idle, so arriving customer has a delay of zero.  (The
           following two statements are for program clarity and do not affect
           the results of the simulation.) */
        delay            = 0.0;
        total_of_delays_2 += delay;
        /* Increment the number of customers delayed, and make server busy. */
        ++num_custs_delayedq2;
        status_server2 = BUSY;
        /* Schedule a departure (service completion). */
        time_next_event[4] = time_sim + expon(mean_service_2);
    }
}


void depart_2(void)  /* Departure event function. */
{
    int   i;
    double delay;
    /* Check to see whether the queue is empty. */
    if (num_q_2 == 0)
    {
        /* The queue is empty so make the server idle and eliminate the
           departure (service completion) event from consideration. */
        status_server2      = IDLE;
        time_next_event[4] = 1.0e+30;
    }
    else
    {
        /* The queue is nonempty, so decrement the number of customers in
           queue. */
        --num_q_2;
        /* Compute the delay of the customer who is beginning service and update
           the total delay accumulator. */
        delay            = time_sim - arrival_time2[1];
        total_of_delays_2 += delay;
        /* Increment the number of customers delayed, and schedule departure. */
        ++num_custs_delayedq2;
        time_next_event[4] = time_sim + expon(mean_service_2);
        /* Move each customer in queue (if any) up one place. */
        for (i = 1; i <= num_q_2; ++i)
            arrival_time2[i] = arrival_time2[i + 1];
    }
}



void report(void)  /* Report generator function. */
{
    /* Compute and write estimates of desired measures of performance. */

    fprintf(salida, "\n\nPromedio retraso en la cola 1 %11.3f minutes\n\n",
            total_of_delays / num_custs_delayedq1);
    fprintf(salida, "\n\nPromedio retraso en la cola 2 %11.3f minutes\n\n",
            total_of_delays_2 / num_custs_delayedq2);
    fprintf(salida, "Numero promedio en la cola 1 %10.3f\n\n",
            area_num_in_q1 / time_sim);
    fprintf(salida, "Numero promedio en la cola 2 %10.3f\n\n",
            area_num_in_q2 / time_sim);
    fprintf(salida, "Uso del canal 1 %15.3f\n\n",
            area_server_status1 / time_sim);
    fprintf(salida, "Uso del canal 2 %15.3f\n\n",
            area_server_status2 / time_sim);
    fprintf(salida, "Time simulation ended%12.3f minutes", time_sim);
}


void update_time_avg_stats(void)  /* Update area accumulators for time-average
                                     statistics. */
{
    double time_since_last_event;
    /* Compute time since last event, and update last-event-time marker. */
    time_since_last_event = time_sim - time_last_event;
    time_last_event       = time_sim;
    /* Update area under number-in-queue function. */
    area_num_in_q1      += num_q_1 * time_since_last_event;
    area_num_in_q2      += num_q_2 * time_since_last_event;
    /* Update area under server-busy indicator function. */
    area_server_status1 += status_server1 * time_since_last_event;
    area_server_status2 += status_server2 * time_since_last_event;
}


double expon(double mean)  /* Exponential variate generation function. */
{
    /* Return an exponential random variate with mean "mean". */
    return -mean * log(lcgrand(1));
}


double uniform(double a, double b)  /* Uniform variate generation function. */
{
    /* Return a U(a,b) random variate. */
    return a + lcgrand(1) * (b - a);
}
