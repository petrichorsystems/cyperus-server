DSP_MODULE_SOURCES := $(shell ./enum_dsp_module_sources.sh)
all:
	bash compose_source_osc_c.sh
	gcc  -o cyperus cyperus.c rtqueue.c dsp_math.c dsp.c dsp_types.c dsp_ops.c jackcli.c osc.c osc_handlers.c modules/math_utils.c $(DSP_MODULE_SOURCES) -ljack -lpthread -lm -llo -lfftw3 -luuid
