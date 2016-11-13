#ifndef TIMELINE_H
#define TIMELINE_H
#include <map>
#include <vector>
#include "systemtask.h"

class Scheduler
{
    private:
        int min;
        int max;
        int nbProcessors;


        /*!
         * \brief Indicate all deadlines of a task in the interval [min, max]
         *        To each task, we assign a list of deadlines
         *        (their occurence in the timeline).
         */
        std::map<Task *, std::vector<int>> tasks_deadlines;

        /**
         * \brief Same as tasks_deadlines but for releases.
         */
        std::map<Task *, std::vector<int>> tasks_releases;

        /*!
         * \brief The system of tasks to schedule
         */
        SystemTask &taskSystem;

        /*!
         * \brief To each slot of time, we assign a job to a processor.
         *        -> First  : The assigned job
         *        -> Second : The processor index
         *        -> Third  : The slot number (time)
         *        Example : the tuple (0x09332f, 4, 32) means
         *                  the job 0x09332f is assigned to the fourth processor
         *                  at time t = 32
         */
        std::vector<std::tuple<Job *, int, int>> assignments;

    public:

        /*!
         * \brief Constructs a timeline where the min and max value of the
         *        interval corresponds to [0, Omax+2P] of the system.
         *        Tasks and their deadlines are automatically
         *        added to the timeline.
         * \param to The system tasks.
         */
        Scheduler(SystemTask &to);

        /*!
         * \brief Constructs a timeline where the min and max value of the
         *        interval corresponds to [0, Omax+2P] of the system.
         *        Tasks and their deadlines are automatically
         *        added to the timeline.
         *        The timeline also creates a defined number of processors
         * \param to The system tasks.
         * \param nbProcessors Number of processors used on the timeline.
         */
        Scheduler(SystemTask &to, int nbProcessors);

        /*!
         * \brief Assign an amount of time of a job starting
         *        from the minimum slot (second argument).
         * \param j The job to assign to.
         * \param startSlot
         */
        void assignJob(Job *j, int startSlot, int indexProcessor);

        void scheduleGlobal();

        void schedulePartitionned();

        void resetSchedule();
};

#endif // TIMELINE_H
