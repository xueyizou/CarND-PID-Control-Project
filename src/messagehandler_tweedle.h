#ifndef MESSAGEHANDLER_TWEEDLE_H
#define MESSAGEHANDLER_TWEEDLE_H

#include <uWS/uWS.h>
#include <iostream>
#include "json.hpp"
#include "PID.h"
#include <string>

class MessageHandler_Tweedle
{
private:
    PID pid;
    double p[3];
    double dp[3];
    std::string state; //["init", "up", "down"], denoting the stage of the tweedle procedure.
    int idx; // index which the pid parameter {kp, ki, kd} in tuning
    int counter;
    int max_counter; // the max simulation steps before getting the pid error
    double best_sq_cte;
    int itr;

public:
    MessageHandler_Tweedle();
    void operator()(uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, uWS::OpCode opCode);

};
#endif // MESSAGEHANDLER_TWEEDLE_H
