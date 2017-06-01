# Reflection

## Describe the effect each of the P, I, D components had in your implementation.
	
P sets the steer angle proportional to the cte. The consequence is that the cte gradually becomes zero.

I sets the steer angle proportional to the accumulated cte. It is used to offset systematic errors.

D sets the steer angle proportional to the difference between ctes. It is used to avoid overshot.



## Describe how the final hyperparameters were chosen.
	

I used a "tweedle" procedure to tune the hyperparameters. The code is in the "messagehandler_tweedle.cpp" file. 

I tweedled twice. In the first tweedle, I evaluated the pid controler with different hyperparameters uisng 500 simulation steps. The tweedle procudure converged on a local minima ({0.0886294,	0,	0.119224}). In the second tweedle, I evaluated the pid controler using 1000 simulation steps. When the tweedle procudure converged, the resultant parameters ({0.744729,	0,	8.28934}) are good.

The finial hyperparameters are {0.744729,	0,	8.28934} for Kp, ki and Kd respectively.
