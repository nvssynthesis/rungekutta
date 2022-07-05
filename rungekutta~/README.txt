The rungekutta~ object.  BSD licensed; copyright notice is in the source code.

rungekutta~ loads a shared object file defining a differential equation
and integrates teh rquation using the runge-kutte technique.
The differential equation is supplied as a shared object loaded by the
rungekutta~ object at run time.  The shared object should provide two
integers:

int nstate;
int nparam;

initialized to the number of state variables and of input parameters to the
system, and a function:

void derivative(float *result, float *state, float *params)

where:
 result is the routine's output (the calculated derivitaves, nstate elements)
 state is the current state of the system (nstate elements)
 params is an array of additional parameters (nparams elements)

The rungekutta~ object takes "nparams" and "nstate" as creation arguments and
has "nparams" inlets and "nstate" outputs, all signals.

Two sample differential equation sets are included:

'bob' is a model of the Moog voltage-controlled resonant filter, which
is described in the documentation for Pd's bob~ object.  The equations are:

y1' = k * (S(x - r * y4) - S(y1))
y2' = k * (S(y1) - S(y2))
y3' = k * (S(y2) - S(y3))
y4' = k * (S(y3) - S(y4))

where k controls the cutoff frequency, r is feedback (<= 4 for
stability), and S(x) is a saturation function.

The second is the Lorentz attractor:

x' = alpha * (sigma * (y - x))
y' = alpha * (x * (rho - z) - y)
z' = alpha * (x * y - beta * z)

Here we've added a parameter alpha to control the overall time scale, in
addition to the three classical parameters sigma, rho, and beta (CF. the
Wikipedia page).

