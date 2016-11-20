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
                    highestPriorityJob = jobAt(t-1, P);
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

void Scheduler::exportToBMP(string pathFile)
{
    int w = 1300, h = 700, padding = 50;
    int lengthTimeAxis      = w - 2*padding;
    int lengthProcessorAxis = h - 2*padding;
    int lengthDashes = 10;
    int widthAxis    =  3;
    int cptProcessor =  1;
    std::string indexProcessor;
    BMP image;
    std::map<Task *, RGBApixel> taskColors;
    image.SetSize(w, h);

    // Time axis
    for(int i = padding; i < padding + lengthTimeAxis; i++) {
        for(int j = h - padding - (int)(widthAxis/2); j < h - padding + (int)(widthAxis/2); j++) {
            image(i, j)->Blue  = 255;
            image(i, j)->Red   =   0;
            image(i, j)->Green =   0;
        }
        if(i%50 == 0)
            PrintString(image,
                        const_cast<char*>(std::to_string(i).c_str()),
                        i, lengthProcessorAxis + padding + (int)(padding*0.25),
                        8, {0,0,0,0} /* = {Blue, Green, Red, Alpha} */ );
    }

    // Dashes on time axis
    int offset = lengthTimeAxis / taskSystem.feasibleInterval().max;
    for(int i = padding; i < padding + lengthTimeAxis; i += offset)
    {
        for(int j =  lengthProcessorAxis + padding - (int)(lengthDashes/2);
                j <  lengthProcessorAxis + padding + (int)(lengthDashes/2);
                j++)
        {
            image(i, j)->Blue  = 0;
            image(i, j)->Red   = 0;
            image(i, j)->Green = 0;
        }
    }

    // Processors axis
    for(int i = padding; i < padding + lengthProcessorAxis; i++) {
        for(int j = padding - (int)(widthAxis/2); j < padding + (int)(widthAxis/2); j++) {
            image(j, i)->Blue  = 255;
            image(j, i)->Red   =   0;
            image(j, i)->Green =   0;
        }
    }

    // Dashes on processor axis
    offset = lengthProcessorAxis / nbProcessors;
    for(int i = padding; i < padding + lengthProcessorAxis; i += offset)
    {
        for(int j =  padding - (int)(lengthDashes/2);
                j <  padding + (int)(lengthDashes/2);
                j++)
        {
            image(j, i)->Blue  = 0;
            image(j, i)->Red   = 0;
            image(j, i)->Green = 0;
        }
        indexProcessor = std::string("Pi " + std::to_string(cptProcessor));
        PrintString(image, const_cast<char*>(indexProcessor.c_str()), (int)(padding*0.25), i, 10, {0,0,0,0} /* = {Blue, Green, Red, Alpha} */ );
        cptProcessor++;
    }

    // Assignments
    for(unsigned int i = 0; i < assignments.size(); i++) {
        // Assign a random color to the task if it hasn't one yet.
        if(taskColors.find(assignments[i].assignedJob->parent) == taskColors.end()) {
            taskColors[assignments[i].assignedJob->parent] = {
                (ebmpBYTE) taskSystem.random(1, 256),
                (ebmpBYTE) taskSystem.random(1, 256),
                (ebmpBYTE) taskSystem.random(1, 256),
                (ebmpBYTE) 0
            };
        }
        int fromX, fromY, toX, toY;
        fromX = padding + (lengthTimeAxis/taskSystem.feasibleInterval().max)*assignments[i].slotTime;
        fromY = padding + (lengthProcessorAxis/nbProcessors)*assignments[i].numProcessor;
        toX   = fromX + (lengthTimeAxis/taskSystem.feasibleInterval().max);
        toY   = fromY + (lengthProcessorAxis/nbProcessors)/2;

        // Print a square in the corresponding slot of time.
        for (int x = fromX; x < toX; x++){
            for (int y = fromY; y < toY; y++) {
                image(x,y)->Blue = taskColors[assignments[i].assignedJob->parent].Blue;
                image(x,y)->Green = taskColors[assignments[i].assignedJob->parent].Green;
                image(x,y)->Red = taskColors[assignments[i].assignedJob->parent].Red;
            }
        }

    }

    image.WriteToFile("out.bmp");
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

