/* function loaded by the rungekutta~ object to describe the Lorenz
attractor.  See the README in this directory. 
Credit: Miller Puckette 
*/

    /* parameters and state variablesare given symbolic names here to make
    the code below more readable */
#define PARAM_ALPHA (params[0])     /* time scale (in Hz.) */
#define PARAM_SIGMA (params[1])     /* other params as defined in wikipedia */
#define PARAM_RHO (params[2])
#define PARAM_BETA (params[3])

#define STATE_X (state[0])         /* 3 state variables x, y, and z */
#define STATE_Y (state[1])
#define STATE_Z (state[2])

#define DERIVATIVE_X (result[0])    /* 3 derivatives x-dot, etc */
#define DERIVATIVE_Y (result[1])
#define DERIVATIVE_Z (result[2])

int rungekutta_nparam = 4;
int rungekutta_nstate = 3;

void rungekutta_derivative(float *result, float *state, float *params)
{
    DERIVATIVE_X = PARAM_ALPHA * (PARAM_SIGMA * (STATE_Y - STATE_X));
    DERIVATIVE_Y = PARAM_ALPHA * (STATE_X * (PARAM_RHO - STATE_Z) - STATE_Y);
    DERIVATIVE_Z = PARAM_ALPHA * (STATE_X * STATE_Y - PARAM_BETA * STATE_Z);
}

