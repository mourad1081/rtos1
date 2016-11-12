
#include "systemtask.h"
using namespace std;

SystemTask::SystemTask() { }

SystemTask::SystemTask(std::vector<Task> tasks)
{
    this->taskSet = tasks;
}

SystemTask::SystemTask(int nbTask, double U)
{
    if(U <= 0)
        throw logic_error("error. U must be in the range (0, 100].");

    // We're generating an U close to the original U.
    // Not necessarily equals
    int Urand = random(U*0.95, U*1.05);
    int T, multiplier = 1;

    // Critical case => nbTask > U
    while(nbTask > Urand*multiplier) {
        multiplier++;
    }
    Urand *= multiplier;
    T = 100*multiplier;
    // General case => nbTask < U
    // 1 -- Give 1 WCET to each task
    // 2 -- For i = (nbTask, nbTask+1, ..., U-1, U) :
    //      -> increment WCET in a random task
    // 3 -- C = C / GCD(C, T); T = T / GCD(C, T);
    for(int i = 0; i < nbTask; i++)
        this->taskSet.push_back(Task(random(0, 100), T, random(1, 100), 1));

    for(int i = 0; i < Urand; i++)
        this->taskSet[ random(0, nbTask-1) ].WCET++;
}

SystemTask::SystemTask(char *pathFile)
{
    string line;
    ifstream file(pathFile);
    int T, C, D, O;

    if (file.is_open()) {
        while (file >> O >> T >> D >> C) {
            Task t(O, T, D, C);
            addtask(t);
        }

    } else
        cout << "Unable to open file";
    file.close();

    cout << toString();
}

std::vector<Task> SystemTask::getTaskSet()
{
    return taskSet;
}

int SystemTask::GCD(int a, int b)
{
    while(true)
    {
        if (a == 0) return b;
        b %= a;
        if (b == 0) return a;
        a %= b;
    }
}

int SystemTask::LCM(int a, int b)
{
    return abs(a * b) / GCD(a, b);
}

int SystemTask::random(int min, int max)
{
    std::mt19937 eng(this->rd()); // seed the generator
    std::uniform_int_distribution<> distr(min, max); // define the range
    return distr(eng);
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

int SystemTask::P()
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
    return {0, Omax() + 2*P()};
}

void SystemTask::addtask(Task &t)
{
    this->taskSet.push_back(t);
}

void SystemTask::sortByDeadline()
{
    std::sort(taskSet.begin(), taskSet.end(), [](Task a, Task b)
    {
        return a.deadline < b.deadline;
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

