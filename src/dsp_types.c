/* dsp_types.c
This file is a part of 'cyperus'
This program is free software: you can redistribute it and/or modify
hit under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

'cyperus' is a JACK client for learning about software synthesis

Copyright 2018 murray foster */

#include "dsp_types.h"

struct dsp_operation *dsp_global_operation_head_processing;
struct dsp_operation *dsp_global_operation_head;

struct dsp_connection *dsp_global_connection_graph;
struct dsp_connection *dsp_global_connection_graph_processing;
struct dsp_translation_connection *dsp_global_translation_connection_graph_processing;
struct dsp_bus *dsp_global_bus_head;
struct dsp_operation *dsp_global_translation_graph;

struct dsp_port_out *dsp_main_ins;
struct dsp_port_in *dsp_main_outs;

struct dsp_operation *dsp_optimized_main_ins;
struct dsp_operation *dsp_optimized_main_outs;

char* dsp_generate_object_id() {
  char *id;
  int id_len = 37;
  uuid_t uuid;
  char uuid_str[37];

  uuid_generate(uuid);
  uuid_unparse_lower(uuid, uuid_str);

  id = (char *)malloc(sizeof(char) * id_len);
  snprintf(id, id_len, "%s", uuid_str);

  return id;
}

struct dsp_port_in* dsp_port_in_init(const char *port_name, int fifo_size) {

  struct dsp_port_in *new_port = (struct dsp_port_in*)malloc(sizeof(struct dsp_port_in));
  new_port->prev = NULL;
  new_port->next = NULL;
  new_port->name = malloc(sizeof(char) * strlen(port_name) + 1);
  strcpy((char *)new_port->name, port_name);
  new_port->id = dsp_generate_object_id();
  new_port->remove = 0;
  
  return new_port;
}

void dsp_port_in_insert_head(struct dsp_port_in *head_port, struct dsp_port_in *port_in) {
  if(head_port == NULL) {
    head_port= port_in;
    return;
  }
  head_port->prev = port_in;
  port_in->next = head_port;
  head_port = port_in;
}

void dsp_port_in_insert_tail(struct dsp_port_in *head_port, struct dsp_port_in *port_in) {
  struct dsp_port_in *temp_port = head_port;
  if(temp_port == NULL) {
    head_port = port_in;
    return;
  }
  while(temp_port->next != NULL)
    temp_port = temp_port->next;
  temp_port->next = port_in;
  port_in->prev = temp_port;
}

void dsp_port_in_list(struct dsp_port_in *head_port) {
  struct dsp_port_in *temp_port = head_port;
  while(temp_port != NULL) {
    printf("%s", temp_port->name);
    temp_port = temp_port->next;
  }
}

void dsp_port_in_list_reverse(struct dsp_port_in *head_port) {
  struct dsp_port_in *temp_port = head_port;
  if(temp_port == NULL)
    return;
  while(temp_port != NULL)
    temp_port = temp_port->next;
  while(temp_port != NULL) {
    printf("%s", temp_port->name);
    temp_port = temp_port->prev;
  } 
}

struct dsp_port_out* dsp_port_out_init(const char *port_name, int audio) {
  struct dsp_port_out *new_port = (struct dsp_port_out*)malloc(sizeof(struct dsp_port_out));
  new_port->prev = NULL;
  new_port->next = NULL;
  new_port->name = malloc(sizeof(char) * strlen(port_name) + 1);
  strcpy((char *)new_port->name, port_name);
  new_port->id = dsp_generate_object_id();
  new_port->remove = 0;
  new_port->audio = audio;
  new_port->value = (float)0; /* should we init this at 0? */
  return new_port;
}

void dsp_port_out_insert_head(struct dsp_port_out *head_port, struct dsp_port_out *port_out) {
  if(head_port == NULL) {
    head_port= port_out;
    return;
  }
  port_out->prev = port_out;
  port_out->next = head_port;
  head_port = port_out;
}

void dsp_port_out_insert_tail(struct dsp_port_out *head_port, struct dsp_port_out *port_out) {
  struct dsp_port_out *temp_port = head_port;
  if(temp_port == NULL) {
    head_port = port_out;
    return;
  }
  while(temp_port->next != NULL)
    temp_port = temp_port->next;
  temp_port->next = port_out;
  port_out->prev = temp_port;
}

void dsp_port_out_list(struct dsp_port_out *head_port) {
  struct dsp_port_out *temp_port = head_port;
  while(temp_port != NULL) {
    printf("%s", temp_port->name);
    temp_port = temp_port->next;
  }
}

struct dsp_connection* dsp_connection_init(const char *id_out,
					   const char *id_in,
					   struct dsp_port_out *port_out,
					   struct dsp_port_in *port_in) {
  struct dsp_connection *new_connection = (struct dsp_connection*)malloc(sizeof(struct dsp_connection));
  new_connection->id = dsp_generate_object_id();
  new_connection->prev = NULL;
  new_connection->next = NULL;
  new_connection->remove = 0;

  new_connection->id_out = malloc(sizeof(char) * strlen(id_out) + 1);
  strcpy((char *)new_connection->id_out, id_out);
  
  new_connection->id_in = malloc(sizeof(char) * strlen(id_in) + 1);
  strcpy((char *)new_connection->id_in, id_in);

  return new_connection;
}

void dsp_connection_insert_head(struct dsp_connection *head_connection, struct dsp_connection *connection) {
  if(head_connection == NULL) {
    head_connection = connection;
    return;
  }
  head_connection->prev = connection;
  connection->next = head_connection;
  head_connection = connection;
}

void dsp_connection_insert_tail(struct dsp_connection *head_connection, struct dsp_connection *connection) {
  struct dsp_connection *temp_connection = head_connection;
  if(temp_connection == NULL) {
    head_connection = connection;
    return;
  }
  while(temp_connection->next != NULL)
    temp_connection = temp_connection->next;
  temp_connection->next = connection;
  connection->prev = temp_connection;
}

void dsp_connection_list(struct dsp_connection *head_connection, void (*func)(struct dsp_connection*) ) {
  struct dsp_connection *temp_connection = head_connection;
  while(temp_connection != NULL) {
    func(temp_connection);
    temp_connection = temp_connection->next;
  }
}

void dsp_connection_printf(struct dsp_connection *connection) {
  printf("%s\n", connection->id);
}

void dsp_connection_list_reverse(struct dsp_connection *head_connection, void (*func)(struct dsp_connection*) ) {
  struct dsp_connection *temp_connection = head_connection;
  struct dsp_connection *current_connection;
  if(temp_connection == NULL)
    return;
  while(temp_connection != NULL) {
    current_connection = temp_connection;
    temp_connection = temp_connection->next;
  }
  temp_connection = current_connection;
  while(temp_connection != NULL) {
    current_connection = temp_connection;
    temp_connection = temp_connection->prev;
    func(current_connection);
  } 
}

void dsp_connection_terminate(struct dsp_connection *connection) {
  connection->id_out = NULL;
  connection->id_in = NULL;
  free((char *)connection->id);
  connection->id = NULL;
  free((char *)connection);
  connection = NULL;
}

struct dsp_module* dsp_module_init(const char *module_name,
				   void (*dsp_function) (struct dsp_operation*, int),
                                   void (*dsp_osc_listener_function) (struct dsp_operation*, int),
				   struct dsp_operation *(*dsp_optimize) (char*, struct dsp_module*),
				   dsp_parameter dsp_param,
				   struct dsp_port_in *ins,
				   struct dsp_port_out *outs) {
  struct dsp_module *new_module = (struct dsp_module*)malloc(sizeof(struct dsp_module));
  new_module->prev = NULL;
  new_module->next = NULL;
  new_module->name = malloc(sizeof(char) * strlen(module_name) + 1);
  strcpy((char *)new_module->name, module_name);
  new_module->id = dsp_generate_object_id();
  new_module->dsp_function = dsp_function;
  new_module->dsp_osc_listener_function = dsp_osc_listener_function;
  new_module->dsp_optimize = dsp_optimize;
  new_module->dsp_param = dsp_param;

  new_module->remove = 0;
  new_module->bypass = 0;
  new_module->osc_listener = 0;

  new_module->ins = ins;
  new_module->outs = outs;
  return new_module;
}

void dsp_module_insert_head(struct dsp_module *head_module, struct dsp_module *new_module) {
  if(head_module == NULL) {
    head_module = new_module;
    return;
  }
  head_module->prev = new_module;
  new_module->next = head_module;
  head_module = new_module;
}

void dsp_module_insert_tail(struct dsp_module *head_module, struct dsp_module *new_module) {
  struct dsp_module *temp_module;
  if(head_module == NULL) {
    head_module = new_module;
    return;
  }
  temp_module = head_module;
  while(temp_module->next != NULL)
    temp_module = temp_module->next;
  temp_module->next = new_module;
  new_module->prev = temp_module;
}

void dsp_module_list(struct dsp_module *head_module) {
  struct dsp_module *temp_module = head_module;
  while(temp_module != NULL) {
    printf("%s", temp_module->name);
    temp_module = temp_module->next;
  }
}

void dsp_module_list_reverse(struct dsp_module *head_module) {
  struct dsp_module *temp_module = head_module;
  if(temp_module == NULL)
    return;
  while(temp_module != NULL)
    temp_module = temp_module->next;
  while(temp_module != NULL) {
    printf("%s", temp_module->name);
    temp_module = temp_module->prev;
  } 
}

struct dsp_bus_port* dsp_bus_port_init(char *port_name, int output) {
  struct dsp_bus_port *new_port = (struct dsp_bus_port*)malloc(sizeof(struct dsp_bus_port));
  new_port->prev = NULL;
  new_port->next = NULL;
  new_port->name = malloc(sizeof(char) * strlen(port_name) + 1);
  strcpy((char *)new_port->name, port_name);
  new_port->id = dsp_generate_object_id();
  new_port->remove = 0;
  new_port->output = output;
  return new_port;
}

void dsp_bus_port_insert_head(struct dsp_bus_port *head_port, struct dsp_bus_port *bus_port) {
  if(head_port == NULL) {
    head_port= bus_port;
    return;
  }
  bus_port->prev = bus_port;
  bus_port->next = head_port;
  head_port = bus_port;
}

void dsp_bus_port_insert_tail(struct dsp_bus_port *head_port, struct dsp_bus_port *bus_port) {
  struct dsp_bus_port *temp_port;
  if(temp_port == NULL) {
    head_port = bus_port;
    return;
  }
  temp_port = head_port;
  while(temp_port->next != NULL)
    temp_port = temp_port->next;
  temp_port->next = bus_port;
  bus_port->prev = temp_port;
}

void dsp_bus_port_list(struct dsp_bus_port *head_port) {
  struct dsp_bus_port *temp_port = head_port;
  while(temp_port != NULL) {
    printf("%s", temp_port->name);
    temp_port = temp_port->next;
  }
}

void dsp_bus_port_list_reverse(struct dsp_bus_port *head_port) {
  struct dsp_bus_port *temp_port = head_port;
  if(temp_port == NULL)
    return;
  while(temp_port != NULL)
    temp_port = temp_port->next;
  while(temp_port != NULL) {
    printf("%s", temp_port->name);
    temp_port = temp_port->prev;
  } 
}

struct dsp_bus* dsp_bus_init(const char *bus_name) {
  struct dsp_bus *new_bus = (struct dsp_bus*)malloc(sizeof(struct dsp_bus));
  new_bus->name = malloc(sizeof(char) * strlen(bus_name) + 1);
  strcpy((char *)new_bus->name, bus_name);
  new_bus->id = dsp_generate_object_id();
  new_bus->prev = NULL;
  new_bus->next = NULL;
  new_bus->up = NULL;
  new_bus->down = NULL;
  new_bus->remove = 0;
  new_bus->bypass = 0;
  new_bus->ins = NULL;
  new_bus->outs = NULL;
  new_bus->dsp_module_head = NULL;
  return new_bus;
}

void dsp_bus_insert_head(struct dsp_bus *head_bus, struct dsp_bus *new_bus) {
  if(head_bus == NULL) {
    head_bus = new_bus;
    return;
  }
  head_bus->prev = new_bus;
  new_bus->next = head_bus;
  head_bus = new_bus;
}

void dsp_bus_insert_tail(struct dsp_bus *head_bus, struct dsp_bus *new_bus) {
  struct dsp_bus *temp_bus = head_bus;
  if(temp_bus == NULL) {
    head_bus = new_bus;
    return;
  }
  while(temp_bus->next != NULL)
    {
      temp_bus = temp_bus->next;
    }
  temp_bus->next = new_bus;
  new_bus->prev = temp_bus;
}

void dsp_bus_insert_tail_deep(struct dsp_bus *head_bus, struct dsp_bus *new_bus) {
  struct dsp_bus *temp_bus = head_bus;
  if(temp_bus == NULL) {
    head_bus = new_bus;
    return;
  }
  while(temp_bus->down != NULL)
    {
      printf("DOWN temp_bus: %s\n", temp_bus->name);
      temp_bus = temp_bus->down;
    }
  while(temp_bus->next != NULL)
    {
      printf("NEXT temp_bus: %s\n", temp_bus->name);
      temp_bus = temp_bus->next;
    }
  temp_bus->next = new_bus;
  new_bus->prev = temp_bus;
}

void dsp_bus_list(struct dsp_bus *head_bus) {
  struct dsp_bus *temp_bus = head_bus;
  while(temp_bus != NULL) {
    printf("%s", temp_bus->name);
    temp_bus = temp_bus->next;
  }
}

void dsp_bus_list_reverse(struct dsp_bus *head_bus) {
  struct dsp_bus *temp_bus = head_bus;
  if(temp_bus == NULL)
    return;
  while(temp_bus != NULL)
    temp_bus = temp_bus->next;
  while(temp_bus != NULL) {
    printf("%s", temp_bus->name);
    temp_bus = temp_bus->prev;
  } 
}

struct dsp_operation* dsp_operation_init(char *dsp_id) {
  struct dsp_operation *new_operation = (struct dsp_operation*)malloc(sizeof(struct dsp_operation));
  new_operation->id = dsp_generate_object_id();
  new_operation->dsp_id = malloc(sizeof(char) * strlen(dsp_id) + 1);
  strcpy((char *)new_operation->dsp_id, dsp_id);
  new_operation->prev = NULL;
  new_operation->next = NULL;
  new_operation->ins = NULL;
  new_operation->outs = NULL;
  new_operation->module = NULL;
  return new_operation;
}

void dsp_operation_insert_head(struct dsp_operation *head_operation, struct dsp_operation *new_operation) {
  if(head_operation == NULL) {
    head_operation = new_operation;
    return;
  }

  head_operation->prev = new_operation;
  new_operation->next = head_operation;
  head_operation = new_operation;
}

void dsp_operation_insert_tail(struct dsp_operation *head_operation, struct dsp_operation *new_operation) {
  struct dsp_operation *temp_operation = head_operation;

  if(temp_operation == NULL) {
    head_operation = new_operation;
    return;
  }

  while(temp_operation->next != NULL)
    {
      temp_operation = temp_operation->next;
    }
  temp_operation->next = new_operation;
  new_operation->prev = temp_operation;
}

void dsp_operation_insert_behind(struct dsp_operation *existing_operation, struct dsp_operation *new_operation) {
  new_operation->prev = existing_operation->prev;
  new_operation->next = existing_operation;
  existing_operation->prev = new_operation;
  if( existing_operation == dsp_global_operation_head_processing )
    dsp_global_operation_head_processing = new_operation;
}

void dsp_operation_insert_ahead(struct dsp_operation *existing_operation, struct dsp_operation *new_operation) {
  if( existing_operation->next == NULL ) {
    printf("NULL existing_operation->next\n");
    existing_operation->next = new_operation;
    printf("NULL existing_operation->next, assigned\n");    
  } else {
    printf("NOT NULL existing_operation->next\n");    
    new_operation->next = existing_operation->next;
    existing_operation->next->prev = new_operation;
    existing_operation->next = new_operation;
    new_operation->prev = existing_operation;
  }
}

struct dsp_translation_connection* dsp_translation_connection_init(struct dsp_connection *connection,
								   char *id_out,
								   char *id_in,
								   struct dsp_operation *op_out,
								   struct dsp_operation *op_in,
								   struct dsp_operation_sample *sample_out,
								   struct dsp_operation_sample *sample_in) {
  struct dsp_translation_connection *new_translation_conn = (struct dsp_translation_connection*)malloc(sizeof(struct dsp_translation_connection));
  new_translation_conn->prev = NULL;
  new_translation_conn->next = NULL;
  new_translation_conn->id_out = NULL;
  new_translation_conn->id_in = NULL;
  new_translation_conn->operation_out = NULL;
  new_translation_conn->operation_in = NULL;
  new_translation_conn->sample_out = NULL;
  new_translation_conn->sample_in = NULL;

  new_translation_conn->connection = connection;
  
  new_translation_conn->id_out = malloc(sizeof(char) * strlen(id_out) + 1);
  strcpy((char *)new_translation_conn->id_out, id_out);
  
  new_translation_conn->id_in = malloc(sizeof(char) * strlen(id_in) + 1);
  strcpy((char *)new_translation_conn->id_in, id_in);

  new_translation_conn->operation_out = op_out;
  new_translation_conn->operation_in = op_in;

  new_translation_conn->sample_out = sample_out;
  new_translation_conn->sample_in = sample_in;

  return new_translation_conn;
}

void dsp_translation_connection_insert_tail(struct dsp_translation_connection *head_translation_conn, struct dsp_translation_connection *new_translation_conn) {
  struct dsp_translation_connection *temp_translation_conn = head_translation_conn;
  if(temp_translation_conn == NULL) {
    head_translation_conn = new_translation_conn;
    return;
  }
  while(temp_translation_conn->next != NULL)
    {
      temp_translation_conn = temp_translation_conn->next;
    }
  temp_translation_conn->next = new_translation_conn;
  new_translation_conn->prev = temp_translation_conn;
}

struct dsp_sample* dsp_sample_init(unsigned short blocksize, float value) {
  struct dsp_sample *new_sample = (struct dsp_sample*)malloc(sizeof(struct dsp_sample));
  new_sample->id = dsp_generate_object_id();
  new_sample->value = malloc(sizeof(float) * blocksize);
  new_sample->value[0] = value;

  for(int p = 1; p < blocksize; p++) {
    new_sample->value[p] = 0.0f;
  }
  
  return new_sample;
}

struct dsp_operation_sample* dsp_operation_sample_init(char *dsp_id, unsigned short blocksize, float value, int init_sample) {
  struct dsp_operation_sample *new_operation_sample = (struct dsp_operation_sample*)malloc(sizeof(struct dsp_operation_sample));
  new_operation_sample->id = dsp_generate_object_id();
  new_operation_sample->dsp_id = malloc(sizeof(char) * strlen(dsp_id) + 1);
  strcpy((char *)new_operation_sample->dsp_id, dsp_id);
  new_operation_sample->prev = NULL;
  new_operation_sample->next = NULL;
  new_operation_sample->summands = NULL;

  if( init_sample )
    new_operation_sample->sample = dsp_sample_init(blocksize, value);
  
  return new_operation_sample;
}

void dsp_operation_sample_insert_head(struct dsp_operation_sample *head_sample, struct dsp_operation_sample *new_sample) {
  if(head_sample == NULL) {
    head_sample = new_sample;
    return;
  }
  head_sample->prev = new_sample;
  new_sample->next = head_sample;
  head_sample = new_sample;
}

void dsp_operation_sample_insert_tail(struct dsp_operation_sample *head_sample, struct dsp_operation_sample *new_sample) {
  struct dsp_operation_sample *temp_sample = head_sample;
  if(temp_sample == NULL) {
    head_sample = new_sample;
    return;
  }
  while(temp_sample->next != NULL)
    {
      temp_sample = temp_sample->next;
    }
  temp_sample->next = new_sample;
  new_sample->prev = temp_sample;
}

struct dsp_bus_port*
_dsp_search_bus_port(struct dsp_bus *head_bus, char *id) {
  struct dsp_bus *temp_bus = head_bus;
  struct dsp_bus_port *temp_bus_port = NULL;
  
  while(temp_bus != NULL) {

    /* check 'in' bus ports */
    temp_bus_port = temp_bus->ins;
    while( temp_bus_port != NULL ) {
      if( strcmp(temp_bus_port->id, id) == 0 )
        return temp_bus_port; 
      temp_bus_port = temp_bus_port->next;
    }

    /* check 'out' bus ports */
    temp_bus_port = temp_bus->outs;
    while( temp_bus_port != NULL ) {
      if( strcmp(temp_bus_port->id, id) == 0 )
        return temp_bus_port; 
      temp_bus_port = temp_bus_port->next;
    }
    
    if( (temp_bus_port = _dsp_search_bus_port(temp_bus->down, id)) != NULL)
      return temp_bus_port;
    else if( (temp_bus_port = _dsp_search_bus_port(temp_bus->next, id)) != NULL)
      return temp_bus_port;
    else
      return NULL;
  }
} /* _dsp_search_bus_port */

struct dsp_bus_port*
dsp_find_bus_port(char *id) {
  printf("dsp.c::dsp_find_bus_port()\n");
  printf("id: %s\n", id);
  return _dsp_search_bus_port(dsp_global_bus_head, id);
} /* dsp_find_bus_port */

struct dsp_bus_port*
_dsp_search_bus_port_out(struct dsp_bus *head_bus, char *id) {
  struct dsp_bus *temp_bus = head_bus;
  struct dsp_bus_port *temp_bus_port = NULL;
  
  while(temp_bus != NULL) {

    /* check 'out' bus ports */
    temp_bus_port = temp_bus->outs;
    while( temp_bus_port != NULL ) {
      printf("_dsp_search_bus_port_out::id: %s\n", id);
      printf("_dsp_search_bus_port_out::temp_bus_port->id: %s\n\n", temp_bus_port->id);
      if( strcmp(temp_bus_port->id, id) == 0 )
        return temp_bus_port; 
      temp_bus_port = temp_bus_port->next;
    }
    
    if( (temp_bus_port = _dsp_search_bus_port(temp_bus->down, id)) != NULL)
      return temp_bus_port;
    else if( (temp_bus_port = _dsp_search_bus_port(temp_bus->next, id)) != NULL)
      return temp_bus_port;
    else
      return NULL;
  }
} /* _dsp_search_bus_port_out */

struct dsp_bus_port*
dsp_find_bus_port_out(char *id) {
  printf("dsp.c::dsp_find_bus_port_out()\n");
  printf("id: %s\n", id);
  return _dsp_search_bus_port_out(dsp_global_bus_head, id);
} /* dsp_find_bus_port_out */

struct dsp_bus_port*
_dsp_search_bus_port_in(struct dsp_bus *head_bus, char *id) {
  struct dsp_bus *temp_bus = head_bus;
  struct dsp_bus_port *temp_bus_port = NULL;
  
  while(temp_bus != NULL) {

    /* check 'in' bus ports */
    temp_bus_port = temp_bus->ins;
    while( temp_bus_port != NULL ) {
      printf("_dsp_search_bus_port_in::id: %s\n", id);
      printf("_dsp_search_bus_port_in::temp_bus_port->id: %s\n\n", temp_bus_port->id);      
      if( strcmp(temp_bus_port->id, id) == 0 )
        return temp_bus_port; 
      temp_bus_port = temp_bus_port->next;
    }
    
    if( (temp_bus_port = _dsp_search_bus_port(temp_bus->down, id)) != NULL)
      return temp_bus_port;
    else if( (temp_bus_port = _dsp_search_bus_port(temp_bus->next, id)) != NULL)
      return temp_bus_port;
    else
      return NULL;
  }
} /* _dsp_search_bus_port_in */

struct dsp_bus_port*
dsp_find_bus_port_in(char *id) {
  printf("dsp.c::dsp_find_bus_port_in()\n");
  printf("id: %s\n", id);
  return _dsp_search_bus_port_in(dsp_global_bus_head, id);
} /* dsp_find_bus_port_in */

struct dsp_module*
_dsp_search_module(struct dsp_bus *head_bus, char *id) {
  struct dsp_bus *temp_bus = head_bus;
  struct dsp_module *temp_module = NULL;
  
  while(temp_bus != NULL) {
    temp_module = temp_bus->dsp_module_head;
    while( temp_module != NULL ) {
      if( strcmp(temp_module->id, id) == 0 )
        return temp_module; 
      temp_module = temp_module->next;
    }
    if( (temp_module = _dsp_search_module(temp_bus->down, id)) != NULL)
      return temp_module;
    else if( (temp_module = _dsp_search_module(temp_bus->next, id)) != NULL)
      return temp_module;
    else
      return NULL;
  }
} /* _dsp_search_module */

struct dsp_module*
dsp_find_module(char *id) {
  /* printf("dsp.c::dsp_find_module()\n"); */
  /* printf("id: %s\n", id); */
  return _dsp_search_module(dsp_global_bus_head, id);
} /* dsp_find_module */

struct dsp_port_out*
_dsp_search_port_out(struct dsp_bus *head_bus, char *id) {
  struct dsp_bus *temp_bus = head_bus;
  struct dsp_module *temp_module = NULL;
  struct dsp_bus_port *temp_bus_port = NULL;
  struct dsp_port_out *temp_port_out = NULL;
  
  while(temp_bus != NULL) {

    temp_bus_port = temp_bus->outs;
    while( temp_bus_port != NULL ) {
      temp_port_out = temp_bus_port->out;
      printf("_dsp_search_port_out::id: %s\n", id);      
      printf("_dsp_search_port_out::temp_port_out->id: %s\n\n", temp_port_out->id);
      if( strcmp(temp_port_out->id, id) == 0)
        return temp_port_out;
      temp_bus_port = temp_bus_port->next;
    }
    
    temp_module = temp_bus->dsp_module_head;
    while( temp_module != NULL ) {
      temp_port_out = temp_module->outs;
      while( temp_port_out != NULL) {
      printf("_dsp_search_port_out::id: %s\n", id);        
      printf("_dsp_search_port_out::temp_port_out->id: %s\n\n", temp_port_out->id);
        if( strcmp(temp_port_out->id, id) == 0)
          return temp_port_out;
        temp_port_out = temp_port_out->next;
      }
      temp_module = temp_module->next;
    }
    
    if( (temp_port_out = _dsp_search_port_out(temp_bus->down, id)) != NULL)
      return temp_port_out;
    else if( (temp_port_out = _dsp_search_port_out(temp_bus->next, id)) != NULL)
      return temp_port_out;
    else
      return NULL;
  }
} /* _dsp_search_port_out */

struct dsp_port_out*
dsp_find_port_out(char *id) {
  printf("dsp.c::dsp_find_port_out()\n");
  printf("id: %s\n\n", id);

  struct dsp_port_out *temp_port_out = NULL;
  temp_port_out = dsp_main_ins;
  while(temp_port_out != NULL) {
    if(strcmp(temp_port_out->id, id) == 0)
      return temp_port_out;
    temp_port_out = temp_port_out->next;
  }
  
  return _dsp_search_port_out(dsp_global_bus_head, id);
} /* dsp_find_port_out */

struct dsp_port_in*
_dsp_search_port_in(struct dsp_bus *head_bus, char *id) {
  struct dsp_bus *temp_bus = head_bus;
  struct dsp_module *temp_module = NULL;
  struct dsp_bus_port *temp_bus_port = NULL;
  struct dsp_port_in *temp_port_in = NULL;
  
  while(temp_bus != NULL) {

    temp_bus_port = temp_bus->ins;
    while( temp_bus_port != NULL ) {
      temp_port_in = temp_bus_port->in;
      printf("_dsp_search_port_in::id: %s\n", id);      
      printf("_dsp_search_port_in::temp_port_in->id: %s\n\n", temp_port_in->id);      
      if( strcmp(temp_port_in->id, id) == 0)
        return temp_port_in;
      temp_bus_port = temp_bus_port->next;
    }
    
    temp_module = temp_bus->dsp_module_head;
    while( temp_module != NULL ) {
      temp_port_in = temp_module->ins;
      printf("_dsp_search_port_in::id: %s\n", id);      
      printf("_dsp_search_port_in::temp_port_in->id: %s\n\n", temp_port_in->id);            
      while( temp_port_in != NULL) {
        if( strcmp(temp_port_in->id, id) == 0)
          return temp_port_in;
        temp_port_in = temp_port_in->next;
      }
      temp_module = temp_module->next;
    }
    
    if( (temp_port_in = _dsp_search_port_in(temp_bus->down, id)) != NULL)
      return temp_port_in;
    else if( (temp_port_in = _dsp_search_port_in(temp_bus->next, id)) != NULL)
      return temp_port_in;
    else
      return NULL;
  }
} /* _dsp_search_port_in */

struct dsp_port_in*
dsp_find_port_in(char *id) {
  printf("dsp.c::dsp_find_port_in()\n");
  printf("id: %s\n", id);

  struct dsp_port_in *temp_port_in = NULL;
  temp_port_in = dsp_main_outs;
  while(temp_port_in != NULL) {
    if(strcmp(temp_port_in->id, id) == 0)
      return temp_port_in;
    temp_port_in = temp_port_in->next;
  }
  
  return _dsp_search_port_in(dsp_global_bus_head, id);
} /* dsp_find_port_in */

struct dsp_port_out*
dsp_find_main_in_port_out(char *id) {
  printf("dsp.c::dsp_find_main_in_port_out()\n");
  printf("id: %s\n", id);

  struct dsp_port_out *temp_port_out = NULL;
  temp_port_out = dsp_main_ins;
  while(temp_port_out != NULL) {
    if(strcmp(temp_port_out->id, id) == 0) {
      return temp_port_out;
    }
    temp_port_out = temp_port_out->next;
  }
  return NULL;
} /* dsp_find_main_in_port_out */

struct dsp_port_in*
dsp_find_main_out_port_in(char *id) {
  printf("dsp.c::dsp_find_main_out_port_in()\n");
  printf("id: %s\n", id);

  struct dsp_port_in *temp_port_in = NULL;
  temp_port_in = dsp_main_outs;
  while(temp_port_in != NULL) {
    if(strcmp(temp_port_in->id, id) == 0) {
      return temp_port_in;
    }
    temp_port_in = temp_port_in->next;
  }
  return NULL;
} /* dsp_find_main_out_port_in */

struct dsp_port_out*
_dsp_search_module_port_out(struct dsp_bus *head_bus, char *id) {
  struct dsp_bus *temp_bus = head_bus;
  struct dsp_module *temp_module = NULL;
  struct dsp_port_out *temp_port_out = NULL;
  
  while(temp_bus != NULL) {    
    temp_module = temp_bus->dsp_module_head;
    while( temp_module != NULL ) {
      temp_port_out = temp_module->outs;
      while( temp_port_out != NULL) {
        if( strcmp(temp_port_out->id, id) == 0)
          return temp_port_out;
        temp_port_out = temp_port_out->next;
      }
      temp_module = temp_module->next;
    }
    
    if( (temp_port_out = _dsp_search_port_out(temp_bus->down, id)) != NULL)
      return temp_port_out;
    else if( (temp_port_out = _dsp_search_port_out(temp_bus->next, id)) != NULL)
      return temp_port_out;
    else
      return NULL;
  }
} /* _dsp_search_module_port_out */

struct dsp_port_out*
dsp_find_module_port_out(char *id) {
  printf("dsp.c::dsp_find_module_port_out()\n");
  printf("id: %s\n", id);
  
  return _dsp_search_module_port_out(dsp_global_bus_head, id);
} /* dsp_find_module_port_out */

struct dsp_port_in*
_dsp_search_module_port_in(struct dsp_bus *head_bus, char *id) {
  struct dsp_bus *temp_bus = head_bus;
  struct dsp_module *temp_module = NULL;
  struct dsp_port_in *temp_port_in = NULL;
  
  while(temp_bus != NULL) {    
    temp_module = temp_bus->dsp_module_head;
    while( temp_module != NULL ) {
      temp_port_in = temp_module->ins;
      while( temp_port_in != NULL) {
        if( strcmp(temp_port_in->id, id) == 0)
          return temp_port_in;
        temp_port_in = temp_port_in->next;
      }
      temp_module = temp_module->next;
    }
    
    if( (temp_port_in = _dsp_search_port_in(temp_bus->down, id)) != NULL)
      return temp_port_in;
    else if( (temp_port_in = _dsp_search_port_in(temp_bus->next, id)) != NULL)
      return temp_port_in;
    else
      return NULL;
  }
} /* _dsp_search_module_port_in */

struct dsp_port_in*
dsp_find_module_port_in(char *id) {
  printf("dsp.c::dsp_find_module_port_in()\n");
  printf("id: %s\n", id);
  
  return _dsp_search_module_port_in(dsp_global_bus_head, id);
} /* dsp_find_module_port_in */

struct dsp_port_out*
_dsp_search_bus_port_port_out(struct dsp_bus *head_bus, char *id) {
  struct dsp_bus *temp_bus = head_bus;
  struct dsp_bus_port *temp_bus_port = NULL;
  struct dsp_port_out *temp_port_out = NULL;
  
    temp_bus_port = temp_bus->outs;
    while( temp_bus_port != NULL ) {
      temp_port_out = temp_bus_port->out;
      if( strcmp(temp_port_out->id, id) == 0)
        return temp_port_out;
      temp_bus_port = temp_bus_port->next;
    }
    
    if( (temp_port_out = _dsp_search_port_out(temp_bus->down, id)) != NULL)
      return temp_port_out;
    else if( (temp_port_out = _dsp_search_port_out(temp_bus->next, id)) != NULL)
      return temp_port_out;
    else
      return NULL;
} /* _dsp_search_bus_port_port_out */

struct dsp_port_out*
dsp_find_bus_port_port_out(char *id) {
  printf("dsp.c::dsp_find_bus_port_port_out()\n");
  printf("id: %s\n", id);
  
  return _dsp_search_bus_port_port_out(dsp_global_bus_head, id);
} /* dsp_find_bus_port_port_out */

struct dsp_port_in*
_dsp_search_bus_port_port_in(struct dsp_bus *head_bus, char *id) {
  struct dsp_bus *temp_bus = head_bus;
  struct dsp_bus_port *temp_bus_port = NULL;
  struct dsp_port_in *temp_port_in = NULL;
  
  temp_bus_port = temp_bus->ins;
  while( temp_bus_port != NULL ) {
    temp_port_in = temp_bus_port->in;
    if( strcmp(temp_port_in->id, id) == 0)
      return temp_port_in;
    temp_bus_port = temp_bus_port->next;
  }
    
  if( (temp_port_in = _dsp_search_port_in(temp_bus->down, id)) != NULL)
    return temp_port_in;
  else if( (temp_port_in = _dsp_search_port_in(temp_bus->next, id)) != NULL)
    return temp_port_in;
  else
    return NULL;
} /* _dsp_search_bus_port_port_in */

struct dsp_port_in*
dsp_find_bus_port_port_in(char *id) {
  printf("dsp.c::dsp_find_bus_port_port_in()\n");
  printf("id: %s\n", id);
  
  return _dsp_search_bus_port_port_in(dsp_global_bus_head, id);
} /* dsp_find_bus_port_port_in */

struct dsp_module*
_dsp_search_module_from_port(struct dsp_bus *head_bus, char *id) {
  struct dsp_bus *temp_bus = head_bus;
  struct dsp_module *temp_module = NULL;
  struct dsp_port_in *temp_port_in = NULL;
  struct dsp_port_out *temp_port_out = NULL;
  
  while(temp_bus != NULL) {    
    temp_module = temp_bus->dsp_module_head;
    printf("dsp_types.c::_dsp_search_module_from_port(), assigned temp_module\n");
    while( temp_module != NULL ) {
      printf("dsp_types.c::_dsp_search_module_from_port(), temp_module not NULL\n");      
      temp_port_in = temp_module->ins;      
      while( temp_port_in != NULL) {
        if( strcmp(temp_port_in->id, id) == 0)
          return temp_module;
        temp_port_in = temp_port_in->next;
      }
      temp_port_out = temp_module->outs;
      while( temp_port_out != NULL) {
        if( strcmp(temp_port_out->id, id) == 0)
          return temp_module;
        temp_port_out = temp_port_out->next;
      }

      printf("dsp_types.c::_dsp_search_module_from_port(), done searching\n");
      temp_module = temp_module->next;
    }
    
    if( (temp_module = _dsp_search_module_from_port(temp_bus->down, id)) != NULL)
      return temp_module;
    else if( (temp_module = _dsp_search_module_from_port(temp_bus->next, id)) != NULL)
      return temp_module;
    else
      return NULL;
  }
} /* _dsp_search_module_from_port */

struct dsp_module*
dsp_get_module_from_port(char *id) {
  printf("dsp.c::dsp_get_module_from_port()\n");
  printf("id: %s\n", id);

  return _dsp_search_module_from_port(dsp_global_bus_head, id);
} /* dsp_get_module_from_port */


struct dsp_bus*
_dsp_search_bus(struct dsp_bus *head_bus, char *id) {
  struct dsp_bus *temp_bus = head_bus;
  struct dsp_bus *found_bus = NULL;

  while(temp_bus != NULL) {
    if( strcmp(temp_bus->id, id) == 0 )
      return temp_bus;
    if( (found_bus = _dsp_search_bus(temp_bus->down, id)) != NULL)
      return found_bus;
    else if( (found_bus = _dsp_search_bus(temp_bus->next, id)) != NULL)
      return found_bus;
    else
      return NULL;
  }
} /* _dsp_search_bus */

struct dsp_bus*
dsp_find_bus(char *id) {
  printf("dsp.c::dsp_find_bus()\n");
  printf("id: %s\n", id);
  return _dsp_search_bus(dsp_global_bus_head, id);
} /* dsp_find_bus */
