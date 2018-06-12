#include <stdio.h>
#include <stdlib.h>
#include "fcfs.h"
////////////////////////////////////////////////////////////////////////////////////////////
void fcfs_arrival(struct Event* eve)
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
////////////////////////////////////////////////////////////////////////////////////////////
void fcfs_departure(struct Event* eve)
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
		//make process at head of queue the current process and create its departure event
		current_process = p_head;
		p_head = p_head->next;
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
