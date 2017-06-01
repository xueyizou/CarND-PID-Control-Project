#include "messagehandler_tweedle.h"
#include "toolfuncs.h"

// for convenience
using namespace std;
using json = nlohmann::json;

//500: 0.0886294,	0,	0.119224
//1000: 0.744729,	0,	8.28934
MessageHandler_Tweedle::MessageHandler_Tweedle():p{0.0886294,	0,	0.119224},dp{1,1,1}, state("init"), idx(-1), counter(0), max_counter(1000), best_sq_cte(1e6), itr(0)
{

}

void MessageHandler_Tweedle::operator()(uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, uWS::OpCode opCode)
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
        double steer_value;

        if(dp[0]+dp[1]+dp[2]>0.05)
        {
            if(state=="init")
            {
                if(counter==0)
                {
                    std::cout<<"State: init......................"<<std::endl;
                    pid.Init(p);
                }

                if ( (fabs(cte)>5 || speed<0.8 ) && counter>50 )
                { //probably out of track or crash.restart and tweedle "up"
                  best_sq_cte =1e6;
                  state="up";
                  counter=0;

                  std::cout<<"Iteration: "<<itr<<" finished early. Best_sq_cte: "<<best_sq_cte<<std::endl<<
                  "P: "<<p[0]<<"\t"<<p[1]<<"\t"<<p[2]<<std::endl<<
                  "dP: "<<dp[0]<<"\t"<<dp[1]<<"\t"<<dp[2]<<endl<<"============================================="<<endl<<endl;
                  itr++;

                  std::string msg = "42[\"reset\", {}]";
                  ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
                  return;
                }

                pid.UpdateError(cte);
                json msgJson;
                msgJson["steering_angle"] = pid.getControl();
                msgJson["throttle"] = 0.3;
                auto msg = "42[\"steer\"," + msgJson.dump() + "]";
                ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
                counter++;

                //ready to get pid error and set it as the currently best one
                if(counter==max_counter)
                {
                    best_sq_cte = pid.TotalError();
                    state="up";
                    counter=0;

                    std::cout<<"Iteration: "<<itr<<" finished. Best_sq_cte: "<<best_sq_cte<<std::endl
                            <<"P: "<<p[0]<<"\t"<<p[1]<<"\t"<<p[2]<<std::endl
                            <<"dP: "<<dp[0]<<"\t"<<dp[1]<<"\t"<<dp[2]<<std::endl
                            <<"============================================="<<endl<<endl;
                    itr++;
                }

            }
            else if(state=="up")
            {
                if(counter==0)
                {
                    idx = (idx+1)%3;
                    p[idx]+=dp[idx];
                    pid.Init(p);
                }

                if ( (fabs(cte)>5 || speed<0.8 ) && counter>50 )
                { //probably out of track or crash. restart and tweedle "down"

                    std::cout<<"Index: "<< idx<<" State: up finished early. Best_sq_cte: "<<best_sq_cte<<std::endl
                            <<"P: "<<p[0]<<"\t"<<p[1]<<"\t"<<p[2]<<std::endl
                            <<"dP: "<<dp[0]<<"\t"<<dp[1]<<"\t"<<dp[2]<<std::endl
                            <<endl;

                    state="down";
                    counter=0;

                    std::string msg = "42[\"reset\", {}]";
                    ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
                    return;
                }

                pid.UpdateError(cte);
                json msgJson;
                msgJson["steering_angle"] = pid.getControl();
                msgJson["throttle"] = 0.3;
                auto msg = "42[\"steer\"," + msgJson.dump() + "]";
                ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
                counter++;

                //ready to get pid error and compare it with the currently best one
                if(counter==max_counter)
                {
                    float err= pid.TotalError();
                    if(err<best_sq_cte)
                    {
                        best_sq_cte=err;
                        dp[idx] *=1.1;
                        state="up";
                        counter=0;

                        std::cout<<"Index: "<< idx<<" State: up finished. Best_sq_cte: "<<best_sq_cte<<std::endl
                                <<"Iteration: "<<itr<<" finished. Best_sq_cte: "<<best_sq_cte<<std::endl
                                <<"P: "<<p[0]<<"\t"<<p[1]<<"\t"<<p[2]<<std::endl
                                <<"dP: "<<dp[0]<<"\t"<<dp[1]<<"\t"<<dp[2]<<std::endl
                                <<"============================================="<<endl<<endl;
                    }
                    else
                    {
                        state="down";
                        counter=0;

                        std::cout<<"Index: "<< idx<<" State: up finished. Best_sq_cte: "<<best_sq_cte<<std::endl
                                <<"P: "<<p[0]<<"\t"<<p[1]<<"\t"<<p[2]<<std::endl
                                <<"dP: "<<dp[0]<<"\t"<<dp[1]<<"\t"<<dp[2]<<std::endl<<std::endl;

                    }


                }
            }
            else if(state=="down")
            {
                if(counter==0)
                {
                    p[idx] -= 2*dp[idx];
                    pid.Init(p);
                }

                if( (fabs(cte)>5 || speed<0.8) && counter>50 )
                { //probably out of track or crash. restart and tweedle "up" in next index of p
                  p[idx] += dp[idx];
                  dp[idx] *=0.9;

                  std::cout<<"Index: "<< idx<<" State: down finished early. Best_sq_cte: "<<best_sq_cte<<std::endl
                          <<"Iteration: "<<itr<<" finished. Best_sq_cte: "<<best_sq_cte<<std::endl
                          <<"P: "<<p[0]<<"\t"<<p[1]<<"\t"<<p[2]<<std::endl
                          <<"dP: "<<dp[0]<<"\t"<<dp[1]<<"\t"<<dp[2]<<std::endl
                          <<"============================================="<<endl<<endl;

                  state="up";
                  counter=0;
                  itr++;

                  std::string msg = "42[\"reset\", {}]";
                  ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
                  return;
                }

                pid.UpdateError(cte);
                json msgJson;
                msgJson["steering_angle"] = pid.getControl();
                msgJson["throttle"] = 0.3;
                auto msg = "42[\"steer\"," + msgJson.dump() + "]";
                ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
                counter++;

                //ready to get pid error and compare it with the currently best one
                if(counter==max_counter)
                {
                    float err= pid.TotalError();
                    if(err<best_sq_cte)
                    {
                        best_sq_cte=err;
                        dp[idx] *=1.1;

                    }
                    else
                    {
                        p[idx] += dp[idx];
                        dp[idx] *=0.9;

                    }

                    std::cout<<"Index: "<< idx<<" State: down finished. Best_sq_cte: "<<best_sq_cte<<std::endl
                            <<"Iteration: "<<itr<<" finished. Best_sq_cte: "<<best_sq_cte<<std::endl
                            <<"P: "<<p[0]<<"\t"<<p[1]<<"\t"<<p[2]<<std::endl
                            <<"dP: "<<dp[0]<<"\t"<<dp[1]<<"\t"<<dp[2]<<std::endl
                            <<"============================================="<<endl<<endl;

                    state="up";
                    counter=0;
                    itr++;
                }
            }
        }
        else
        {
            cout<<"Tweedle finished!"<<endl
               <<"best P: "<<p[0]<<"\t"<<p[1]<<"\t"<<p[2]<<std::endl
               <<"dP: "<<dp[0]<<"\t"<<dp[1]<<"\t"<<dp[2]<<std::endl;
        }
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
