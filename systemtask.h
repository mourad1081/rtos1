#ifndef SYSTEMTASK_H
#define SYSTEMTASK_H
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <functional>
#include <iomanip>
#include <sstream>
#include <random>

#include "task.h"

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
    int GCD(int a, int b);

    /*!
     * \brief Computes the least common multiple of two values
     * @param a First value
     * @param b Second value
     * \return The least common multiple
     */
    int LCM(int a, int b);

    /*!
     * \brief Generate a random integer according to the given arguments.
     * \param min The minimum value (included in the bound)
     * \param max the maximal value (excluded from the bound)
     * \return A random integer in the interval [min, max[
     */
    int random(int min, int max);
    std::random_device rd; // obtain a random number from hardware
public:

    /*!
     * \brief Create a system with an empty set of task
     */
    SystemTask();

    /*!
     * \brief create a system with given set of tasks
     * \param tasks The set of tasks
     */
    SystemTask(std::vector<Task> tasks);

    /*!
     * \brief Create a system with a random set of task
     * according to the arguments.
     * \param nbTask The desired number of tasks
     * \param U The desired utilisation factor
     */
    SystemTask(int nbTask, double U);

    /*!
     * \brief Get the utilisation factor in percent
     * @throws std::string If there is no task in the set
     * \return The utilisation factor
     */
    double U();

    /*!
     * \brief Get the maximal offset of the set of tasks
     * @throws std::string If there is no task in the set
     * \return The maximal offset of the set of tasks
     */
    int Omax();

    /*!
     * \brief Returns the period of the sytem.
     * P is defined as LCM(to) where
     * "to" is the set of task of this system.
     * @throws std::string If there is no task in the set
     * \return The period of the sytem.
     */
    int P();

    /*!
     * \brief Adds a task in the set
     * \param t the task
     */
    void addtask(Task &t);

    /*!
     * \brief Sorts the task set by deadline.
     * (Smaller the deadline, hihger the index/priority)
     */
    void sortByDeadline();

    /*!
     * \brief Returns this object in a string format
     * \return This object in string format
     */
    std::string toString();

    /*!
     * \brief Since minGW has issue with std::to_string,
     * I've made my own function to convert int's to strings.
     * \param a The value to convert
     * \return a in std::string format
     */
    std::string intToStr(int a);
};



#endif // SYSTEMTASK_H
