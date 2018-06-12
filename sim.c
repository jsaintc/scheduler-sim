#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "sim.h"
#include "fcfs.h"
#include "srtf.h"
#include "hrrn.h"
#include "rr.h"

struct Event* e_head; // head of event queue
struct Process* p_head; //head of ready queue
struct Process* current_process; //process currently getting service
float clock; // simulation clock
float lambda; //arrival rate
float avg_service_time;
float total_turnaround;
float idle_time;
float went_idle;
float q_duration;
float last_queue_change;
float accumulated_queue_size;
int schedule;
int server_busy;
int queue_size;
int total_departures;
int next_proc_id;

void init(int sched, float lam, float stime, float q)
{
    clock = 0;
    total_turnaround = 0;
    idle_time = 0;
    went_idle = 0;
    last_queue_change = 0;
    accumulated_queue_size = 0;
    server_busy = 0;
    queue_size = 0;
    total_departures = 0;
    next_proc_id = 0;

    p_head = NULL;
    current_process = NULL;

    lambda = lam;
    avg_service_time = stime;
    q_duration = q;
    schedule = sched;

    //put first arrival into event queue
    e_head = new_event(next_proc_id++, EVENT1, 0);
}
////////////////////////////////////////////////////////////////
void generate_report()
{
	float avg_turnaround = total_turnaround / MAX_DEPARTURES;
	float avg_throughput = MAX_DEPARTURES / clock;
	float cpu_util = (clock - idle_time) / clock;
	float avg_queue_size = accumulated_queue_size / clock;

	printf("%f,", avg_turnaround);
	printf("%f,", avg_throughput);
	printf("%f,", cpu_util);
	printf("%f\n", avg_queue_size);
}
////////////////////////////////////////////////////////////////
//schedules an event in the future
void schedule_event(struct Event* ne)
{
	//if the future event has the same time as the clock due to imprecise floating point addition, increase it slightly
	if(ne->time == clock)
		ne->time += 0.01;

    // insert event in the event queue in its order of time
    struct Event* current;

	if(e_head == NULL || e_head->time >= ne->time)
	{
		ne->next = e_head;
		e_head = ne;
	}
	else
	{
		current = e_head;
		while(current->next != NULL && current->next->time <= ne->time)
		{
			current = current->next;
		}
		ne->next = current->next;
		current->next = ne;
    }

    //printf("%f ", clock);
	//printf("Scheduling event type %i ", ne->type);
	//printf("for time %f\n", ne->time);
}
////////////////////////////////////////////////////////////////
void delete_event(int id, int ty)
{
    struct Event *current, *previous;
    current = e_head;
    while(current != NULL)
    {
    	if(current->p_id == id && current->type == ty)
    	{
    		if(current == e_head)
    		{
    			e_head = current->next;
    			free(current);
    			return;
    		}
    		else
    		{
    			previous->next = current->next;
    			free(current);
    			return;
    		}
    	}
    	else
    	{
    		previous = current;
    		current = current->next;
    	}
    }
}
////////////////////////////////////////////////////////////////
void queue_process(struct Process* np)
{
	struct Process* current;
	np->next = NULL;

	if(p_head == NULL)
    {
        p_head = np;
    }
    else
    {
    	current = p_head;
    	while(current->next != NULL)
    	{
    		current = current->next;
    	}
	current->next = np;
	np->prev = current;
    }
	//printf("%f ", clock);
    //printf("Process %i queued. ", np->id);
    //printf("Queue size is %i\n", queue_size);
}
////////////////////////////////////////////////////////////////
void process_arrival(struct Event* eve)
{

    //schedule another arrival
    struct Event* next_arrival = new_event(next_proc_id++, EVENT1, clock + genexp(lambda));
    schedule_event(next_arrival);

	//printf("%f ", clock);
    //printf("Arrival id: %i ", eve->p_id);

    switch(schedule)
    {
      case SCHEDULE1:
    	  fcfs_arrival(eve);
    	  break;
      case SCHEDULE2:
    	  srtf_arrival(eve);
    	  break;
      case SCHEDULE3:
    	  hrrn_arrival(eve);
    	  break;
      case SCHEDULE4:
    	  rr_arrival(eve);
    	  break;
      default:
    	  break;
    }
}
////////////////////////////////////////////////////////////////
void process_departure(struct Event* eve)
{
    total_departures++;

	//printf("%f ", clock);
    //printf("Departure id: %i\n", eve->p_id);

    switch(schedule)
    {
      case SCHEDULE1:
    	  fcfs_departure(eve);
    	  break;
      case SCHEDULE2:
    	  srtf_departure(eve);
    	  break;
      case SCHEDULE3:
    	  hrrn_departure(eve);
    	  break;
      case SCHEDULE4:
    	  rr_departure(eve);
    	  break;
      default:
    	  break;
    }
}
////////////////////////////////////////////////////////////////
//creates a new event structure
struct Event* new_event(int i, int ty, float tm)
{
    struct Event* event;
	event = (struct Event*) malloc(sizeof(struct Event));
    event->p_id = i;
    event->type = ty;
    event->time = tm;
    event->next = NULL;

    return event;
}
////////////////////////////////////////////////////////////////
//creates a new process structure
struct Process* new_process(int i, float at, float st)
{
	struct Process* process;
	process = (struct Process*) malloc(sizeof(struct Process));
	process->id = i;
	process->arrival_time = at;
	process->service_time = st;
	process->service_began = 0;
	process->next = NULL;
	process->prev = NULL;

	return process;
}
////////////////////////////////////////////////////////////////
// returns a random number between 0 and 1
float urand()
{
	return( (float) rand()/RAND_MAX );
}
/////////////////////////////////////////////////////////////
// returns a random number that follows an exp distribution
float genexp(float lambda)
{
	float u,x;
	x = 0;
	while (x == 0)
		{
			u = urand();
			x = (-1/lambda)*log(u);
		}
	return(x);
}
////////////////////////////////////////////////////////////
int run_sim()
{
	struct Event* eve;
	int done = 0;
	while (!done)
    {

		eve = e_head;
		clock = eve->time;
		switch (eve->type)
		{
		case EVENT1:
			process_arrival(eve);
			break;
		case EVENT2:
			process_departure(eve);
			break;
		case EVENT3:
			slice_finished();
			break;
		default:
			break;
		}

		e_head = eve->next;
		free(eve);
		eve = NULL;

		if (total_departures == MAX_DEPARTURES)
		{
			done = 1;
		}
    }
  return 0;
}
////////////////////////////////////////////////////////////////
int main(int argc, char *argv[] )
{
  // parse arguments
  int sched;
  float lam, stime, q;

  sched = atoi(argv[1]);
  lam = atof(argv[2]);
  stime = atof(argv[3]);
  q = atof(argv[4]);

  init(sched, lam, stime, q);
  run_sim();
  generate_report();
  return 0;
}
