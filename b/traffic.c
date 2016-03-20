#include <stdbool.h>
#include <assert.h>
#include "crossing.h"

int K;
int num_vehicles;
int num_pedestrians;

void *vehicles(void *arg);
void *pedestrians(void *arg);

sem_t horizontal_street;
sem_t vertical_street;
sem_t both_streets;

pthread_t *pedestrian_thread;
pthread_t *vehicle_thread;

void init()
{
    sem_init(&horizontal_street, 0, 1);
    sem_init(&vertical_street, 0, 1);
    sem_init(&both_streets, 0, 1);

    pedestrian_thread = malloc(sizeof(pthread_t) * num_pedestrians);
    vehicle_thread = malloc(sizeof(pthread_t) * num_vehicles);
}

void spawn_pedestrian(thread_info *arg)
{
    Pthread_create(&pedestrian_thread[arg->thread_nr], 0, pedestrians, arg);
}

void spawn_vehicle(thread_info *arg)
{
    Pthread_create(&vehicle_thread[arg->thread_nr], 0, vehicles, arg);
}

void *vehicles(void *arg)
{
    thread_info *info = arg;

    // Each thread needs to call these functions in this order
    // Note that the calls can also be made in helper functions
    int place = vehicle_arrive(info);
    
    if (info->crossing == 3){
      P(&horizontal_street);
    }
    else {
      P(&vertical_street);
    }

    P(&both_streets);
    vehicle_drive(info);
    vehicle_leave(info);
    V(&both_streets);
    
    if (info->crossing == 3){
      V(&horizontal_street);
    }
    else {
      V(&vertical_street);
    }

    return NULL;
}

void *pedestrians(void *arg)
{
    thread_info *info = arg;

    // Each thread needs to call these functions in this order
    // Note that the calls can also be made in helper functions
    int place = pedestrian_arrive(info);
    
    if (info->crossing == 0){
      P(&horizontal_street);
    }
    else {
      P(&vertical_street);
    }

    //P(&cross_access);
    pedestrian_walk(info);
    pedestrian_leave(info);
    //V(&cross_access);
    
    if (info->crossing == 0){
      V(&horizontal_street);
    }
    else {
      V(&vertical_street);
    }

    return NULL;
}


void clean()
{
    for(int i = 0; i < num_pedestrians; i++) {
	Pthread_join(pedestrian_thread[i], NULL);
    }
    for(int i = 0; i < num_vehicles; i++) {
	Pthread_join(vehicle_thread[i], NULL);
    }
}
