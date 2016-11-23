#include "task.h"

Task::Task(int O, int T, int D, int C) : Task(O, T, D, C, 0) {}
Task::Task(int O, int T, int D, int C, int num)
{
    if(D > T)
        throw std::invalid_argument("constrained deadlines must have D <= T");

    this->WCET = C;
    this->period = T;
    this->deadline = D;
    this->offset = O;
    this->num = num;
}


void Task::addJob(Job &j)
{
    this->jobs.push(j);
}

Job &Task::releaseJob()
{
    Job &j = jobs.front();
    jobs.pop();
    return j;
}

Job &Task::nextJob()
{
    return jobs.front();
}


bool Task::hasNextJob() {
    return !jobs.empty();
}
