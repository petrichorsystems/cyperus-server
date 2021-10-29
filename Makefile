# DSP_MODULE_SOURCES := $(shell ./enum_dsp_module_sources.sh)
all:
	# bash compose_source_osc.sh
	gcc  -save-temps=obj -o build/cyperus cyperus.c rtqueue.c dsp_math.c dsp.c dsp_types.c dsp_ops.c jackcli.c osc_handlers.c  $(DSP_MODULE_SOURCES) -ljack -lpthread -lm -llo -lfftw3 -luuid
