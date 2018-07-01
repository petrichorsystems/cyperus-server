osc:
	gcc  -o cyperus cyperus.c rtqueue.c libcyperus.c dsp.c dsp_types.c dsp_ops.c osc_handlers.c -ljack -lpthread -lm -llo -lfftw3_threads -lfftw3
