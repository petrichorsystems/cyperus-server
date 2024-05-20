dsp_remove:
	gcc -o test_dsp_remove src/test_dsp_remove.c ../src/dsp_types.c ../src/dsp.c ../src/dsp_ops.c ../src/dsp_graph_id.c -luuid

osc:
	gcc  -o test_osc test_osc.c -llo

dsp:
	gcc  -o test_dsp test_dsp.c ../rtqueue.c ../dsp_math.c ../dsp.c ../dsp_types.c ../dsp_ops.c ../jackcli.c ../osc.c ../osc_handlers.c -ljack -lpthread -lm -llo -lfftw3 -luuid -ljack
