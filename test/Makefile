osc:
	gcc  -o test_osc test_osc.c -llo

dsp:
	gcc  -o test_dsp test_dsp.c ../rtqueue.c ../dsp_math.c ../dsp.c ../dsp_types.c ../dsp_ops.c ../jackcli.c ../osc.c ../osc_handlers.c -ljack -lpthread -lm -llo -lfftw3 -luuid -ljack
