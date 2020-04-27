osc:
	gcc  -o cyperus cyperus.c rtqueue.c dsp_math.c dsp.c dsp_types.c dsp_ops.c jackcli.c osc.c osc_handlers.c modules/dsp/math_utils.c modules/dsp/filter/varslope_lowpass/math_modules_dsp_filter_varslope_lowpass.c modules/dsp/filter/varslope_lowpass/ops_modules_dsp_filter_varslope_lowpass.c -ljack -lpthread -lm -llo -lfftw3 -luuid
