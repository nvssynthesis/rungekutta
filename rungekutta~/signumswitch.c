/* based on "A UNIQUE SIGNUM SWITCH FOR CHAOS AND HYPERCHAOS" by 
Li et al. (2015) */
#include <math.h>

    /* parameters and state variablesare given symbolic names here to make
    the code below more readable */
#define PARAM_A (params[0])     
#define PARAM_B (params[1])    
#define PARAM_ALPHA (params[2]) // speed

#define STATE_X (state[0])         /* 3 state variables x, y, and z */
#define STATE_Y (state[1])
#define STATE_Z (state[2])
#define STATE_U (state[3])

#define DERIVATIVE_X (result[0])    /* 3 derivatives x-dot, etc */
#define DERIVATIVE_Y (result[1])
#define DERIVATIVE_Z (result[2])
#define DERIVATIVE_U (result[3])

// static float sgn(float val){
//     return val >= 0 ? 1.f : -1.f;
// }
static float sgn(float val){
    return tanhf(val * 200.f);

}

int rungekutta_nparam = 3;
int rungekutta_nstate = 4;
void rungekutta_derivative(float *result, float *state, float *params)
{
    DERIVATIVE_X = PARAM_ALPHA * (STATE_Y - STATE_X);
    DERIVATIVE_Y = PARAM_ALPHA * (-1 * STATE_Z * sgn(STATE_X) + STATE_U);
    DERIVATIVE_Z = PARAM_ALPHA * (STATE_X * sgn(STATE_Y) - PARAM_A);
    DERIVATIVE_U = PARAM_ALPHA * (-1 * PARAM_B * STATE_Y);
}