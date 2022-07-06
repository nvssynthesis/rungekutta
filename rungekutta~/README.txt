rungekutta~ pd external developed by Miller Puckette:
"The rungekutta~ object.  BSD licensed; copyright notice is in the source code.

rungekutta~ loads a shared object file defining a differential equation
and integrates the equation using the runge-kutta technique.
The differential equation is supplied as a shared object loaded by the
rungekutta~ object at run time.  The shared object should provide two
integers:

int nstate;
int nparam;

initialized to the number of state variables and of input parameters to the
system, and a function:

void derivative(float *result, float *state, float *params)

where:
 result is the routine's output (the calculated derivatives, nstate elements)
 state is the current state of the system (nstate elements)
 params is an array of additional parameters (nparams elements)

The rungekutta~ object takes "nparams" and "nstate" as creation arguments and
has "nparams" inlets and "nstate" outputs, all signals.

Two sample differential equation sets are included:

'moogfilter' is a model of the Moog voltage-controlled resonant filter, which
is described in the documentation for Pd's bob~ object.  The equations are:

y1' = k * (S(x - r * y4) - S(y1))
y2' = k * (S(y1) - S(y2))
y3' = k * (S(y2) - S(y3))
y4' = k * (S(y3) - S(y4))

where k controls the cutoff frequency, r is feedback (<= 4 for
stability), and S(x) is a saturation function.

The second is the Lorenz attractor:

x' = alpha * (sigma * (y - x))
y' = alpha * (x * (rho - z) - y)
z' = alpha * (x * y - beta * z)

Here we've added a parameter alpha to control the overall time scale, in
addition to the three classical parameters sigma, rho, and beta (CF. the
Wikipedia page)."

From nvssynthesis:
I have included several other differential equation examples:
-exp_decay: this represents a sort of 'hello world' example. Just an exponential decay.
-rk_onepole: another very simple example, just a one-pole lowpass filter.
-rossler: the Rössler Attractor, see https://en.wikipedia.org/wiki/R%C3%B6ssler_attractor 
-signumswitch: an interpretation of the hyperchaotic system presented in "A Unique Signum Switch For Chaos And Hyperchaos" (https://www.researchgate.net/publication/318206297_A_unique_signum_switch_for_chaos_and_hyperchaos). The only modification made was to replace the piecewise sign function with tanh(value * bigNumber) to ensure differentiability. 
-state_variable: a (linear OR nonlinear) state-variable filter, outputs low- and band-pass. Combine in different ways to create high-pass or notch filters as well.
-vcs3: an nonlinear emulation of the VCS3 diode ladder filter, based on the paper "A Nonlinear Digital Model Of The EMS VCS3 Voltage-Controlled Filter" (http://legacy.spa.aalto.fi/dafx08/papers/dafx08_06.pdf) 