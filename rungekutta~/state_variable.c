//
//  state_variable.c
//
#define PARAM_INPUT (params[0])     /* filter input */
#define PARAM_CUTOFF (params[1])    /* "cutoff" frequency */
#define PARAM_Q (params[2])         /* resonance */
#define NONLINEARITY (params[3])

#define Y_BP (state[0])
#define Y_LP (state[1])

#define Y_PRIME_BP (result[0])
#define Y_PRIME_LP (result[1])

#define TWOPI 6.283185307179586

static float clip(float value, float saturation)
{
    float satval = value*(1.f/saturation);
    float v2 = (satval > 1.f ? 1.f : (satval < -1.f ? -1.f: satval));
    return (saturation * (v2 - (1.f/3.f) * v2 * v2 * v2));
}

// limit values of Q
static float invert_clamp(float value)
{
    // avoid divide by zero and limit output value, which in turn limits bandwidth
    if (value < 0.707f)
        value = 0.707f;
    return 1.f / value;
}

// y_hp  = x - 2R*y_bp - y_lp
// y'_bp = wc * y_hp
// y'_lp = wc * y_bp

int rungekutta_nstate = 2; // only 2 memory elements needed
int rungekutta_nparam = 4;
void rungekutta_derivative(float *result, float *state, float *params)
{
    if (PARAM_CUTOFF < 0.f)
        PARAM_CUTOFF = 0.f;
    
    float y_hp = PARAM_INPUT - 2.f * invert_clamp(PARAM_Q) * Y_BP - Y_LP;
    Y_PRIME_BP = TWOPI * PARAM_CUTOFF * (NONLINEARITY != 0.f ? clip(y_hp, NONLINEARITY) : y_hp);
    Y_PRIME_LP = TWOPI * PARAM_CUTOFF * (NONLINEARITY != 0.f ? clip(Y_BP, NONLINEARITY) : Y_BP);
    //result[0] = TWOPI * PARAM_CUTOFF * (PARAM_INPUT - state[0]);
}
