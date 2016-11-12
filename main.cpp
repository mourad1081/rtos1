#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include "systemtask.h"

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
 * \brief startPartitionnedAssignment
 */
void startPartitionnedAssignment(char *pathFile, int nbProcessors);

/*!
 * \brief startGlobalAssignment
 */
void startGlobalAssignment(char *pathFile, int nbProcessors);

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

// -------------------------------------------------------------------------- //

int strToInt(char *c) {
    char *endPtr;
    int value = std::strtol(c, &endPtr, 10);
    if(endPtr == c)
        throw std::invalid_argument("Error : not a valid argument");
    return value;
}

// ======================== TASK GENERATOR MODULE =========================== //


int taskGenerator(int argc, char *argv[]) {
    int U, nbTasks;
    char *outputFile;

    if(argc < 7) {
        throw invalid_argument("Error. Correct command must be : taskGenerator "
                               "-n NbTasks -u UtilisationFactor -o file");
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
    bool global = false;
    char *pathFile;
    int nbProcessors;

    if(argc < 4) {
        throw std::invalid_argument("Correct command must be : simDM "
                                    "[-g | -p] <tasksFile> <processorsNbr>");
    }
    else {
        global       = !strcmp(argv[1], "-g");
        partitionned = !strcmp(argv[1], "-p");
        if(partitionned == global)
            throw std::invalid_argument("Correct command must be: simDM [-g | "
                                        "-p] <tasksFile> <processorsNbr>");
        pathFile = argv[2];
        nbProcessors = strToInt(argv[3]);
    }
    partitionned ? startPartitionnedAssignment(pathFile, nbProcessors) :
                   startGlobalAssignment(pathFile, nbProcessors);
    return 0;
}

void startPartitionnedAssignment(char * pathFile, int nbProcessors) {
    SystemTask to(pathFile);
}

void startGlobalAssignment(char *pathFile, int nbProcessors) {

}

// ============================= STUDY DM MODULE ============================ //

int studyDM(int argc, char *argv[]) {
    // TODO
    return 0;
}



// ================================= MAIN =================================== //

int main(int argc, char *argv[])
{
    #if defined TASK_GENERATOR_MODULE
        return taskGenerator(argc, argv);
    #elif defined SIM_DM_MODULE
        return simDM(argc, argv);
    #elif defined STUDY_DM_MODULE
        return studyDM(argc, argv);
    #endif
    return 0;
}
// -------------------------------------------------------------------------- //

