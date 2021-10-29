DSP_MODULE_SOURCES := $(shell ./enum_dsp_module_sources.sh)
all:
	bash compose_source_osc.sh
	gcc -o build/cyperus cyperus.c dsp.c dsp_types.c dsp_ops.c dsp_math.c rtqueue.c jackcli.c osc.c osc_handlers.c $(DSP_MODULE_SOURCES) -lm -lpthread -ljack -llo -luuid
