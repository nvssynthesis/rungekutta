/* function loaded by the rungekutta~ object to describe the Lorenz
attractor.  See the README in this directory. */

    /* parameters and state variablesare given symbolic names here to make
    the code below more readable */
#define PARAM_ALPHA (params[0])     /* time scale (in Hz.) */
#define PARAM_A (params[1])     /* other params as defined in wikipedia */
#define PARAM_B (params[2])
#define PARAM_C (params[3])

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
    DERIVATIVE_X = PARAM_ALPHA * (-STATE_Y - STATE_Z);
    DERIVATIVE_Y = PARAM_ALPHA * (STATE_X + PARAM_A * STATE_Y);
    DERIVATIVE_Z = PARAM_ALPHA * (PARAM_B + STATE_Z * (STATE_X - PARAM_C));
}
