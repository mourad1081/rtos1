#ifndef TIMELINE_H
#define TIMELINE_H
#include <map>
#include <tuple>
#include <sstream>
#include "systemtask.h"
#include "easyBMP/EasyBMP.h"

/*!
 * \brief The ScheduleInfos struct is used to store all stats after a scheduling
 */
struct ScheduleInfos {
    int nbIdleTotal;
    // key : num processor, value : nb idle for that processor
    std::map<int, int> nbIdlePerProcessor;
    double utilisationTotal;
    std::map<int, double> utilisationPerProcessor;
    int nbPreemptionsTotal;
    std::map<int, int> nbPreemptionsPerProcessor;
    int nbRequiredProcessors;
    int nbDesiredProcessors;
    Interval studyInterval;
    bool failed;
    std::string optionalMessage;
};

struct Assignment {
    int numTaskOfJob;
    int numProcessor;
    long slotTime;
};

class Scheduler
{
    private:
        long min;
        long max;
        int nbProcessors;
        /*!
         * \brief The system of tasks to schedule
         */
        SystemTask &taskSystem;

        /*!
         * \brief To each slot of time, we assign a job to a processor.
         */
        std::vector<Assignment> assignments;
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
         * \brief Returns the job on the desired processor at the desired slot
         * \param slot The slot of time
         * \param numProcessor Number of processor
         * \return The assigned job
         */
        int jobAt(long slot, int numProcessor);

        /*!
         * \brief Assign a job to a processor and a slot of time
         * \param numProcessor Number of the processor
         * \param slot Slot of time
         * \param job The job to assign
         */
        void assign(int numProcessor, long slot, int numTaskOfJob);

        /*!
         * \brief scheduleGlobal
         * \return
         */
        ScheduleInfos scheduleGlobal();

        /*!
         * \brief schedulePartitionned
         * \return
         */
        std::vector<ScheduleInfos> schedulePartitionned();

        /*!
         * \brief exportToBMP
         * \param pathFile
         */
        void exportToBMP(std::string pathFile = "scheduling.bmp");

        /*!
         * \brief countAssignments
         * \param P
         * \return
         */
        long countAssignments(int P);

        /*!
         * \brief (Used in scheduleGlobal()) Finds free slots on
         *        the interval [0, Omax + 2P].
         * \param from
         * \param freeSlot
         * \param freeProcessor
         */
        void findFreeSlot(long from, long *freeSlot, int *freeProcessor);

        /*!
         * \brief Produce a list of task system according to this system.
         *        The list is produced using the best fit algorithm
         *        of the bin packing problem.
         * \return
         */
        std::vector<SystemTask> bestFit();

        /*!
         * \brief Put the task t in a task system regarding
         *        the best fit algorithm.
         * \param t The task to add in a task system.
         * \param listPartitions The list of task system.
         */
        void fit(Task &t, std::vector<SystemTask> &listPartitions);

        /*!
         * \brief printInfos
         * \param infos
         */
        static void printInfos(ScheduleInfos &infos);

        /*!
         * \brief toString
         * \return
         */
        std::string toString();
};

#endif // TIMELINE_H
