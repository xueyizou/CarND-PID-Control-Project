#include <uWS/uWS.h>
#include <iostream>
#include "json.hpp"
#include "messagehandler_notweedle.h"
#include "messagehandler_tweedle.h"

using namespace std;

int main()
{
    /* if do tweedle, first evaluate the pid controler with different hyperparameters uisng 500 simulation steps.
     * the tweedle procudure with converge on a local minima. Then evaluate the pid controler using 1000 simulation
     * steps. When the tweedle procudure converged, the resultant parameters are good.
     */
  //tweedle?
  string flag="tweedled";

  uWS::Hub h;
  if(flag=="tweedle")
  {
      MessageHandler_Tweedle messageHandler;
      h.onMessage(messageHandler);
  }
  else
  {
      //500: 0.0886294,	0,	0.119224
      //1000: 0.744729,	0,	8.28934
      double p[3]={0.744729,	0,	8.28934};
      MessageHandler_NoTweedle messageHandler(p);
      h.onMessage(messageHandler);
  }

  // We don't need this since we're not using HTTP but if it's removed the program
  // doesn't compile :-(
  h.onHttpRequest([](uWS::HttpResponse *res, uWS::HttpRequest req, char *data, size_t, size_t) {
    const std::string s = "<h1>Hello world!</h1>";
    if (req.getUrl().valueLength == 1)
    {
      res->end(s.data(), s.length());
    }
    else
    {
      // i guess this should be done more gracefully?
      res->end(nullptr, 0);
    }
  });

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    //wait for a moment, otherwise the connection sometimes does not sent data.
    for(int i=0; i<1e8; i++)
    {

    }
    std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code, char *message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  if (h.listen(port))
  {
    std::cout << "Listening to port " << port << std::endl;
  }
  else
  {
    std::cerr << "Failed to listen to port" << std::endl;
  }
  h.run();

}
