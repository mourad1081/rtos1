#include "scheduler.h"
#define IDLE -1

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
    // First variable : Informations about this schedule
    ScheduleInfos scheduleInfos;
    for(int i = 0; i < nbProcessors; i++) {
        scheduleInfos.nbIdlePerProcessor[i] = 0;
        scheduleInfos.nbPreemptionsPerProcessor[i] = 0;
        scheduleInfos.utilisationPerProcessor[i] = 0.0;
    }
    scheduleInfos.nbIdleTotal = 0;
    scheduleInfos.nbPreemptionsTotal = 0;
    scheduleInfos.studyInterval = {0, max};
    scheduleInfos.nbDesiredProcessors = nbProcessors;
    scheduleInfos.nbRequiredProcessors = -1;
    scheduleInfos.failed = false;

    // First, check if U > 100% * nbProcessors
    // OR there is more tasks than processors
    // It will always fail if it is the case.
    if(taskSystem.U() > nbProcessors
            || taskSystem.getTaskSet().size() > nbProcessors) {
        scheduleInfos.failed = true;
        scheduleInfos.optionalMessage = "The scheduling will always fail with "
                                        "that number of processors. "
                                        "Perhaps retry with more.";
    }

    std::vector<Task> &to = taskSystem.getTaskSet();
    for(unsigned int i = 0; i < to.size(); i++) {
        while(to[i].hasNextJob() && !scheduleInfos.failed) {
            Job job = to[i].releaseJob();

            long slotFrom = job.release, freeSlot;
            int freeProcessor, previousNumTaskOfJob;
            while(job.remeaningComputation > 0 && !scheduleInfos.failed) {
                // On cherche un slot libre
                findFreeSlot(slotFrom, &freeSlot, &freeProcessor);
                // On assigne le job à ce slot
                assign(freeProcessor, freeSlot, job.numTaskParent);
                // Une deadline miss occure-t-elle ?
                if(job.remeaningComputation + freeSlot > job.absolute_deadline)
                    scheduleInfos.failed = true;
                else {
                    // On compute
                    job.remeaningComputation--;
                    // On met à jour d'où on recherche
                    slotFrom = freeSlot + 1;
                    // Preemption ?
                    previousNumTaskOfJob = jobAt(freeSlot-1, freeProcessor);
                    if((freeSlot > 0)
                         && (previousNumTaskOfJob != IDLE)
                         && (previousNumTaskOfJob != job.numTaskParent))
                    {
                        scheduleInfos.nbPreemptionsPerProcessor[freeProcessor]++;
                        scheduleInfos.nbPreemptionsTotal++;
                    }
                }
            }
        }
    }

    // Computation of idles & utilisations...
    for(int P = 0; P < nbProcessors; P++) {
        scheduleInfos.nbIdlePerProcessor[P] = max - countAssignments(P);
        scheduleInfos.utilisationPerProcessor[P] =
             (double) (max - scheduleInfos.nbIdlePerProcessor[P])
           / (double) (max);
    }
    scheduleInfos.nbIdleTotal = max*nbProcessors - assignments.size();
    scheduleInfos.utilisationTotal =
             (double) (max*nbProcessors - scheduleInfos.nbIdleTotal)
           / (double) (max*nbProcessors);

    return scheduleInfos;
}


void Scheduler::printInfos(ScheduleInfos &infos)
{
    if(infos.failed) {
        cout << "====== The scheduling has failed ======" << endl;
        cout << " Desired processors : " << infos.nbDesiredProcessors << endl;
        return;
    }
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

string Scheduler::toString()
{
    std::ostringstream res;
    for(Assignment &a : assignments) {
        res << "@ slot " << a.slotTime
            << " on CPU " << a.numProcessor
            << ", job ID = "
            << a.numTaskOfJob
            << endl;
    }
    return res.str();
}


vector<ScheduleInfos> Scheduler::schedulePartitionned()
{
    vector<ScheduleInfos> infos;
    for(SystemTask &to : this->bestFit())
        infos.push_back( Scheduler(to, 1).scheduleGlobal() );
    return infos;
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
    // map <ID task, Color>
    std::map<int, RGBApixel> taskColors;
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
        if((cptDashes % (taskSystem.feasibleInterval().max
                         /taskSystem.getTaskSet().size()) == 0)
                || (i == (padding + lengthTimeAxis-1)))
        {
            PrintString(image, const_cast<char*>(std::to_string(cptDashes).c_str()),
                        i, lengthProcessorAxis + padding + (int)(padding*0.25),
                        8, {0,0,0,0} /* = {Blue, Green, Red, Alpha} */ );
        }
        cptDashes++;
    }

    // Assignments
    for(Assignment &a : assignments) {
        // Assign a random color to the task if it hasn't one yet.
        if(taskColors.find(a.numTaskOfJob) == taskColors.end()) {
            taskColors[a.numTaskOfJob] = {
                (ebmpBYTE) taskSystem.random(1, 256),
                (ebmpBYTE) taskSystem.random(1, 256),
                (ebmpBYTE) taskSystem.random(1, 256),
                (ebmpBYTE) 0
            };
        }
        int fromX, fromY, toX, toY;
        fromX = padding + (lengthTimeAxis/taskSystem.feasibleInterval().max)*a.slotTime;
        fromY = padding + (lengthProcessorAxis/nbProcessors)*a.numProcessor;
        toX   = fromX + (lengthTimeAxis/taskSystem.feasibleInterval().max);
        toY   = fromY + (lengthProcessorAxis/nbProcessors)/2;

        // Print a square in the corresponding slot of time.
        for (int x = fromX; x < toX; x++){
            for (int y = fromY; y < toY; y++) {
                image.SetPixel(x, y, {
                    taskColors[a.numTaskOfJob].Blue,
                    taskColors[a.numTaskOfJob].Green,
                    taskColors[a.numTaskOfJob].Red,
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

    image.WriteToFile(pathFile.c_str());
}

long Scheduler::countAssignments(int P)
{
    return std::count_if(assignments.begin(), assignments.end(),
           [=](Assignment &elt) {
                return elt.numProcessor == P;
    });
}

void Scheduler::findFreeSlot(long from, long *freeSlot, int *freeProcessor)
{
    for(int i = from; from < max; i++) {
        for(int P = 0; P < nbProcessors; P++) {
            if(jobAt(from, P) == IDLE) {
                *freeSlot = from;
                *freeProcessor = P;
                return;
            }
        }
        from++;
    }
    *freeSlot = -1;
    *freeProcessor = -1;
}

std::vector<SystemTask> Scheduler::bestFit()
{
    vector<SystemTask> listPartitions;
    taskSystem.sortByUtilization();
    for(Task &t : taskSystem.getTaskSet()) {
        fit(t, listPartitions);
        if(listPartitions.size() > nbProcessors)
            throw std::string("error. More bins than CPU's");
    }
    return listPartitions;
}

void Scheduler::fit(Task &t, std::vector<SystemTask> &listPartitions)
{
    SystemTask newPartition;
    int indexBest = -1, currentFit, bestFit;
    if(!listPartitions.empty()) {
        indexBest = 0;
        bestFit = t.U() + listPartitions[0].U();
    }
    for(int i = 0; i < listPartitions.size(); i++) {
        currentFit = t.U() + listPartitions[i].U();
        if( (currentFit < 1) && (currentFit < bestFit) ) {
            indexBest = i;
            bestFit = currentFit;
        }
    }

    if(indexBest == -1) {
        newPartition.addTask(t);
        listPartitions.push_back(newPartition);
    } else
        listPartitions[indexBest].addTask(t);
}

int Scheduler::jobAt(long slot, int numProcessor)
{
    std::vector<Assignment>::iterator it = std::find_if(
        assignments.begin(),
        assignments.end(),
        // [=] to indicate that I would like all local
        // variables to be captured by value in a lambda
        [=](Assignment &a) {
            return a.numProcessor == numProcessor
            && a.slotTime == slot;
        }
    );

    return it != assignments.end() ? (*it).numTaskOfJob : IDLE;
}

void Scheduler::assign(int numProcessor, long slot, int numTaskOfJob)
{
    assignments.push_back({ numTaskOfJob, numProcessor, slot });
}
