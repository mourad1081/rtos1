#ifndef ASSIGNMENT_H
#define ASSIGNMENT_H

#include "systemtask.h"

class Assignment
{
private:
    int numProcessor;
    int slotTime;
    Job *assignedJob;

public:

    Assignment(Job *j, int p, int s);

    int  getIndexProcessor();
    void setIndexProcessor(int value);
    int  getSlotTime();
    void setSlotTime(int value);
    Job *getAssignedJob();
    void setAssignedJob(Job *value);
};

#endif // ASSIGNMENT_H
