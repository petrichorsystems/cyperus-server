dsp_module_sources := $(shell ./enum_dsp_module_sources.sh)

core = cyperus.c dsp.c dsp_types.c dsp_ops.c dsp_math.c rtqueue.c jackcli.c osc.c osc_handlers.c
module_utils = modules/math_utils.c

all:
	gcc -o build/cyperus $(core) $(module_utils) $(dsp_module_sources) -lm -lpthread -ljack -llo -luuid
