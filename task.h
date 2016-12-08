#ifndef TASK_H
#define TASK_H
#include <stdexcept>
#include <queue>

class Task;

struct Job {
    long release;
    int computation;
    int deadline;
    long absolute_deadline;
    // The remeaning amount of time needed to finish the job.
    int remeaningComputation;
    int numTaskParent;
};

class Task {

  private:
    std::queue<Job> jobs;
  public:
    Task(int O, int T, int D, int C);
    Task(int O, int T, int D, int C, int num);
    int period;
    int deadline;
    int offset;
    int WCET;
    int num; // Number of the task (for instance : task number 9)
    void addJob(Job j);

    /*!
     * \brief U
     * \return The utilization of the task
     */
    double U();

    /*!
     * \brief Releases the last job in the queue.
     * \return The last job in the queue.
     */
    Job &releaseJob();

    /*!
     * \brief Returns the latest job in the queue without releasing (poping) it.
     * \return The last job in the queue.
     */
    Job &nextJob();

    /*!
     * \brief hasNextJob
     * \return
     */
    bool hasNextJob();
};

#endif // TASK_H
