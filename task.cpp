#include "task.h"

Task::Task() : Task(0, 0, 0, 0) { }

Task::Task(int O, int T, int D, int C)
{
    if(D > T)
        throw std::invalid_argument("constrained deadlines must have D <= T");

    this->WCET = C;
    this->period = T;
    this->deadline = D;
    this->offset = O;

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
