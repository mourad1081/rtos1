#ifndef TIMELINE_H
#define TIMELINE_H
#include <map>
#include <tuple>
#include "systemtask.h"
#include "easyBMP/EasyBMP.h"
#include "easyBMP/EasyBMP_Font.h"

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
};

struct Assignment {
    Job *assignedJob;
    int numProcessor;
    int slotTime;
};

class Scheduler
{
    private:
        int min;
        int max;
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
         * \brief Assign an amount of time of a job starting
         *        from the minimum slot (second argument).
         * \param j The job to assign to.
         * \param startSlot
         */
        void assignJob(Job *j, int startSlot, int indexProcessor);

        /*!
         * \brief Returns the job on the desired processor at the desired slot
         * \param slot The slot of time
         * \param numProcessor Number of processor
         * \return The assigned job
         */
        Job *jobAt(int slot, int numProcessor);

        /*!
         * \brief Assign a job to a processor and a slot of time
         * \param numProcessor Number of the processor
         * \param slot Slot of time
         * \param job The job to assign
         */
        void assign(int numProcessor, int slot, Job *job);

        ScheduleInfos scheduleGlobal();

        ScheduleInfos schedulePartitionned();

        void exportToBMP(std::string pathFile);

        void printInfos(ScheduleInfos &infos);

        void addProcessor();

        void removeProcessor();
};

#endif // TIMELINE_H
