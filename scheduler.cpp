#include "scheduler.h"
#define IDLE nullptr

using namespace std;

Scheduler::Scheduler(SystemTask &to) : Scheduler(to, 0) { }

Scheduler::Scheduler(SystemTask &to, int nbProcessors) : taskSystem(to)
{
    this->min = 0;
    this->max = to.feasibleInterval().max;
    this->nbProcessors = nbProcessors;
}


ScheduleLogs Scheduler::scheduleGlobal()
{
    Job *highestPriorityJob, *previousJob;
    // Clear the previous scheduling
    assignments.clear();
    // Informations about this schedule
    ScheduleLogs scheduleInfos;
    for(int i = 0; i < nbProcessors; i++) {
        scheduleInfos.nbIdlePerProcessor[i] = 0;
        scheduleInfos.nbPreemptionsPerProcessor[i] = 0;
        scheduleInfos.utilisationPerProcessor[i] = 0.0;
    }
    scheduleInfos.studyInterval = {0, taskSystem.feasibleInterval().max};
    scheduleInfos.nbDesiredProcessors = nbProcessors;
    scheduleInfos.failed = false;
    // Lambda that represents how jobs must be compared
    auto comparator = [](Job * left, Job *right) -> bool {
        return left->absolute_deadline > right->absolute_deadline;
    };

    // Represents all pending jobs.
    std::priority_queue<Job *, std::vector<Job *>,
                        decltype(comparator)> jobs_to_execute(comparator);
    
    // For each slot of time and while the scheduling hasn't failed
    for(int t = 0; t < taskSystem.feasibleInterval().max && !scheduleInfos.failed; t++)
    {
        // All tasks that must release a job do it.
        // (release time of the next job of a task >= i)
        for(Task &task : taskSystem.getTaskSet())
            if (task.nextJob().release == t)
                jobs_to_execute.push(&task.releaseJob());

        // And then, for each processors and while the scheduling hasn't failed
        for(int P = 0; P < nbProcessors && !scheduleInfos.failed; P++) {
            // Is there any job at slot t-1  OR
            // is the task at slot t-1 on the processor P finished ?
            previousJob = jobAt(P, t-1);
            if(previousJob == nullptr
                    || (previousJob != nullptr
                        && previousJob->remeaningComputation == 0))
            {
                // YES, release the next highest priority job if there is one.
                if(! jobs_to_execute.empty()) {
                    highestPriorityJob = jobs_to_execute.top();
                    assign(P, t, highestPriorityJob);
                    jobs_to_execute.pop();

                    // Deadline will miss ?
                    if(highestPriorityJob->absolute_deadline
                         < (t + highestPriorityJob->remeaningComputation))
                    {
                        scheduleInfos.failed = true;
                    }
                    else
                        highestPriorityJob->remeaningComputation--;

                } else {
                    // There is no job to execute. Insert an idle time.
                    scheduleInfos.nbIdlePerProcessor[P]++;
                    scheduleInfos.nbIdleTotal++;
                }
            }
            else {
                // NO, is there any awaiting job with a higher priority ?
                if(!jobs_to_execute.empty()
                  && (jobs_to_execute.top()->absolute_deadline
                      < previousJob->absolute_deadline)) {

                    // YES, swap the awaiting job with the assigned.
                    highestPriorityJob = jobs_to_execute.top();
                    jobs_to_execute.pop();
                    jobs_to_execute.push(previousJob);
                    assign(P, t, highestPriorityJob);
                    // Deadline will miss ?
                    if(highestPriorityJob->absolute_deadline
                         < (t + highestPriorityJob->remeaningComputation))
                    {
                        scheduleInfos.failed = true;
                    }
                    else
                        highestPriorityJob->remeaningComputation--;

                    // So, a preemption happened on processor P.
                    scheduleInfos.nbPreemptionsPerProcessor[P]++;
                    scheduleInfos.nbPreemptionsTotal++;
                }
                else {
                    // NO, continue executing the previous job.
                    highestPriorityJob = previousJob;
                    assign(P, t, highestPriorityJob);
                    // Deadline will miss ?
                    if(highestPriorityJob->absolute_deadline
                         < (t + highestPriorityJob->remeaningComputation))
                    {
                        scheduleInfos.failed = true;
                    }
                    else
                        highestPriorityJob->remeaningComputation--;
                }
            }
        }
    }

    // Computation of utilisations...
    for(int P = 0; P < nbProcessors; P++) {
        scheduleInfos.utilisationPerProcessor[P] =
             (double) (scheduleInfos.studyInterval.max - scheduleInfos.nbIdlePerProcessor[P])
           / (double) (scheduleInfos.studyInterval.max);
    }
    scheduleInfos.utilisationTotal =
             (double) (scheduleInfos.studyInterval.max*nbProcessors - scheduleInfos.nbIdleTotal)
           / (double) (scheduleInfos.studyInterval.max*nbProcessors);



    return scheduleInfos;
}





ScheduleLogs Scheduler::schedulePartitionned()
{
    return {};
}

Job *Scheduler::jobAt(int slot, int numProcessor)
{
    auto it = std::find_if(assignments.begin(),
                           assignments.end(),
                           // [=] to indicate that I would like all local
                           // variables to be captured by value in a lambda
                           [=](Assignment &a) {
                                return a.numProcessor == numProcessor
                                    && a.slotTime == slot;
                           });

    return it != assignments.end() ? (*it).assignedJob : nullptr;
}

void Scheduler::assign(int numProcessor, int slot, Job *job)
{
    assignments.push_back({ job, numProcessor, slot });
}

