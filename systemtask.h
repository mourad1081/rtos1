#ifndef SYSTEMTASK_H
#define SYSTEMTASK_H
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iostream>
#include <cmath>
#include <fstream>
#include <algorithm>
#include <functional>
#include <iomanip>
#include <sstream>
#include <random>

#include "task.h"

struct Interval {
    long min;
    long max;
};

class SystemTask
{
private:

    /*!
     * \brief The set of tasks
     */
    std::vector<Task> taskSet;


    /*!
     * @brief Computes the grather common divisor of two values
     * @param a First value
     * @param b Second value
     * @return The grather common divisor
     */
    long GCD(long a, long b);

    /*!
     * \brief Computes the least common multiple of two values
     * @param a First value
     * @param b Second value
     * \return The least common multiple
     */
    long LCM(long a, long b);


    std::random_device rd; // obtain a random number from hardware
public:

    SystemTask();
    /*!
     * \brief Copy constructor
     * \param toCopy
     */
    SystemTask(SystemTask const& toCopy);

    /*!
     * \brief create a system with given set of tasks
     * \param tasks The set of tasks
     */
    SystemTask(std::vector<Task> &tasks);

    /*!
     * \brief Create a system with a random set of task
     *        according to the arguments.
     * \param nbTask The desired number of tasks
     * \param U The desired utilisation factor
     */
    SystemTask(int nbTask, double U);

    /*!
     * \brief Create a system according to a given set of tasks defined
     *        in a file.
     *        Note : I assume that the given file is correctly structured.
     * \param pathFile Path to the file that describes the set of tasks.
     */
    SystemTask(char *pathFile);

    /*!
     * \brief Get the utilisation factor
     * \return The utilisation factor
     */
    double U();

    /*!
     * \brief Get the maximal offset of the set of tasks
     * \return The maximal offset of the set of tasks
     */
    int Omax();

    /*!
     * \brief Omin
     * \return
     */
    int Omin();


    /*!
     * \brief Generate the minimal set of jobs for each tasks of the system.
     *        The minimal set of jobs corresponds to tha maximum release of jobs
     *        of a task during the feasible interval.
     */
    void createJobs();

    /*!
     * \brief Returns the hyper-period of the sytem.
     *        P is defined as LCM(to) where
     *        to is the set of task of this system.
     * \return The period of the sytem.
     */
    long P();

    /*!
     * \brief Returns the feasible interval of the system.
     * \return The feasible interval of the system.
     */
    Interval feasibleInterval();

    /*!
     * \brief Adds a task in the set
     * \param t the task
     */
    void addTask(Task &t);

    /*!
     * \brief Sorts the task set by deadline.
     *        (Smaller the deadline, hihger the priority, lower the index)
     */
    void assignPriority();

    /*!
     * \brief Sorts the system task by (decreasing) utilization
     */
    void sortByUtilization();

    /*!
     * \brief Returns this object in a string format
     * \return This object in string format
     */
    std::string toString();

    /*!
     * \brief Since minGW has issue with std::to_string,
     *        I've made my own function to convert int's to strings.
     * \param a The value to convert
     * \return a in std::string format
     */
    std::string intToStr(int a);

    /*!
     * \brief getTaskSet
     * \return
     */
    std::vector<Task> &getTaskSet();

    /*!
     * \brief Generate a random integer according to the given arguments.
     * \param min The minimum value (included in the bound)
     * \param max the maximal value (excluded from the bound)
     * \return A random integer in the interval [min, max[
     */
    int random(int min, int max);
};


#endif // SYSTEMTASK_H
