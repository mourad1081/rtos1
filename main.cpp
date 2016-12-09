#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include "systemtask.h"
#include "scheduler.h"
using namespace std;

// ========================== Functions declaration ========================= //
/*!
 * \brief strToInt
 * \return
 */
int strToInt(char *);

/*!
 * \brief taskGenerator Task generator module.
 *        Usage : taskGenerator -n NbTasks -u UtilisationFactor -o file
 * \param argc number of arguments
 * \param argv arguments
 * \return 0 if command succeeds.
 */
int taskGenerator(int argc, char *argv[]);

/*!
 * \brief simDM
 * \param argc
 * \param argv
 * \return
 */
int simDM(int argc, char *argv[]);

/*!
 * \brief studyDM
 * \param argc
 * \param argv
 * \return
 */
int studyDM(int argc, char *argv[]);

/*!
 * \brief main Main function that produces all modules according to
 * the specified flag during compilation.
 * Compile with -DSIM_DM_MODULE to produce "simDM" module,
 *              -DTASK_GENERATOR_MODULE to produce "taskGeneration" module and
 *              -STUDY_DM_MODULE for "studyDM" module.
 * \param argc number of arguments
 * \param argv arguments
 */
int main(int argc, char *argv[]);

static const string INVALID_ARGUMENT = "Error : not a valid argument";

static const string ERROR_TASK_GENERATOR = "Error. Correct command must be : "
                                           "taskGenerator -n NbTasks -u "
                                           "UtilisationFactor -o file";

static const string ERROR_SIM_DM = "Correct command must be : simDM "
                                   "[-g | -p] <tasksFile> <processorsNbr>";
// -------------------------------------------------------------------------- //

int strToInt(char *c) {
    char *endPtr;
    int value = std::strtol(c, &endPtr, 10);
    if(endPtr == c)
        throw std::invalid_argument(INVALID_ARGUMENT);
    return value;
}





// ======================== TASK GENERATOR MODULE =========================== //


int taskGenerator(int argc, char *argv[]) {
    cout << "Here ????" << endl;
    int U, nbTasks;
    char *outputFile;

    if(argc < 7) {
        throw invalid_argument(ERROR_TASK_GENERATOR);
    }
    else {
        for (int i = 1; i < argc; i++) {
            if (i + 1 != argc) {
                if (!strcmp(argv[i], "-u"))
                    U = strToInt(argv[i + 1]);
                else if (!strcmp(argv[i], "-n"))
                    nbTasks = strToInt(argv[i + 1]);
                else if (!strcmp(argv[i], "-o"))
                    outputFile = argv[i + 1];
            }
        }
    }
    SystemTask t(nbTasks, U);
    ofstream myfile;
    myfile.open (outputFile);
    myfile << t.toString();
    myfile.close();
    return 0;
}






// ============================= SIM DM MODULE ============================== //

int simDM(int argc, char *argv[]) {

    bool partitionned = false;
    bool global       = false;
    bool image        = false;
    char *pathFile;
    int nbProcessors;
    // Parsing arguments
    if(argc < 4)
        throw std::invalid_argument(ERROR_SIM_DM);
    else {
        global       = !strcmp(argv[1], "-g");
        partitionned = !strcmp(argv[1], "-p");
        pathFile     = argv[2];
        nbProcessors = strToInt(argv[3]);
        if(argc > 4)
            image    = !strcmp(argv[4], "-i");
        if(partitionned == global)
            throw std::invalid_argument(ERROR_SIM_DM);

    }
    // Creation of the system tasks
    SystemTask to(pathFile);

    // Priority assignment
    ScheduleInfos log, logRequired;
    vector<ScheduleInfos> *logsPartitionned;
    Scheduler *scheduler = new Scheduler(to, nbProcessors);
    if(partitionned) {
        try {
            logsPartitionned = scheduler->schedulePartitionned();
            cout << Colors::cyan << "Number of partitions : " << logsPartitionned->size() << Colors::reset << endl;
            for(int i = 0; i < logsPartitionned->size(); i++) {
                cout << Colors::cyan << "PARTITION " << i << Colors::reset << endl;
                logsPartitionned->at(i).nbRequiredProcessors = 1;
                Scheduler::printInfos(logsPartitionned->at(i));
                cout << endl << endl;
            }
        } catch(string e) {
            cerr << e << endl;
        }
    } else {
        // 1. schedule global with desired parameters.
        // 2. schedule global from U to nbtask and stop as soon as it succeeds
        // 3. As soon as it succeeds, that schedule contains the
        //    required number of processors.
        // 4. If fail, the system cannot be scheduled using global asignment.
        // 5. Otherwise, print log
        //   (of the first scheduling at step 1 if it has succeeded.
        //   Otherwise, print the log of the scheduling that succeed at step 2.)
        log = scheduler->scheduleGlobal();
        int i = std::ceil(to.U());
        logRequired.failed = true;
        if(log.failed) {
            cout << "The system has failed with "
                 << nbProcessors << " processor(s). Retrying with more..."
                 << endl;
        }

        while(logRequired.failed && i <= to.getTaskSet().size())
        {
            to.createJobs();
            scheduler = new Scheduler(to, i);
            logRequired = scheduler->scheduleGlobal();
            if(logRequired.failed) {
                cout << "G FAIL AVEC " << i << " CPUUU" << endl;
                cout << logRequired.optionalMessage << endl;
            }
            i++;
        }

        if(logRequired.failed) {
            cout << "The system cannot be scheduling using global"
                 << " asignment and DM scheduling."
                 << endl;
        } else {
            // Update desired/required #processors.
            log.nbRequiredProcessors = logRequired.nbDesiredProcessors;
            logRequired.nbRequiredProcessors = logRequired.nbDesiredProcessors;
            logRequired.nbDesiredProcessors = log.nbDesiredProcessors;

            log.failed ? Scheduler::printInfos(logRequired)
                       : Scheduler::printInfos(log);
        }

    }
    if(image)
        scheduler->exportToBMP("global.bmp");
    return 0;
}







// ============================= STUDY DM MODULE ============================ //

int studyDM(int argc, char *argv[]) {
    // TODO
    return 0;
}






// ================================= MAIN =================================== //

int main(int argc, char *argv[])
{
    #if defined(TASK_GENERATOR_MODULE)
        return taskGenerator(argc, argv);
    #elif defined(SIM_DM_MODULE)
        return simDM(argc, argv);
    #elif defined(STUDY_DM_MODULE)
        return studyDM(argc, argv);
    #endif
    return -1;
}
// -------------------------------------------------------------------------- //

