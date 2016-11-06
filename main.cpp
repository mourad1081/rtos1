#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include "systemtask.h"

using namespace std;

void error() {
    cerr << "Usage : taskGenerator -n NbTasks -u UtilisationFactor [-o file]"
         << endl
         << "Exemple : taskGenerator -n 10 -u 80 -o out.txt"
         << endl;
}

int strToInt(char *c) {
    char *endPtr;
    int value = std::strtol(c, &endPtr, 10);
    if(endPtr == c)
        throw std::invalid_argument("Error : not a valid argument");
    return value;
}

int taskGenerator(int argc, char *argv[]) {
    int U, nbTasks;
    char *outputFile;

    if(argc < 7) {
        error();
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
    cout << system("pwd") << endl;
    myfile.open (outputFile);
    myfile << t.toString();
    myfile.close();
    return 0;
}

int simDM(int argc, char *argv[]) {}
int studyDM(int argc, char *argv[]) {}

int main(int argc, char *argv[])
{
    #if defined TASK_GENERATOR_MODULE
        return taskGenerator(argc, argv);
    #elif defined SIM_DM_MODULE
        return simDM(argc, argv);
    #elif defined STUDY_DM_MODULE
        return studyDM(argc, argv);
    #endif
}


