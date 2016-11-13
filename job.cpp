#include "job.h"

Job::Job()
{

}

Job::Job(int a, int e, int d)
{
    this->computation = e;
    this->release = a;
    this->deadline = d;
    this->remeaningComputation = computation;
}
