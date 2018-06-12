#ifndef RR_H
#define RR__H
#include "sim.h"
void rr_arrival(struct Event* eve);
void rr_departure(struct Event* eve);
void slice_finished();
#endif
