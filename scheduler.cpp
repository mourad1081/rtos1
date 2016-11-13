#include "scheduler.h"
#include <iostream>

Scheduler::Scheduler(SystemTask &to) : Scheduler(to, 0) { }

Scheduler::Scheduler(SystemTask &to, int nbProcessors) : taskSystem(to)
{
    int k = 0, slotDeadline = 0, slotRelease = 0;
    this->min = 0;
    this->max = to.feasibleInterval().max;
    this->nbProcessors = nbProcessors;

    // We place all the deadlines during the feasible interval.
    for(Task &t : to.getTaskSet()) {
        k = 0;
        slotDeadline = t.offset + t.deadline + k*t.period;
        slotRelease = t.offset + k*t.period;
        while(slotDeadline <= max && slotRelease <= max) {
            // We add the deadline (& release) to all of its occurences
            tasks_deadlines[&t].push_back(slotDeadline);
            tasks_releases[&t].push_back(slotRelease);
            k++;
            slotDeadline = t.offset + t.deadline + k*t.period;
            slotDeadline = t.offset + k*t.period;
        }
    }
}

void Scheduler::scheduleGlobal()
{

}

void Scheduler::schedulePartitionned()
{

}

void Scheduler::resetSchedule()
{

}
