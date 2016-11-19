#include "assignment.h"

int Assignment::getIndexProcessor()
{
    return numProcessor;
}

void Assignment::setIndexProcessor(int value)
{
    numProcessor = value;
}

int Assignment::getSlotTime()
{
    return slotTime;
}

void Assignment::setSlotTime(int value)
{
    slotTime = value;
}

Job *Assignment::getAssignedJob()
{
    return assignedJob;
}

void Assignment::setAssignedJob(Job *value)
{
    assignedJob = value;
}

Assignment::Assignment(Job *j, int p, int s) : numProcessor {p},
                                               slotTime {s},
                                               assignedJob {j}
{ }
