//
//  rk_onepole.c
//

#define PARAM_INPUT (params[0])     /* filter input */
#define PARAM_CUTOFF (params[1])    /* "cutoff" frequency */
#define TWOPI 6.283185307179586

// y' = a * (x - y)

int rungekutta_nstate = 1;
int rungekutta_nparam = 2;
void rungekutta_derivative(float *result, float *state, float *params)
{
    result[0] = TWOPI * PARAM_CUTOFF * (PARAM_INPUT - state[0]);
}
