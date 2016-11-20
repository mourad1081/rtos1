#ifndef JOB_H
#define JOB_H

class Task;

class Job
{
public:
    Job();
    Job(int a, int e, int d);
    Job(int a, int e, int d, int ad);
    int release;
    int computation;
    int deadline;
    int absolute_deadline;
    Task *parent;
    // The remeaning amount of time needed to finish the job.
    int remeaningComputation;
};

#endif // JOB_H