#include <stdio.h>
#include <stdlib.h>
#include "srtf.h"
/////////////////////////////////////////////////////////////////////////////////////////////////
//Puts the new process in the ready queue ordered by service time
void queue_process_srtf(struct Process* np)
{
    struct Process* current;

    if(p_head == NULL || p_head->service_time > np->service_time)
    {
        np->next = p_head;
    	p_head = np;
    }
    else
    {
        current = p_head;
        while(current->next != NULL && current->next->service_time <= np->service_time)
        {
			current = current->next;
        }
        np->next = current->next;
        current->next = np;
    }

	//printf("%f ", clock);
    //printf("Process %i queued. ", np->id);
    //printf("Queue size is %i\n", queue_size);
}
/////////////////////////////////////////////////////////////////////////////////////////////////
void srtf_arrival(struct Event* eve)
{
    struct Process* arrived_process;
    arrived_process = new_process(eve->p_id, clock, genexp(1/avg_service_time));
    //printf("service time: %f\n", arrived_process->service_time);

    if(server_busy == 0)
    {
    	//create the departure event for this process
        struct Event* departure;
        departure = new_event(arrived_process->id, EVENT2, clock + arrived_process->service_time);
        schedule_event(departure);

        current_process = arrived_process;
        current_process->service_began = clock;

        server_busy = 1;
        idle_time += clock - went_idle;
    }
    else
    {
    	//if the new process has a shorter service time, preempt the current process, if not, queue the new process
        current_process->service_time -= (clock - current_process->service_began);
        if(current_process->service_time > arrived_process->service_time)
        {
        	queue_size++;
            accumulated_queue_size += queue_size * (clock - last_queue_change);
    	    last_queue_change = clock;

            delete_event(current_process->id, EVENT2);
            queue_process_srtf(current_process);

            current_process = arrived_process;
            current_process->service_began = clock;

            struct Event* departure = new_event(current_process->id, EVENT2, clock + current_process->service_time);
            schedule_event(departure);

        	//printf("%f ", clock);
            //printf("Current process preempted\n");
        }
        else
        {
        	queue_size++;
            accumulated_queue_size += queue_size * (clock - last_queue_change);
    	    last_queue_change = clock;
            queue_process_srtf(arrived_process);
        }
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////
void srtf_departure(struct Event* eve)
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
		//make process at head of queue the current process and create its departure event;
		current_process = p_head;
		current_process->service_began = clock;
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
