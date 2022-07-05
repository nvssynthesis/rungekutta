/* rungekutta~ - a differential equation solver */

/* copyright 2015 Miller Puckette - BSD license */

#include "m_pd.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#ifdef _WIN32
# include <malloc.h> /* MSVC or mingw on windows */
#elif defined(__linux__) || defined(__APPLE__)
# include <alloca.h> /* linux, mac, mingw, cygwin */
#else
# include <stdlib.h> /* BSDs for example */
#endif
#ifdef MSW
#include <io.h>
#include <windows.h>
#else
#include <unistd.h>
#include <dlfcn.h>
#endif

#if defined(__linux__) || defined(__FreeBSD_kernel__) || defined(__GNU__) || defined(__FreeBSD__)
# ifdef __x86_64__
static char objextent[] = ".rk_x86_64";
# elif defined(__i386__) || defined(_M_IX86)
static char objextent[] = ".rk_i386";
# elif defined(__arm__)
static char objextent[] = ".rk_arm";
# else
static char objextent[] = ".so";
# endif
#elif defined(__APPLE__)
static char objextent[] = ".rk_mac";
#elif defined(_WIN32) || defined(__CYGWIN__)
static char objextent[] = ".rk_dll";
#else
static char objextent[] = ".so";
#endif

typedef void (*t_extfn)(float *result, float *state, float *params);


static t_class *rungekutta_class;
typedef struct _rungekutta
{
    t_object x_obj;
    t_float x_f;
    float **x_io;       /* input/output signals */
    int x_nparam;
    float *x_state;
    int x_nstate;
    float x_sr;
    int x_oversample;
    void *x_dlobj;
    t_extfn x_fn;
    t_canvas *x_canvas;
} t_rungekutta;

static void rungekutta_load(t_rungekutta *x, t_symbol *s)
{
    int *intptr;
#ifdef MSW
    HINSTANCE ntdll;
#endif
    char estring[200];
    char buf[MAXPDSTRING], *bufptr, basename[MAXPDSTRING],
        filename[MAXPDSTRING];
    int fd;
    x->x_dlobj = 0;
    strncpy(basename, s->s_name, MAXPDSTRING);
    basename[MAXPDSTRING-1] = 0;
    if (!strchr(basename, '.'))
    {
        strncat(basename, objextent, MAXPDSTRING-strlen(basename));
        basename[MAXPDSTRING-1] = 0;
    }
    
    fd = open_via_path(canvas_getdir(x->x_canvas)->s_name,
         basename, "", buf, &bufptr, MAXPDSTRING, 0);
    if (fd < 0)
    {
        pd_error(x, "%s: can't open file", basename);
        return;
    }
    sys_close(fd);
    strncpy(filename, buf, MAXPDSTRING);
    filename[MAXPDSTRING-2] = 0;
    strcat(filename, "/");
    strncat(filename, bufptr, MAXPDSTRING-strlen(filename));
    filename[MAXPDSTRING-1] = 0;
#ifdef MSW
    sys_bashfilename(filename, filename);
    ntdll = LoadLibrary(filename);
    if (!ntdll)
    {
        post("%s: couldn't load", filename);
        return;
    }
    x->x_dlobj = (void *)ntdll;
    if (!(x->x_fn = (t_extfn)GetProcAddress(ntdll, "rungekutta_derivative"))) 
    {
        post("%s: symbol not found in object file", "rungekutta_derivative");
        goto fail;
    }
    if (!(intptr = (int *)GetProcAddress(ntdll, "rungekutta_nstate")))
    {
        post("%s: symbol not found in object file", "rungekutta_nstate");
        goto fail;
    }
    x->x_nstate = *intptr;
    if (x->x_nstate < 1)
        x->x_nstate = 1;
    if (!(intptr = (int *)GetProcAddress(ntdll, "rungekutta_nparam")))
    {
        post("%s: symbol not found in object file", "rungekutta_nparam");
        goto fail;
    }
    x->x_nparam = *intptr;
    if (x->x_nparam < 1)
        x->x_nparam = 1;
#else
    x->x_dlobj = dlopen(filename, RTLD_NOW | RTLD_GLOBAL);
    if (!x->x_dlobj)
    {
        post("%s", dlerror());
        x->x_dlobj = 0;
        goto fail;
    }
    x->x_fn = (t_extfn)dlsym(x->x_dlobj,  "rungekutta_derivative");
    if (!x->x_fn)
        x->x_fn = (t_extfn)dlsym(x->x_dlobj,  "_rungekutta_derivative");
    if (!x->x_fn)
    {
        post("%s: symbol not found in object file", "rungekutta_derivative");
        goto fail;
    }
    intptr = (int *)dlsym(x->x_dlobj,  "rungekutta_nstate");
    if (!intptr)
        intptr = (int *)dlsym(x->x_dlobj,  "_rungekutta_nstate");
    if (!intptr)
    {
        post("%s: symbol not found in object file", "rungekutta_nstate");
        goto fail;
    }
    x->x_nstate = *intptr;
    if (x->x_nstate < 1)
        x->x_nstate = 1;

    intptr = (int *)dlsym(x->x_dlobj,  "rungekutta_nparam");
    if (!intptr)
        intptr = (int *)dlsym(x->x_dlobj,  "_rungekutta_nparam");
    if (!intptr)
    {
        post("%s: symbol not found in object file", "rungekutta_nparam");
        goto fail;
    }
    x->x_nparam = *intptr;
    if (x->x_nparam < 1)
        x->x_nparam = 1;

#endif
    return;
fail:
    if (x->x_dlobj)
    {
#ifdef MSW
            FreeLibrary(x->x_dlobj);
#else
            dlclose(x->x_dlobj);
#endif
    }
    x->x_dlobj = 0;   
    x->x_fn = 0;
    x->x_nstate = x->x_nparam = 1;
}

static void rungekutta_free(t_rungekutta *x)
{
    if (x->x_dlobj)
    {
#ifdef MSW
            FreeLibrary(x->x_dlobj);
#else
            dlclose(x->x_dlobj);
#endif
    }
}
static void rungekutta_set(t_rungekutta *x, t_symbol *s, int argc, t_atom *argv)
{
    int i;
    for (i = 0; i < x->x_nstate; i++)
        x->x_state[i] = atom_getfloatarg(i, argc, argv);
}

static void rungekutta_print(t_rungekutta *x)
{
    int i;
    for (i = 0; i < x->x_nstate; i++)
        post("state %d: %f", i, x->x_state[i]);
}

static void rungekutta_oversample(t_rungekutta *x, t_float oversample)
{
    if (oversample <= 1)
        oversample = 1;
    x->x_oversample = oversample;
}

static void *rungekutta_new(t_symbol *filesym, t_floatarg oversample)
{
    int nparams, nstate, i;
    t_rungekutta *x = (t_rungekutta *)pd_new(rungekutta_class);
    x->x_canvas = canvas_getcurrent();
    x->x_dlobj = 0;
    x->x_fn = 0;
    rungekutta_load(x, (*filesym->s_name ? filesym:gensym("function")));
    if (!x->x_fn)
    {
        pd_free(&x->x_obj.ob_pd);
        return (0);
    }
    for (i = 0; i < x->x_nparam-1; i++)
        inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
    for (i = 0; i < x->x_nstate; i++)
        outlet_new(&x->x_obj, &s_signal);
    x->x_state = (float *)getbytes(sizeof(float) * x->x_nstate);
    x->x_io = (float **)getbytes(sizeof(float *) * (x->x_nparam + x->x_nstate));
    x->x_f = 0;

    rungekutta_set(x, 0, 0, 0);
    rungekutta_oversample(x, oversample);
    return (x);
}

static void solver(t_rungekutta *x, float *state, float *params, float stepsize)
{
    float cumerror = 0;
    int i;
    float *buffer = (float *)alloca((sizeof(float)*5)*x->x_nstate);
    float *deriv1 = buffer, *deriv2 = buffer + x->x_nstate,
        *deriv3 = buffer + 2*x->x_nstate, *deriv4 = buffer + 3*x->x_nstate, 
        *tempstate = buffer + 4*x->x_nstate;

    (*x->x_fn)(deriv1, state, params);
    for (i = 0; i < x->x_nstate; i++)
        tempstate[i] = state[i] + 0.5 * stepsize * deriv1[i];
    (*x->x_fn)(deriv2, tempstate, params);
    for (i = 0; i < x->x_nstate; i++)
        tempstate[i] = state[i] + 0.5 * stepsize * deriv2[i];
    (*x->x_fn)(deriv3, tempstate, params);
    for (i = 0; i < x->x_nstate; i++)
        tempstate[i] = state[i] + stepsize * deriv3[i];
    (*x->x_fn)(deriv4, tempstate, params);
    for (i = 0; i < x->x_nstate; i++)
        state[i] += (1./6.) * stepsize * 
            (deriv1[i] + 2 * deriv2[i] + 2 * deriv3[i] + deriv4[i]);
}

static t_int *rungekutta_perform(t_int *w)
{
    t_rungekutta *x = (t_rungekutta *)(w[1]);
    int n = (int)(w[2]), i, j;
    float stepsize = 1./(x->x_oversample * x->x_sr);
    float *param = (float *)alloca(sizeof(float)*x->x_nparam);
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < x->x_nparam; j++)
            param[j] = x->x_io[j][i];
        for (j = 0; j < x->x_oversample; j++)
            solver(x, x->x_state, param, stepsize);
        for (j = 0; j < x->x_nstate; j++)
            x->x_io[x->x_nparam+j][i] = x->x_state[j];
    }
    return (w+3);
}

static void rungekutta_dsp(t_rungekutta *x, t_signal **sp)
{
    int i;
    for (i = 0; i < x->x_nparam + x->x_nstate; i++)
        x->x_io[i] = sp[i]->s_vec;
    x->x_sr = sp[0]->s_sr;
    dsp_add(rungekutta_perform, 2, x, sp[0]->s_n);
}
void rungekutta_tilde_setup(void)
{
    int i;
    rungekutta_class = class_new(gensym("rungekutta~"),
        (t_newmethod)rungekutta_new, (t_method)rungekutta_free, 
            sizeof(t_rungekutta), 0, A_DEFSYMBOL, A_DEFFLOAT, 0);
    class_addmethod(rungekutta_class, (t_method)rungekutta_oversample,
        gensym("oversample"), A_FLOAT, 0);
    class_addmethod(rungekutta_class, (t_method)rungekutta_set, 
        gensym("set"), A_GIMME, 0);
    class_addmethod(rungekutta_class, (t_method)rungekutta_print,
        gensym("print"), 0);

    class_addmethod(rungekutta_class, (t_method)rungekutta_dsp, gensym("dsp"), A_CANT, 0);
    CLASS_MAINSIGNALIN(rungekutta_class, t_rungekutta, x_f);
}
