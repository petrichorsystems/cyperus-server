
#include <pthread.h>

#include "common.h"
#include "../dsp.h"

extern void modules_common_dsp_graph_lock() {
	pthread_spin_lock(&dsp_global.optimization_spinlock);
	pthread_mutex_lock(&dsp_global.graph_state_mutex);
} /* modules_common_dsp_graph_lock */

extern void modules_common_dsp_graph_unlock() {
	pthread_spin_unlock(&dsp_global.optimization_spinlock);
	pthread_mutex_unlock(&dsp_global.graph_state_mutex);		
} /* modules_common_dsp_graph_unlock */
