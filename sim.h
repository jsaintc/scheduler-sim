#ifndef SIM_H
#define SIM_H

////////////////////////////////////////////////////////////////
#define EVENT1 1
#define EVENT2 2
#define EVENT3 3

#define SCHEDULE1 1
#define SCHEDULE2 2
#define SCHEDULE3 3
#define SCHEDULE4 4

#define MAX_DEPARTURES 10000

////////////////////////////////////////////////////////////////
struct Event{
  int    p_id;
  int    type;
  float  time;
  struct Event* next;
};

struct Process{
  int    id;
  float  arrival_time;
  float  service_began;
  float  service_time;
  struct Process* next;
  struct Process* prev;
};

////////////////////////////////////////////////////////////////
// function definition
void init(int sched, float lam, float stime, float q);
void generate_report();
void schedule_event(struct Event* ne);
void delete_event(int id, int ty);
void queue_process(struct Process* np);
void process_arrival(struct Event* eve);
void process_departure(struct Event* eve);
struct Event* new_event(int i, int ty, float tm);
struct Process* new_process(int i, float at, float st);
float urand();
float genexp(float lambda);
int run_sim();

////////////////////////////////////////////////////////////////
//Global variables
extern struct Event* e_head; // head of event queue
extern struct Process* p_head; //head of ready queue
extern struct Process* current_process; //process currently getting service
extern float clock; // simulation clock
extern float lambda; //arrival rate
extern float avg_service_time;
extern float total_turnaround;
extern float idle_time;
extern float went_idle;
extern float q_duration;
extern float last_queue_change;
extern float accumulated_queue_size;
extern int schedule;
extern int server_busy;
extern int queue_size;
extern int total_departures;
extern int next_proc_id;

#endif
