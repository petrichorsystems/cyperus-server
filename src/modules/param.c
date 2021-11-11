#include "params.h"

// these "accessors" are just for convenience, performing lookup and cast
int *param_state_get_int_type(const param_state_t *state) {
    return (int *) state->param_data[PARAM_TYPE_INT];
}

float *param_state_get_float_type(const param_state_t *state) {
    return (float *) state->param_data[PARAM_TYPE_FLOAT];
}

float **param_state_get_float_arr_type(const param_state_t *state) {
    return (float **) state->param_data[PARAM_TYPE_FLOAT_ARR];
}

double *param_state_get_double_type(const param_state_t *state) {
    return (double *) state->param_data[PARAM_TYPE_DOUBLE];
}

uint8_t *param_state_get_uint_type(const param_state_t *state) {
    return (uint8_t *) state->param_data[PARAM_TYPE_UINT];
}

char **param_state_get_char_type(const param_state_t *state) {
    return (char **) state->param_data[PARAM_TYPE_CHAR];
}

/// i'm assuming that each `bytes` param should be `void*` not `void`
void **param_state_get_bytes_type(const param_state_t *state) {
    return (void **) state->param_data[PARAM_TYPE_BYTES];
}

// allocators
int param_state_add_int(param_state_t *state, void *data) {
    int *type_data = param_state_get_int_type(state);
    type_data = realloc(type_data, sizeof(int) * state->registry.count_per_type[PARAM_TYPE_INT]);
    if (type_data == NULL) { return 1; }
    state->param_data[PARAM_TYPE_INT] = type_data;
    (param_state_get_int_type(state))[PARAM_TYPE_INT] = *((int*)data);
    return 0;
}

int param_state_add_float(param_state_t *state, void *data) {
    int *type_data = param_state_get_int_type(state);
    type_data = realloc(type_data, sizeof(float) * state->registry.count_per_type[PARAM_TYPE_FLOAT]);
    if (type_data == NULL) { return 1; }
    state->param_data[PARAM_TYPE_FLOAT] = type_data;
    (param_state_get_int_type(state))[PARAM_TYPE_FLOAT] = *((int*)data);
    return 0;
}

//... etc

//---------------------
// allocation / management / editing...

param_state_t * param_state_new() {
    param_state_t *state = (param_state_t *)malloc(sizeof(param_state_t));
    for (param_type_t t=0; t<NUM_PARAM_TYPES; ++t) {
        state->registry.count_per_type[t] = 0;
    }
    //.. etc?
    return state;
}

void param_state_add_param(param_state_t *state, struct param_desc *desc) {
    unsigned int id_type = state->registry.count_per_type[desc->type]++;
    unsigned int id_param =state->registry.count++;
    state->registry.entries[id_param].type = desc->type;
    state->registry.entries[id_type].id = id_type;
    state->registry.entries[id_type].hash = hash_string((unsigned char*)desc->name);

    switch(desc->type) {
        case PARAM_TYPE_INT:
            param_state_add_int(state, desc->data);
            break;
        case PARAM_TYPE_FLOAT:
            param_state_add_float(state, desc->data);
            break;
        case PARAM_TYPE_FLOAT_ARR:
            // ...etc
        case PARAM_TYPE_DOUBLE:
            // ...etc
        case PARAM_TYPE_UINT:
            // ...etc
        case PARAM_TYPE_CHAR:
            // ...etc
        case PARAM_TYPE_BYTES:
            // ...etc
        case NUM_PARAM_TYPES:
        default:
            ;;
    }
}

int main() {
    ///...
}
