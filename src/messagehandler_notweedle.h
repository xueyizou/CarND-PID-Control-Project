#ifndef MESSAGEHANDLER_NOTWEEDLE_H
#define MESSAGEHANDLER_NOTWEEDLE_H
#include <uWS/uWS.h>
#include <iostream>
#include "json.hpp"
#include "PID.h"

class MessageHandler_NoTweedle
{
private:
    PID pid;
public:
    MessageHandler_NoTweedle(double * p);
    void operator()(uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, uWS::OpCode opCode);
};

#endif // MESSAGEHANDLER_NOTWEEDLE_H
