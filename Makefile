osc:
	gcc  -o cyperus cyperus.c rtqueue.c dsp_math.c dsp.c dsp_types.c dsp_ops.c  jackcli.c osc.c osc_handlers.c -ljack -lpthread -lm -llo -lfftw3 -luuid
