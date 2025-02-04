dsp_module_sources := $(shell ./tools/enum_dsp_module_sources.sh)

core = src/main.c src/dsp.c src/dsp_types.c src/dsp_ops.c src/dsp_math.c src/jackcli.c src/osc.c src/osc_handlers.c src/osc_string.c src/dsp_graph_id.c
module_utils = src/modules/math_utils.c

all:
	mkdir -p build/
	gcc -o build/cyperus-server $(core) $(module_utils) $(dsp_module_sources) -lm -lpthread -ljack -llo -luuid

clean:
	rm -r build/
