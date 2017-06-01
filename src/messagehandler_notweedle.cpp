#include "messagehandler_notweedle.h"
#include "toolfuncs.h"

using namespace std;
using json = nlohmann::json;

MessageHandler_NoTweedle::MessageHandler_NoTweedle(double *p)
{
    pid.Init(p);
}

void MessageHandler_NoTweedle::operator()(uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, uWS::OpCode opCode)
{
  // "42" at the start of the message means there's a websocket message event.
  // The 4 signifies a websocket message
  // The 2 signifies a websocket event
  if (length && length > 2 && data[0] == '4' && data[1] == '2')
  {
    auto s = hasData(std::string(data).substr(0, length));
    if (s != "")
    {
      auto j = json::parse(s);
      std::string event = j[0].get<std::string>();
      if (event == "telemetry")
      {
        // j[1] is the data JSON object
        double cte = std::stod(j[1]["cte"].get<std::string>());
        double speed = std::stod(j[1]["speed"].get<std::string>());
        double angle = std::stod(j[1]["steering_angle"].get<std::string>());

        pid.UpdateError(cte);
        double steer_value = pid.getControl();
        if(steer_value>1.0)
        {
            steer_value=1.0;
        }
        else if(steer_value<-1.0)
        {
            steer_value=-1.0;
        }
        std::cout << "CTE: " << cte << " Steering Value: " << steer_value << std::endl;

        json msgJson;
        msgJson["steering_angle"] = steer_value;
        msgJson["throttle"] = 0.3;
        auto msg = "42[\"steer\"," + msgJson.dump() + "]";
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }

    }
    else
    {
      // Manual driving
      std::string msg = "42[\"manual\",{}]";
      ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
    }
  }
}
