#include <stdio.h>
#include <stdlib.h>
#include "rr.h"
/////////////////////////////////////////////////////////////////////////////////////////////////
void rr_arrival(struct Event* eve)
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
		current_process->service_began = clock;

		server_busy = 1;
		idle_time += clock - went_idle;
	}
	else
	{
	    queue_size++;
	    accumulated_queue_size += queue_size * (clock - last_queue_change);
	    last_queue_change = clock;
		queue_process(arrived_process);

		if(queue_size == 1)
		{
		struct Event* time_slice = new_event(current_process->id, EVENT3, clock + q_duration);
		schedule_event(time_slice);
		}

	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////
void rr_departure(struct Event* eve)
{
	total_turnaround += clock - current_process->arrival_time;
	delete_event(current_process->id, EVENT3);
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
		//make process at head of queue the current process and create its departure and time slice events
		current_process = p_head;
		current_process->service_began = clock;
		p_head = p_head->next;

		queue_size--;
		accumulated_queue_size += queue_size * (clock - last_queue_change);
	    last_queue_change = clock;

		struct Event* departure = new_event(current_process->id, EVENT2,  clock + current_process->service_time);
		schedule_event(departure);

		if(queue_size != 0)
		{
			struct Event* time_slice = new_event(current_process->id, EVENT3, clock + q_duration);
			schedule_event(time_slice);
		}

		//printf("%f ", clock);
		//printf("Process %i is now current process. ", current_process->id);
		//printf("Queue size is %i\n", queue_size);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////
/*If there is another process waiting, the current process goes back in the queue and the next process
 *  gets a turn. Otherwise, do nothing*/
void slice_finished()
{
	if(queue_size != 0)
	{
		delete_event(current_process->id, EVENT2);
        current_process->service_time -= (clock - current_process->service_began);
		queue_process(current_process);

		current_process = p_head;
		current_process->service_began = clock;
		p_head = p_head->next;

		struct Event* departure = new_event(current_process->id, EVENT2, clock + current_process->service_time);
		schedule_event(departure);

		struct Event* time_slice = new_event(current_process->id, EVENT3, clock + q_duration);
		schedule_event(time_slice);

		//printf("%f ", clock);
		//printf("Process %i is now current process. ", current_process->id);
		//printf("Queue size is %i\n", queue_size);
	}
}
