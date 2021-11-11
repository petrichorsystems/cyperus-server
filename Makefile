dsp_module_sources := $(shell ./tools/enum_dsp_module_sources.sh)

core = src/cyperus.c src/dsp.c src/dsp_types.c src/dsp_ops.c src/dsp_math.c src/rtqueue.c src/jackcli.c src/osc.c src/osc_handlers.c
module_utils = src/modules/math_utils.c

all:
	gcc -o build/cyperus $(core) $(module_utils) $(dsp_module_sources) -lm -lpthread -ljack -llo -luuid
