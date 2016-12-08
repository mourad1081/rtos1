#include "systemtask.h"
using namespace std;

SystemTask::SystemTask(const SystemTask &toCopy)
{
    this->taskSet = toCopy.taskSet;
    this->rd();
}


SystemTask::SystemTask(std::vector<Task> &tasks)
{
    this->taskSet = tasks;
    createJobs();
}

SystemTask::SystemTask(int nbTask, double U)
{
    int randIndex = 0, tmpC = 0, gcd = 0;
    if(U <= 0)
        throw logic_error("error. U must be greather than zero.");

    if(U > 100*nbTask)
        cout << "\033[0;36m" << "Warning : Asking for an utilisation greather "
                                "than nbTask * 100% will always fail if you "
                                "schedule the system. However, the system will"
                                " be generated now." << "\033[0m" << endl;

    // We're generating an U close to the original U.
    // Not necessarily equals
    int Urand = random(U*0.96, U);
    int T, multiplier = 1;

    // Critical case => nbTask > U
    while(nbTask > Urand*multiplier) {
        multiplier++;
    }
    Urand *= multiplier;
    T = 100*multiplier;

    // General case => nbTask < U
    // 1) -- Give 1 WCET to each task
    for(int i = 0; i < nbTask; i++)
        this->taskSet.push_back(Task(random(0, 100), T, 1, 1, i));

    // 2) -- For i = (nbTask, nbTask+1, ..., U-1, U) :
    //          -> increment WCET in a random task
    for(int i = nbTask; i < Urand; i++) {
        randIndex = random(0, nbTask-1);
        // We do not give more than 100% of utilisation on a random task.
        // So, we affect to randIndex a random index where
        // the WCET of the random task is lesser than T.
        // (We do that only if U < 100% * nbTasks.)
        // The 2nd condition is there to exclude the case where U > 100% * nbTask
        if(Urand > T && (Urand <= T * nbTask))
            while(this->taskSet[randIndex = random(0, nbTask-1)].WCET == T);
        this->taskSet[randIndex].WCET++;
    }
    // 3) Make the fraction C/T irreductible (for a smaller period)
    // And generate a decent deadline for each task. (i.e. between C and T)
    for(int i = 0; i < nbTask; i++) {
        tmpC = taskSet[i].WCET;
        // Generate random deadline in interval [C, T]

        gcd = GCD(tmpC, taskSet[i].period);
        // make its C/T irreductible
        taskSet[i].WCET   /= gcd;
        taskSet[i].period /= gcd;
        taskSet[i].deadline = random(taskSet[i].WCET, taskSet[i].period);
    }
    createJobs();
}

SystemTask::SystemTask(char *pathFile)
{
    string line;
    ifstream file(pathFile);
    int T, C, D, O, cpt = 0;

    if (file.is_open()) {
        while (file >> O >> T >> D >> C) {
            Task t(O, T, D, C, cpt++);
            addTask(t);
        }

    } else
        throw logic_error("Unable to open file");

    file.close();
    createJobs();
}

std::vector<Task> &SystemTask::getTaskSet()
{
    return taskSet;
}

long SystemTask::GCD(long a, long b)
{
    while(true)
    {
        if (a == 0) return b;
        b %= a;
        if (b == 0) return a;
        a %= b;
    }
}

long SystemTask::LCM(long a, long b)
{
    return abs(a * b) / GCD(a, b);
}

SystemTask::SystemTask()
{

}

int SystemTask::random(int min, int max)
{
    if(min >= max)
        return min;
    std::mt19937 eng(this->rd()); // seed the generator
    std::uniform_int_distribution<> distr(min, max); // define the range
    return distr(eng);
}

void SystemTask::createJobs()
{
    int maxInterval = feasibleInterval().max;
    int nbJobs;
    for(unsigned int i = 0; i < taskSet.size(); i++) {
        nbJobs = (int) ((maxInterval - taskSet[i].offset) / taskSet[i].period);
        for(int j = 0; j < nbJobs; j++) {
            taskSet[i].addJob({
                taskSet[i].offset + j*taskSet[i].period,
                taskSet[i].WCET,
                taskSet[i].deadline,
                taskSet[i].offset + j*taskSet[i].period + taskSet[i].deadline,
                taskSet[i].WCET,
                taskSet[i].num
            });
        }
    }
}

double SystemTask::U()
{
    if(taskSet.empty())
        throw std::length_error("error : no taks in the set");

    double U = 0;
    for(unsigned int i = 0; i < this->taskSet.size(); i++) {
        U += ((double)taskSet[i].WCET / (double)taskSet[i].period);
    }
    return U;
}

int SystemTask::Omax()
{
    if(taskSet.empty())
        throw std::length_error("error : no taks in the set");

    int Omax = taskSet[0].offset;
    for(unsigned int i = 1; i < this->taskSet.size(); i++) {
        Omax = (Omax < taskSet[i].offset) ? taskSet[i].offset : Omax;
    }
    return Omax;
}

int SystemTask::Omin()
{
    if(taskSet.empty())
        throw std::length_error("error : no taks in the set");

    int Omax = taskSet[0].offset;
    for(unsigned int i = 1; i < this->taskSet.size(); i++) {
        Omax = (Omax > taskSet[i].offset) ? taskSet[i].offset : Omax;
    }
    return Omax;
}

long SystemTask::P()
{
    if(taskSet.empty())
        throw std::length_error("error : no taks in the set");

    int lcm = taskSet[0].period;
    for(unsigned int i = 1; i < taskSet.size(); i++) {
        lcm = LCM(lcm, taskSet[i].period);
    }
    return lcm;
}

Interval SystemTask::feasibleInterval()
{
    return {Omax(), Omax() + 2*P()};
}

void SystemTask::addTask(Task &t)
{
    this->taskSet.push_back(t);
}

void SystemTask::assignPriority()
{
    std::sort(taskSet.begin(), taskSet.end(), [](Task a, Task b)
    {
        return a.deadline < b.deadline;
    });
}

void SystemTask::sortByUtilization()
{
    std::sort(taskSet.begin(), taskSet.end(), [](Task a, Task b)
    {
        return a.U() > b.U();
    });
}



string SystemTask::toString()
{
    std::string s = "";
    for(unsigned int i = 0; i < taskSet.size(); i++) {
        s += intToStr(taskSet[i].offset)   + " "
           + intToStr(taskSet[i].period)   + " "
           + intToStr(taskSet[i].deadline) + " "
           + intToStr(taskSet[i].WCET)     + " "
           + "\n" ;
    }
    return s;
}

string SystemTask::intToStr(int a)
{
    return static_cast<ostringstream*>(&(ostringstream() << a))->str();
}

