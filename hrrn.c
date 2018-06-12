#include <stdio.h>
#include <stdlib.h>
#include "hrrn.h"

////////////////////////////////////////////////////////////////////////////////////////
//returns the process with the highest response ratio and removes it from the ready queue
struct Process* pop_hrr()
{
	struct Process *current, *highest;
	current = p_head;
	highest = p_head;
	while(current->next != NULL)
	{
		//find the process with the highest response ratio
		if((clock - current->arrival_time + current->service_time)/current->service_time >
		(clock - highest->arrival_time + highest->service_time)/highest->service_time)
		{
			highest = current;
		}
		current = current->next;
	}

	//remove the highest rr process from the ready queue
	if(highest == p_head && p_head->next == NULL)
		p_head = NULL;
	else if(highest == p_head && p_head->next != NULL)
	{
		p_head = p_head->next;
		p_head->prev = NULL;
	}
	else
	{
		highest->prev->next = highest->next;
		if(highest->next != NULL)
			highest->next->prev = highest->prev;
	}

	return highest;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void hrrn_arrival(struct Event* eve)
{
	struct Process* arrived_process;
	arrived_process = new_process(eve->p_id, clock, genexp(1/avg_service_time));
    //printf("service time: %f\n", arrived_process->service_time);

	if(server_busy == 0)
	{
		//create the departure event for this process and make it the current process
		struct Event* departure = new_event(arrived_process->id, EVENT2, clock + arrived_process->service_time);
		schedule_event(departure);

		current_process = arrived_process;

		server_busy = 1;
		idle_time += clock - went_idle;
	}
	else
	{
	    queue_size++;
	    accumulated_queue_size += queue_size * (clock - last_queue_change);
	    last_queue_change = clock;
		queue_process(arrived_process);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////
void hrrn_departure(struct Event* eve)
{
	total_turnaround += clock - current_process->arrival_time;
	free(current_process);

	if(queue_size == 0)
	{
		server_busy = 0;
		went_idle = clock;
		//printf("%f ", clock);
		//printf("Server has gone idle\n");
	}
	else
	{
		//make process with highest response ratio current, schedule it's departure
		current_process = pop_hrr();
		queue_size--;
		accumulated_queue_size += queue_size * (clock - last_queue_change);
	    last_queue_change = clock;

		struct Event* departure = new_event(current_process->id, EVENT2, clock + current_process->service_time);
		schedule_event(departure);

		//printf("%f ", clock);
		//printf("Process %i is now current process. ", current_process->id);
		//printf("Queue size is %i\n", queue_size);

	}
}
