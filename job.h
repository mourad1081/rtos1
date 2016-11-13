#ifndef JOB_H
#define JOB_H


class Job
{
public:
    Job();
    Job(int a, int e, int d);
    int release;
    int computation;
    int deadline;
    // The remeaning amount of time needed to finish the job.
    int remeaningComputation;
};

#endif // JOB_H
