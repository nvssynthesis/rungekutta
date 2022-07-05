/* function loaded by the rungekutta~ object to make it act like the Moog
voltage-controlled filter.  See the bob~ documentation in Pd for theory and
detailed credits to papers by Tim Stilson, Timothy E. Stinchcombe, and Antti
Huovilainen. 
Credit: Miller Puckette
*/


    /* parameters are given symbolic names here to make the code below
    more readable */
#define PARAM_INPUT (params[0])     /* filter input */
#define PARAM_CUTOFF (params[1])    /* "cutoff" frequency */
#define PARAM_RESONANCE (params[2]) /* resonance control (4 to oscillate) */

    /* imitate the (tanh) clipping function of a transistor pair.
    In the papers this is given as 'tanh' but we use a cheaper way:
    to 4th order, tanh is x - x*x*x/3; this cubic's relative maximum and
    minimu are at +/- 1 so clip to 1 and evaluate the cubic.
    This is pretty coarse - for instance if you clip a sinusoid this way you
    can sometimes hear the discontinuity in 4th derivative at the clip point */

#define SATURATION 3.f /* transistor saturation voltage */
static float clip(float value)
{
    float satval = value*(1.f/SATURATION);
    float v2 = (satval > 1.f ? 1.f : (satval < -1.f ? -1.f: satval));
    return (SATURATION * (v2 - (1.f/3.f) * v2 * v2 * v2));
}

int rungekutta_nstate = 4;
int rungekutta_nparam = 3;
void rungekutta_derivative(float *result, float *state, float *params)
{
    float k = ((float)(2*3.14159)) * PARAM_CUTOFF;
    float satstate0 = clip(state[0]);
    float satstate1 = clip(state[1]);
    float satstate2 = clip(state[2]);
    result[0] = k *
        (clip(PARAM_INPUT - PARAM_RESONANCE * state[3]) - satstate0);
    result[1] = k * (satstate0 - satstate1);
    result[2] = k * (satstate1 - satstate2);
    result[3] = k * (satstate2 - clip(state[3]));
}

