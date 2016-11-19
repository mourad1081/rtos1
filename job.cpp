#include "job.h"

Job::Job()
{

}

Job::Job(int a, int e, int d)
{
    this->release = a;
    this->computation = e;
    this->deadline = d;
    this->remeaningComputation = computation;
}

Job::Job(int a, int e, int d, int ad)
{
    this->release = a;
    this->computation = e;
    this->deadline = d;
    this->absolute_deadline = ad;
    this->remeaningComputation = computation;
}
