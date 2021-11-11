#ifndef MODULES_PARAMS_H_
#define MODULES_PARAMS_H_
#include <stdint.h>
#include <stddef.h>

#include <stdint.h>
#include <stdlib.h>


//------------------------
//--- constants and basic types

#define MAX_PARAM_NAME_LEN 64

typedef enum param_type {
    PARAM_TYPE_INT = 0,
    PARAM_TYPE_FLOAT,
    PARAM_TYPE_FLOAT_ARR,
    PARAM_TYPE_DOUBLE,
    PARAM_TYPE_UINT,
    PARAM_TYPE_CHAR,
    PARAM_TYPE_BYTES,
    NUM_PARAM_TYPES
} param_type_t;


struct param_desc {
    char name[MAX_PARAM_NAME_LEN];
    param_type_t type;
    void *data;
};

//-----------------------
//--- parameter registry

struct param_registry_entry {
    // the `id` indexes into the type-specific registry/data
    unsigned int id;
    param_type_t type;
    unsigned long hash;
    char name[MAX_PARAM_NAME_LEN];
};

struct param_registry {
    unsigned int count;
    unsigned int count_per_type[NUM_PARAM_TYPES];
    struct param_registry_entry *entries;
};

// dan bernstein's xor-based hash
// http://www.cse.yorku.ca/~oz/hash.html
unsigned long hash_string(unsigned char *str)
{
    unsigned long hash = 5381;
    int c = *str++;
    while (c) {
        hash = hash * 33 ^ c;
        c = *str++;
    }
    return hash;
}

//-------------------------
//--- the state structure!

typedef struct param_state {
    int num_params;
    void *param_data[NUM_PARAM_TYPES];
    struct param_registry registry;
} param_state_t;

#endif

