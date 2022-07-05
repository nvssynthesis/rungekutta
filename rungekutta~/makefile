# all: rungekutta~.l_ia64 rungekutta~.l_i386 moogfilter.rk_x86_64
all: rungekutta~.d_fat moogfilter.rk_mac signumswitch.rk_mac lorenz.rk_mac exp_decay.rk_mac rk_onepole.rk_mac state_variable.rk_mac vcs3filter.rk_mac rossler.rk_mac 

NAME=rungekutta~
CSYM=rungekutta_tilde

include makefile.include

.SUFFIXES: .rk_x86_64 .rk_i386 .rk_arm_so .rk_mac .rk_dll

.c.rk_x86_64:
	$(CC) $(LINUXCFLAGS) -shared -o $@ $(@:.rk_x86_64=.c) -lc -lm 
	strip --strip-unneeded $@

.c.rk_mac:
	$(CC) $(DARWINARCH) $(DARWINCFLAGS) -bundle -undefined suppress \
                -flat_namespace -o $@  $(@:.rk_mac=.c)

.c.rk_dll:
	$(MSCC) $(PDNTCFLAGS) $(PDNTINCLUDE) /c $(@:.rk_dll=.c)
	$(MSLN) /nologo /dll \
                /export:rungekutta_derivative \
                /export:rungekutta_nstate \
                /export:rungekutta_nparam \
                /out:$@ $(@:.rk_dll=.obj) $(PDNTLIB)

