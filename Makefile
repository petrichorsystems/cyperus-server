dsp_module_sources := $(shell ./tools/enum_dsp_module_sources.sh)

core = src/main.c src/dsp.c src/dsp_types.c src/dsp_ops.c src/jackcli.c src/osc.c src/osc_handlers.c src/osc_string.c src/dsp_graph_id.c
module_utils = src/modules/math_utils.c src/modules/common.c

all:
	mkdir -p build/
	gcc -D_XOPEN_SOURCE=600 -march=native -O3 -fno-math-errno -fno-trapping-math -ffinite-math-only -Wall -pthread -o build/cyperus-server $(core) $(module_utils) $(dsp_module_sources) -lm -ljack -llo -luuid 

clean:
	rm -r build/
