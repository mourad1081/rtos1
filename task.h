#ifndef TASK_H
#define TASK_H
#include <stdexcept>
#include <queue>
#include "job.h"

class Task {

  private:
    std::queue<Job> jobs;

  public:
    Task(int O, int T, int D, int C);
    Task();
    int period;
    int deadline;
    int offset;
    int WCET;
};

#endif // TASK_H
