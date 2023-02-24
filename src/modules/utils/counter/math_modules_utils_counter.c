
#include "math_modules_utils_counter.h"

extern
float math_modules_utils_counter(dsp_parameter *counter, int samplerate, int pos) {
  float reset = counter->parameters->float32_type[0];
  float start = counter->parameters->float32_type[1];
  float step_size = counter->parameters->float32_type[2];
  float min = counter->parameters->float32_type[3];
  float max = counter->parameters->float32_type[4];
  float direction = counter->parameters->float32_type[5];
  float auto_reset = counter->parameters->float32_type[6];

  float current_step = counter->parameters->float32_type[7];

  if( reset ) {
    if( direction > 0.0f ) {
      current_step = min;
    } else if( direction < 0.0f ) {
      current_step = max;
    } else {
      /* direction is 0, so do nothing */
    }
    counter->parameters->float32_type[0] = 0.0f; /* set reset back to 0 */
  } else if( counter->in ) {
    if( direction > 0.0f ) {
      current_step += step_size; 
      if( current_step > max ) {
        if( auto_reset ) {
          current_step = min;
        } else {
          current_step = max;
        }
      }
    } else if( direction < 0.0f ) {
      current_step -= step_size;
      if( current_step < min ) {
        if( auto_reset ) {
          current_step = max;
        } else {
          current_step = min;
        }
      }    
    } else {
      /* direction is 0, so do nothing */
    }
  }

  if( current_step != counter->parameters->float32_type[7] ) {
    counter->parameters->float32_type[7] = current_step;
  }
  
  return current_step;
}
