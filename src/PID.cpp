#include "PID.h"
#include "iostream"

using namespace std;

PID::PID()
{
}

PID::~PID() {}

void PID::Init(double * p)
{
    Kp=p[0];
    Ki=p[1];
    Kd=p[2];

    p_error = 0;
    i_error = 0;
    d_error = 0;

    total_sq_cte = 0;
    counter=0;
}

void PID::UpdateError(double cte)
{
    if(counter==0)
    {
       p_error = cte;
    }
    else
    {
        d_error = cte - p_error;
        i_error += cte;
        p_error = cte;
    }
    counter++;

    //start to accumulate pid control error after 50 steps.
    if(counter>=50)
    {
        total_sq_cte +=cte*cte;
    }
}

double PID::getControl()
{
    /*
    * Calcuate steering value here.
    */
    double steer = -Kp*p_error -Ki*i_error -Kd*d_error;
//    steer = steer < -1 ? -1 : steer;
//    steer = steer > 1 ? 1 : steer;
    return steer;
}

double PID::TotalError()
{
    //return the mean squred error in steps [50 ,1000]
    return total_sq_cte/950.0;
}

