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


ScheduleInfos Scheduler::scheduleGlobal()
{
    Job *highestPriorityJob, *previousJob;
    // Clear the previous scheduling
    assignments.clear();
    // First variable : Informations about this schedule
    ScheduleInfos scheduleInfos;
    for(int i = 0; i < nbProcessors; i++) {
        scheduleInfos.nbIdlePerProcessor[i] = 0;
        scheduleInfos.nbPreemptionsPerProcessor[i] = 0;
        scheduleInfos.utilisationPerProcessor[i] = 0.0;
    }
    scheduleInfos.studyInterval = {0, taskSystem.feasibleInterval().max};
    scheduleInfos.nbDesiredProcessors = nbProcessors;
    scheduleInfos.nbRequiredProcessors = -1;
    scheduleInfos.failed = false;
    // Lambda that represents how jobs must be compared
    auto comparator = [](Job * left, Job *right) -> bool {
        return left->absolute_deadline > right->absolute_deadline;
    };

    // Represents all pending jobs.
    std::priority_queue<Job *, std::vector<Job *>,
                        decltype(comparator)> jobs_to_execute(comparator);
    
    // For each slot of time and while the scheduling hasn't failed
    for(int t = 0;
        t < taskSystem.feasibleInterval().max && !scheduleInfos.failed;
        t++)
    {
        // All tasks that must release a job do it.
        // (release time of the next job of a task >= i)
        for(Task &task : taskSystem.getTaskSet())
            if (task.hasNextJob() && task.nextJob().release == t)
                jobs_to_execute.push(&task.releaseJob());

        // And then, for each processors and while the scheduling hasn't failed
        for(int P = 0; P < nbProcessors && !scheduleInfos.failed; P++) {
            // Is there any job to execute ?
            if(! jobs_to_execute.empty()) {
                highestPriorityJob = jobs_to_execute.top();
                assign(P, t, highestPriorityJob);
                jobs_to_execute.pop();
                // Deadline will miss ?
                scheduleInfos.failed = highestPriorityJob->absolute_deadline
                           < (t + highestPriorityJob->remeaningComputation);
                // Use Processor
                highestPriorityJob->remeaningComputation--;
                // A preemption occured ?
                previousJob = jobAt(t-1, P);
                if((t > 0) && (previousJob != IDLE)
                           && (previousJob != jobAt(t, P)))
                {
                    scheduleInfos.nbPreemptionsPerProcessor[P]++;
                    scheduleInfos.nbPreemptionsTotal++;
                }
            } else {
                // There is no job to execute.
                scheduleInfos.nbIdlePerProcessor[P]++;
                scheduleInfos.nbIdleTotal++;
            }
        }

        // All not finished jobs at time t returns to the execution queue
        for(int P = 0; P < nbProcessors; P++) {
            Job *j = jobAt(t, P);
            if(j != IDLE && j->remeaningComputation > 0)
                jobs_to_execute.push(j);
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


void Scheduler::printInfos(ScheduleInfos &infos)
{
    if(infos.failed)
        cerr << "The scheduling has failed dammit" << endl;
    cout << "+---------------------------------------+" << endl
         << "Infos : " << endl
         << "Desired Processors : "    << infos.nbDesiredProcessors  << endl
         << "Required Processor(s) : " << infos.nbRequiredProcessors << endl
         << "Study interval : ["       << infos.studyInterval.min    << ", "
         << infos.studyInterval.max    << "]"                        << endl
         << "Global utilisation : "    << infos.utilisationTotal     << endl
         << "--------------------"     << endl;

    for(auto &entry : infos.utilisationPerProcessor) {
        cout << "Utilisation of processor " << entry.first
             << " : " << entry.second       << endl;
    }

    cout << "Total idle time : " << infos.nbIdleTotal << endl
         << "-----------------"  << endl;

    for(auto &entry : infos.nbIdlePerProcessor) {
        cout << "Idle time of processor " << entry.first
             << " : " << entry.second     << endl;
    }

    cout << "Total number of preemptions : " << infos.nbPreemptionsTotal << endl
         << "-----------------------------"  << endl;

    for(auto &entry : infos.nbPreemptionsPerProcessor) {
        cout << "Number of preemption of processor " << entry.first
             << " : " << entry.second                << endl;
    }

}

void Scheduler::addProcessor()
{
    nbProcessors++;
}

void Scheduler::removeProcessor()
{
    nbProcessors--;
}


ScheduleInfos Scheduler::schedulePartitionned()
{
    return {};
}

void Scheduler::exportToBMP(string pathFile)
{
    int w = 1300, h = 700, padding = 50;
    int lengthTimeAxis      = w - 2*padding;
    int lengthProcessorAxis = h - 2*padding;
    int widthAxis    =  3;
    int cptProcessor =  1;
    std::string indexProcessor;
    BMP image;
    std::map<Task *, RGBApixel> taskColors;
    image.SetSize(w, h);

    // Time axis
    for(int i = padding; i < padding + lengthTimeAxis; i++) {
        for(int j = h - padding - (int)(widthAxis/2);
            j < h - padding + (int)(widthAxis/2);
            j++)
        {
            image.SetPixel(i, j, {0, 0, 0, 0});
        }
    }

    // grid on time axis
    int offset = lengthTimeAxis / taskSystem.feasibleInterval().max;
    int cptDashes = 0;
    for(int i = padding; i < padding + lengthTimeAxis; i += offset)
    {
        for(int j =  padding;
                j <  lengthProcessorAxis + padding;
                j++)
        {
            image.SetPixel(i, j, {210, 210, 210, 0});
        }
        if((cptDashes % (taskSystem.feasibleInterval().max/taskSystem.getTaskSet().size()) == 0) || (i == (padding + lengthTimeAxis-1)))
            PrintString(image, const_cast<char*>(std::to_string(cptDashes).c_str()),
                        i, lengthProcessorAxis + padding + (int)(padding*0.25),
                        8, {0,0,0,0} /* = {Blue, Green, Red, Alpha} */ );
        cptDashes++;
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
                image.SetPixel(x, y, {
                    taskColors[assignments[i].assignedJob->parent].Blue,
                    taskColors[assignments[i].assignedJob->parent].Green,
                    taskColors[assignments[i].assignedJob->parent].Red,
                    0
                });
            }
        }
    }

    // grid on processor axis
    offset = lengthProcessorAxis / nbProcessors;
    for(int i = padding; i < padding + lengthProcessorAxis; i += offset)
    {
        for(int j =  padding;
                j <  padding + lengthTimeAxis;
                j++)
        {
            image.SetPixel(j, i, {120, 120, 120, 0});
        }
        indexProcessor = std::string("Pi " + std::to_string(cptProcessor));
        PrintString(image, const_cast<char*>(indexProcessor.c_str()),
                    (int)(padding*0.25), i, 10, {0,0,0,0});
        cptProcessor++;
    }

    // Processors axis
    for(int i = padding; i < padding + lengthProcessorAxis; i++) {
        for(int j = padding - (int)(widthAxis/2);
                j < padding + (int)(widthAxis/2);
                j++)
        {
            image.SetPixel(j,i, {0, 0, 0, 0});
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

    return it != assignments.end() ? (*it).assignedJob : IDLE;
}

void Scheduler::assign(int numProcessor, int slot, Job *job)
{
    assignments.push_back({ job, numProcessor, slot });
}

