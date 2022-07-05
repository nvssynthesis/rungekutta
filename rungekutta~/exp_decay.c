//
//  exp_decay.c
//

// y' = -y

int rungekutta_nstate = 1;
int rungekutta_nparam = 1;
void rungekutta_derivative(float *result, float *state, float *params)
{
    result[0] = -1 * params[0] * state[0];
}
