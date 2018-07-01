/* dsp_bus.c
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

#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);

#include "cyperus.h"
#include "rtqueue.h"
#include "libcyperus.h"
#include "dsp.h"

struct dsp_port_in* dsp_port_in_init(const char *port_name, int fifo_size) {
  struct dsp_port_in *new_port = (struct dsp_port_in*)malloc(sizeof(struct dsp_port_in));
  new_port->prev = NULL;
  new_port->next = NULL;
  new_port->name = port_name;
  new_port->remove = 0;
  new_port->values = rtqueue_init(fifo_size);
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
  new_port->name = port_name;
  new_port->remove = 0;
  new_port->audio = audio;
  new_port->value = 0.0; /* should we init this at 0? */
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

void dsp_port_out_list_reverse(struct dsp_port_out *head_port) {
  struct dsp_port_out *temp_port = head_port;
  if(temp_port == NULL)
    return;
  while(temp_port != NULL)
    temp_port = temp_port->next;
  while(temp_port != NULL) {
    printf("%s", temp_port->name);
    temp_port = temp_port->prev;
  } 
}

struct dsp_connection* dsp_connection_init(const char *id_out,
					   const char *id_in,
					   struct dsp_port_out *port_out,
					   struct dsp_port_in *port_in) {
  struct dsp_connection *new_connection = (struct dsp_connection*)malloc(sizeof(struct dsp_connection));
  new_connection->prev = NULL;
  new_connection->next = NULL;
  new_connection->remove = 0;
  new_connection->id_out = id_out;
  new_connection->id_in = id_in;
  new_connection->out_value = port_out->value;
  new_connection->in_values = port_in->values;
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

void dsp_connection_list(struct dsp_connection *head_connection) {
  struct dsp_connection *temp_connection = head_connection;
  while(temp_connection != NULL) {
    temp_connection = temp_connection->next;
  }
}

void dsp_connection_list_reverse(struct dsp_connection *head_connection) {
  struct dsp_connection *temp_connection = head_connection;
  if(temp_connection == NULL)
    return;
  while(temp_connection != NULL)
    temp_connection = temp_connection->next;
  while(temp_connection != NULL) {
    temp_connection = temp_connection->prev;
  } 
}

struct dsp_module* dsp_module_init(const char *module_name,
				   void (*dsp_function) (struct dsp_module*, int, int),
				   dsp_parameter dsp_param,
				   struct dsp_port_in *ins,
				   struct dsp_port_out *outs) {
  struct dsp_module *new_module = (struct dsp_module*)malloc(sizeof(struct dsp_module));
  new_module->prev = NULL;
  new_module->next = NULL;
  new_module->name = module_name;
  new_module->dsp_function = dsp_function;
  new_module->dsp_param = dsp_param;
  new_module->remove = 0;
  new_module->bypass = 0;
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
  struct dsp_module *temp_module = new_module;
  if(head_module == NULL) {
    head_module = new_module;
    return;
  }
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
  new_port->name = port_name;
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
  new_bus->name = bus_name;
  new_bus->prev = NULL;
  new_bus->next = NULL;
  new_bus->up = NULL;
  new_bus->down = NULL;
  new_bus->remove = 0;
  new_bus->bypass = 0;
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
    fprintf(stderr, "hey");
    return;
  }
  while(temp_bus->next != NULL)
    {
      fprintf(stderr, "temp_bus: %s\n", temp_bus->name);
      temp_bus = temp_bus->next;
    }
  temp_bus->next = new_bus;
  new_bus->prev = temp_bus;
}

void dsp_bus_insert_tail_deep(struct dsp_bus *head_bus, struct dsp_bus *new_bus) {
  struct dsp_bus *temp_bus = head_bus;
  if(temp_bus == NULL) {
    head_bus = new_bus;
    fprintf(stderr, "hey");
    return;
  }
  while(temp_bus->down != NULL)
    {
      fprintf(stderr, "DOWN temp_bus: %s\n", temp_bus->name);
      temp_bus = temp_bus->down;
    }
  while(temp_bus->next != NULL)
    {
      fprintf(stderr, "NEXT temp_bus: %s\n", temp_bus->name);
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
