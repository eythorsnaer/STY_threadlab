#include <stdbool.h>
#include <assert.h>
#include "crossing.h"

int K;
int num_vehicles;
int num_pedestrians;

void *vehicles(void *arg);
void *pedestrians(void *arg);

sem_t mutex;
sem_t vhLock;
sem_t vvLock;
sem_t phLock;
sem_t pvLock;

pthread_t *pedestrian_thread;
pthread_t *vehicle_thread;

void init()
{
    sem_init(&mutex, 0, 1);
    sem_init(&vhLock, 0, 1);
    sem_init(&vvLock, 0, 1);
    sem_init(&phLock, 0, 1);
    sem_init(&pvLock, 0, 1);

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
    
    //P(&mutex);

    if (arg->crossing == 1)
    {
	P(&vhLock);
	P(&pvLock);
	V(&phLock);
	
	vehicle_drive(info);
	vehicle_leave(info);
   
    }
    else if(arg->crossing == 3)
    {
	P(&vvLock);
	P(&phLock);
	V(&pvLock);
    }
       
    V(&mutex);
    
    return NULL;
}

void *pedestrians(void *arg)
{
    thread_info *info = arg;

    // Each thread needs to call these functions in this order
    // Note that the calls can also be made in helper functions
    int place = pedestrian_arrive(info);

    P(&mutex);

    if (arg->crossing == 0)
	{
	    P(&vvLock);
	    V(&phLock);
	}
    else if(arg->crossing == 2)
	{
	    P(&vvLock);
	    P(&phLock);
	    V(&pvLock);
	}


    pedestrian_walk(info);
    pedestrian_leave(info);

    V(&mutex);

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
