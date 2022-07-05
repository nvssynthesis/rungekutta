/* emulation of EMS VCS3 filter. Based on paper 
"A NONLINEAR DIGITAL MODEL OF THE EMS VCS3 VOLTAGE-CONTROLLED FILTER"
by Marco Civolani and Federico Fontana  */

#include <math.h>
    /* parameters are given symbolic names here to make the code below
    more readable */
#define PARAM_INPUT (params[0])     /* filter input */
#define PARAM_CUTOFF (params[1])    /* "cutoff" frequency */
#define K (params[2])               /* resonance control */

#define V_c1 state[0]
#define V_c2 state[1]
#define V_c3 state[2]
#define V_c4 state[3]

#define C 2.0  // capacitance, though it is false
#define VT 0.0260
#define GAMMA 0.047736

#define SATURATION 3.f /* transistor saturation voltage */
// static float clip(float value)
// {
//     float satval = value*(1.f/SATURATION);
//     float v2 = (satval > 1.f ? 1.f : (satval < -1.f ? -1.f: satval));
//     return (SATURATION * (v2 - (1.f/3.f) * v2 * v2 * v2));
// }
static float clip(float value)
{
    return tanhf(value);
}

int rungekutta_nstate = 4;
int rungekutta_nparam = 3;
void rungekutta_derivative(float *result, float *state, float *params)
{
    float cut = 8 * C * VT * 2 * PARAM_CUTOFF;
    float Vout_last = (K*4 + 0.5f) * state[3];

    float tmp, tmp2;
    tmp = clip((V_c2 - V_c1)/(2*GAMMA));
    result[0] = cut*(clip((PARAM_INPUT-Vout_last)/(2*VT)) + tmp);

    tmp2 = clip((V_c3 - V_c2)/(2*GAMMA));
    result[1] = cut*(tmp2 - tmp);

    tmp = clip((V_c4 - V_c3)/(2*GAMMA));
    result[2] = cut*(tmp - tmp2);

    result[3] = cut*(-clip(V_c4/(6*GAMMA)) - tmp);
    //result[3] = k*((1-tmp)/(1+tmp) + tanhf(-state[3] / 6*GAMMA));

}
