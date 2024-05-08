/* osc_handlers.c
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

Copyright 2015 murray foster */

#include "osc_handlers.h"

void osc_error(int num, const char *msg, const char *path)
{
  printf("liblo server error %d in path %s: %s\n", num, path, msg);
  fflush(stdout);
}

int osc_address_handler(const char *path, const char *types, lo_arg **argv,
			int argc, void *data, void *user_data) {
  char *request_id = (char *) argv[0];
  char *new_host_out = (char *)argv[1];
  char *new_port_out = (char *)argv[2];
  printf("hit osc_address_handler\n");
  osc_change_address(request_id, new_host_out, new_port_out);

  return 0;
} /* osc_address_handler */

int osc_list_main_handler(const char *path, const char *types, lo_arg **argv,
			   int argc, void *data, void *user_data)
{
	char *request_id, *mains_str = NULL;
	struct dsp_port_out *temp_port_out;
	struct dsp_port_in *temp_port_in;
	bool multipart;
  

	request_id = (char *)argv[0];
	mains_str = malloc(sizeof(char) * ((37 * (jackcli_channels_in +
						  jackcli_channels_out)) +
					   4 + /* strlen("in:\n") */
					   5 + /* strlen("out:\n") */
					   1));
	strcpy(mains_str, "in:\n");
	/* process main inputs */
	temp_port_out = dsp_main_ins;
	while(temp_port_out != NULL) {
		strcat(mains_str, temp_port_out->id);
		strcat(mains_str, "\n");
		temp_port_out = temp_port_out->next;
	}
	strcat(mains_str, "out:\n");
	/* process main outputs */
	temp_port_in = dsp_main_outs;
	while(temp_port_in != NULL) {
		strcat(mains_str, temp_port_in->id);
		strcat(mains_str, "\n");
		temp_port_in = temp_port_in->next;
	}

	multipart = false;
	lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
	lo_send(lo_addr_send,"/cyperus/list/main", "siis", request_id, 0, multipart, mains_str);
	free(mains_str);
	free(lo_addr_send);
	return 0;
} /* osc_list_main_handler */
			 
int osc_list_bus_handler(const char *path, const char *types, lo_arg **argv,
			   int argc, void *data, void *user_data)
{
	char *request_id = NULL;
	char *bus_id, *result_str = NULL;
	int list_type = 0;
	size_t result_str_size = 0;

	char *partial_result_str = NULL;
	size_t partial_result_str_size = 0;
  
	struct dsp_bus *head_bus = NULL;
	struct dsp_bus_port *temp_bus_port = NULL;
	int count_bus_ports;;
	char *bus_ins_str, *bus_outs_str;
	int root_level = 0;

	char *part_result_str = NULL;
	int current_index, last_break, last_cutoff, copy_index = 0;

	bool multipart = false;
  
	request_id = (char *)argv[0];
	bus_id = (char *)argv[1];
	list_type = argv[2]->i;

	printf("path: <%s>\n", path);
  
	/* list types
	   0 - peer
	   1 - all peers
	   2 - direct descendant
	   3 - all descendants */

	if ( !strcmp(bus_id, "00000000-0000-0000-0000-000000000000") ||
	    !strcmp(bus_id, "") ) {
		head_bus = dsp_global_bus_head;
		root_level = 1;
	}
	else {
		head_bus = dsp_find_bus(bus_id);
	}
  
	if (head_bus == NULL) {
		/* no buses, return new-line char-as-str */
		result_str = "\n";
	} else {
		switch(list_type) {
		case 0: /* list peer */
			result_str = osc_string_build_bus_list(head_bus, root_level, "|", 1, 0);
			break;
		case 1: /* list all peers */
			result_str = osc_string_build_bus_list(head_bus, root_level, "|", 0, 0);
			break;
		case 2: /* list direct descendant */
			result_str = osc_string_build_bus_list(head_bus, root_level, "|", 1, 1);
			break;
		case 3: /* list all direct descendants */
			result_str = osc_string_build_bus_list(head_bus, root_level, "|", 0, 1);
			break;
		default: /* ? */
			break;
		}
	}
  
	if (result_str == NULL)
		result_str = "\n";
  
	lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  
	/* split bus list into 768-byte chunks */
	if (strlen(result_str) > 768) {
		multipart = true;
		for(current_index=0; current_index<strlen(result_str); current_index++) {
			if (result_str[current_index] == '\n')
				last_break = current_index;
			if ((current_index - last_cutoff) > 768) {	
				part_result_str = malloc(sizeof(char) * (last_break - last_cutoff + 1));
				for(copy_index=last_cutoff; copy_index<last_break; copy_index++)
					part_result_str[copy_index - last_cutoff] = result_str[copy_index];
				part_result_str[copy_index] = '\0';
				last_cutoff = last_break;
				if (last_cutoff == strlen(result_str - 1))
					multipart = false;
				lo_send(lo_addr_send,"/cyperus/list/bus", "siisis", request_id, 0, multipart, bus_id, list_type, part_result_str);
				free(part_result_str);
			}
		}
	}
	if (multipart) {
		for(current_index=last_cutoff; current_index<strlen(result_str); current_index++) {
			result_str[current_index - last_cutoff] = result_str[current_index];
		}
		result_str[current_index - last_cutoff] = '\0';
	}
	multipart = false;
	lo_send(lo_addr_send,"/cyperus/list/bus", "siisis", request_id, 0, multipart, bus_id, list_type, result_str);
	free(lo_addr_send);
	if (strcmp(result_str, "\n"))
		free(result_str);
	return 0;
} /* osc_list_bus_handler */

			 
int osc_list_bus_port_handler(const char *path, const char *types, lo_arg **argv,
			   int argc, void *data, void *user_data)
{
	struct dsp_bus *temp_bus = NULL;
	char *request_id, *path_str, *result_str = NULL;
	size_t result_str_size = 0;
	struct dsp_bus_port *temp_bus_port = NULL;
	bool multipart;
  
	request_id = (char *)argv[0];
	path_str = (char *)argv[1];

	printf("path: <%s>\n", path);

	temp_bus = dsp_find_bus(path_str);

	result_str_size = 4;
	result_str = malloc(sizeof(char) * (result_str_size + 1));
	strcpy(result_str, "in:\n");
	/* process main inputs */
	temp_bus_port = temp_bus->ins;
	while(temp_bus_port != NULL) {
		result_str_size += strlen(temp_bus_port->id) + 1 + strlen(temp_bus_port->name) + 2;
		result_str = realloc(result_str, sizeof(char) * result_str_size);
		strcat(result_str, temp_bus_port->id);
		strcat(result_str, "|");
		strcat(result_str, temp_bus_port->name);
		strcat(result_str, "\n");
		temp_bus_port = temp_bus_port->next;
	}

	result_str_size += 4;
	result_str = realloc(result_str, sizeof(char) * (result_str_size) + 1);
	strcat(result_str, "out:\n");
	/* process main outputs */
	temp_bus_port = temp_bus->outs;
	while(temp_bus_port != NULL) {
		result_str_size += strlen(temp_bus_port->id) + 1 + strlen(temp_bus_port->name) + 2;
		result_str = realloc(result_str, sizeof(char) * result_str_size);
		strcat(result_str, temp_bus_port->id);
		strcat(result_str, "|");
		strcat(result_str, temp_bus_port->name);
		strcat(result_str, "\n");
		temp_bus_port = temp_bus_port->next;
	}

	multipart = false;
	lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
	lo_send(lo_addr_send,"/cyperus/list/bus_port", "siiss", request_id, 0, multipart, path_str, result_str);
	free(lo_addr_send);

	free(result_str);
	return 0;
} /* osc_list_bus_port_handler */

int osc_add_bus_handler(const char *path, const char *types, lo_arg **argv,
                               int argc, void *data, void *user_data)
{
	int i;
	struct dsp_bus *temp_bus;
	char *request_id, *target_bus_id, *bus_str, *ins_str, *outs_str, *new_id = NULL;
	bool multipart;
	struct dsp_bus *new_bus;
  
	printf("path: <%s>\n", path);

	request_id = (char *)argv[0];
	target_bus_id = (char *)argv[1];
	bus_str = (char *)argv[2];
	ins_str = (char *)argv[3];
	outs_str = (char *)argv[4];

	for(i=0; i < strlen(ins_str); i++)
		if (ins_str[i] == '|')
			ins_str[i] = ',';
	for(i=0; i < strlen(outs_str); i++)
		if (outs_str[i] == '|')
			outs_str[i] = ',';

	new_bus = dsp_bus_init(bus_str);

	dsp_add_bus(target_bus_id, new_bus, ins_str, outs_str);

	new_id = malloc(sizeof(char) * strlen(new_bus->id));
	strcpy(new_id, new_bus->id);
  
	for(i=0; i < strlen(ins_str); i++)
		if (ins_str[i] == ',')
			ins_str[i] = '|';
	for(i=0; i < strlen(outs_str); i++)
		if (outs_str[i] == ',')
			outs_str[i] = '|';

	multipart = false;
	lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
	lo_send(lo_addr_send,"/cyperus/add/bus", "siisssssi", request_id, 0, multipart, target_bus_id, bus_str, ins_str, outs_str, new_id,
		strcmp(new_bus->name, bus_str));
	free(lo_addr_send);
	free(new_id);
  return 0;
} /* osc_add_bus_handler */

int osc_remove_module_handler(const char *path, const char *types, lo_arg ** argv,
                                     int argc, void *data, void *user_data)
{
	int voice;
	int module_no;
  
	printf("path: <%s>\n", path);
	module_no=argv[0]->i;

	printf("removing module #%d..\n",module_no);
	
	dsp_remove_module(0,module_no);
  
  return 0;
} /* osc_remove_module_handler */


int osc_add_connection_handler(const char *path, const char *types, lo_arg **argv,
                                      int argc, void *data, void *user_data)
{
	char *request_id, *path_out, *path_in;
	bool multipart;
	int failed = 0;
  
	printf("path: <%s>\n", path);
  
	request_id = (char *)argv[0];
	path_out = (char *)argv[1];
	path_in = (char *)argv[2];

	failed = dsp_add_connection(path_out, path_in);
	
	multipart = false;
	lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
	lo_send(lo_addr_send,"/cyperus/add/connection", "siissi", request_id, 0, multipart, path_out, path_in, failed);
	lo_address_free(lo_addr_send);

  
	return 0;
} /* osc_add_connection_handler */

int osc_remove_connection_handler(const char *path, const char *types, lo_arg **argv,
                                         int argc, void *data, void *user_data)
{
	char *request_id, *path_out, *path_in;
	bool multipart;
	int failed = 0;
  
	printf("path: <%s>\n", path);

	request_id = (char *)argv[0];
	path_out = (char *)argv[1];
	path_in = (char *)argv[2];

	failed = dsp_remove_connection(path_out, path_in);

	multipart = false;
	lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
	lo_send(lo_addr_send,"/cyperus/remove/connection", "siissi", request_id, 0, path_out, path_in, failed);
	lo_address_free(lo_addr_send);
  
  return 0;
} /* osc_remove_connection_handler */

int osc_list_modules_handler(const char *path, const char *types, lo_arg ** argv,
                                    int argc, void *data, void *user_data)
{
	struct dsp_bus *target_bus = NULL;
	struct dsp_module *target_module = NULL;
	char *request_id = NULL;
	char *path_str = NULL;
	char *module_list = NULL;
	char *result_str = NULL;
	bool multipart;
	printf("path: <%s>\n", path);

	request_id = (char *)argv[0];
	path_str = (char *)argv[1];

	target_module = dsp_find_module(path_str);
  
	if (target_module) {
		result_str = malloc(sizeof(char) * 38);
		strcpy(result_str, target_module->id);
		strcat(result_str, "\n");
		target_module = target_module->next;
		while(target_module != NULL) {
			printf("'in while' -- target_module->id: %s\n", target_module->id);
			result_str = realloc(result_str, sizeof(char) * (strlen(result_str) + strlen(target_module->id) + 2));

			strcat(result_str, target_module->id);
			if ( target_module->next != NULL )
				strcat(result_str, "\n");
			target_module = target_module->next;
		}
	} else {
		result_str = "";
	}

	multipart = false;
	lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
	lo_send(lo_addr_send,"/cyperus/list/module","siis", request_id, 0, multipart, result_str);
	free(lo_addr_send);
	if ( !strcmp(result_str, "") )
		free(result_str);
	return 0;
  
} /* osc_list_modules_handler */

int osc_list_module_port_handler(const char *path, const char *types, lo_arg ** argv,
                                        int argc, void *data, void *user_data)
{
	int count;
	struct dsp_bus *temp_bus;
	struct dsp_module *temp_module = NULL;
	char *request_id, *bus_path, *module_id, *result_str = NULL;
	size_t result_str_size = 0;
	struct dsp_port_in *temp_port_in;
	struct dsp_port_out *temp_port_out;
	char *module_path;
	bool multipart;
	
	request_id = (char *)argv[0];
	module_path = (char *)argv[1];
  
	/* split up path */
	bus_path = malloc(sizeof(char) * (strlen(module_path) - 36));
	snprintf(bus_path, strlen(module_path) - 36, "%s", module_path);
	
	module_id = malloc(sizeof(char) * 37);
	strncpy(module_id, module_path + strlen(module_path) - 36, 37);
  
	temp_module = dsp_find_module(module_id);
  
	result_str_size = 4;
	result_str = malloc(sizeof(char) * (result_str_size + 1));
	strcpy(result_str, "in:\n");
  
	if (temp_module == NULL)
		printf(" NULL DUDE \n");

	temp_port_in = temp_module->ins;
  
	while(temp_port_in != NULL) {
		result_str_size += strlen(temp_port_in->id) + 1 + strlen(temp_port_in->name) + 2;
		result_str = realloc(result_str, sizeof(char) * result_str_size);
		strcat(result_str, temp_port_in->id);
		strcat(result_str, "|");
		strcat(result_str, temp_port_in->name);
		strcat(result_str, "\n");
		temp_port_in = temp_port_in->next;
	}

	result_str_size += 4;
	result_str = realloc(result_str, sizeof(char) * (result_str_size) + 1);
	strcat(result_str, "out:\n");
  
	temp_port_out = temp_module->outs;
	while(temp_port_out != NULL) {
		result_str_size += strlen(temp_port_out->id) + 1 + strlen(temp_port_out->name) + 2;
		result_str = realloc(result_str, sizeof(char) * result_str_size);
		strcat(result_str, temp_port_out->id);
		strcat(result_str, "|");
		strcat(result_str, temp_port_out->name);
		strcat(result_str, "\n");
		temp_port_out = temp_port_out->next;
	}

	multipart = false;
	lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
	lo_send(lo_addr_send,"/cyperus/list/module_port", "siiss", request_id, 0, multipart, module_path, result_str);
	lo_address_free(lo_addr_send);
	free(result_str);
  
	return 0;
} /* osc_list_module_port_handler */

int osc_get_filesystem_cwd_handler(const char *path, const char *types, lo_arg ** argv,
                                        int argc, void *data, void *user_data)
{
	char *request_id = NULL;
	char cwd[PATH_MAX];
	bool multipart;
  
	request_id = (char *)argv[0];
  
	if (getcwd(cwd, sizeof(cwd)) != NULL) {
	} else {
		perror("getcwd() error");
		return 1;
	}

	multipart = false;
	lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
	lo_send(lo_addr_send,"/cyperus/get/filesystem/cwd", "siis", request_id, 0, multipart, cwd);
	lo_address_free(lo_addr_send);
  
  return 0;
  
} /* osc_get_filesystem_cwd_handler */

int osc_list_filesystem_path_handler(const char *path, const char *types, lo_arg ** argv,
				     int argc, void *data, void *user_data)
{
	char *request_id, *dirpath, *filepath, *raw_str, *raw_str_tmp, *result_str;
	char **osc_str;
	int raw_strlen;
	int osc_str_len;
	int i;

	bool filepath_tr_slash;
	DIR *d;
	struct dirent *dir;
	struct stat stat_file;
	int stat_ret;

	time_t modified_time;
	struct tm modified_time_local;
	char modified_time_str[20];
	
	char *file_metadata_str;
	char *filesize_str;
	int tmp_strlen;
	
	int multipart_no, multipart_total;
	
	request_id = dirpath = filepath = raw_str = raw_str_tmp = result_str = file_metadata_str = filesize_str = NULL;
	osc_str = NULL;
	filepath_tr_slash = false;
  
	request_id = (char *)argv[0];
	dirpath = (char *)argv[1];

	raw_strlen = 1;

	osc_str_len = 0;
	d = opendir(dirpath);
	if (d) {
		while ((dir = readdir(d)) != NULL) {

			/* skip hidden files */
			if (dir->d_name[0] == '.')
				if (strcmp(dir->d_name, "..") && strcmp(dir->d_name, ".."))
					continue;
			
			if (dirpath[strlen(dirpath)-1] == '/')
				filepath_tr_slash = true;
			
			tmp_strlen = strlen(dirpath) + strlen(dir->d_name);
			if (!filepath_tr_slash)
				tmp_strlen += 1;

			filepath = malloc(sizeof(char) * (tmp_strlen + 1));
			if (!filepath_tr_slash)
				snprintf(filepath, tmp_strlen+1, "%s/%s", dirpath, dir->d_name);
			else
				snprintf(filepath, tmp_strlen+1, "%s%s", dirpath, dir->d_name);

			
			stat_ret = stat(filepath, &stat_file);
			if (stat_ret) {
				printf("warning: error calling stat() on file %s! error: %d, skipping..\n", filepath, stat_ret);
				free(filepath);				
				continue;
			}
			free(filepath);
			
			tmp_strlen = snprintf(NULL, 0, "%lu", stat_file.st_size);
			filesize_str = malloc(sizeof(char) * (tmp_strlen+1));
			snprintf(filesize_str, tmp_strlen+1, "%lu", stat_file.st_size);

			tmp_strlen = 0;			
			tmp_strlen += strlen(dir->d_name);
			tmp_strlen += 1; /* \t separator */
			tmp_strlen += strlen(filesize_str);
			tmp_strlen += 1; /* \t separator */
			
			if (S_ISDIR(stat_file.st_mode)) {
				tmp_strlen += 9; /* "directory" */
			} else if (S_ISREG(stat_file.st_mode)) {
				tmp_strlen += 4; /* "file" */
			} else {
				printf("filepath points to unknown type\n");
			}

			/* add length for time string (19) + \t (1) */
			tmp_strlen += 20;
			
			file_metadata_str = malloc(sizeof(char) * tmp_strlen + 1);
			memcpy(file_metadata_str, dir->d_name, strlen(dir->d_name));
			file_metadata_str[strlen(dir->d_name)] = '\t';
			memcpy(file_metadata_str+strlen(dir->d_name)+1, filesize_str, strlen(filesize_str));
			file_metadata_str[strlen(dir->d_name)+1+strlen(filesize_str)] = '\t';

			modified_time = stat_file.st_mtime;
			localtime_r(&modified_time, &modified_time_local);
			strftime(modified_time_str, sizeof(modified_time_str), "%Y-%m-%d %H:%M:%S", &modified_time_local);
			
			if (S_ISDIR(stat_file.st_mode)) {
				memcpy(file_metadata_str+strlen(dir->d_name)+1+strlen(filesize_str)+1, "directory", 9);
				file_metadata_str[strlen(dir->d_name)+1+strlen(filesize_str)+1+9] = '\t';
				memcpy(file_metadata_str+strlen(dir->d_name)+1+strlen(filesize_str)+1+9+1, modified_time_str, 19);
				file_metadata_str[tmp_strlen] = '\0';
			} else if (S_ISREG(stat_file.st_mode)) {
				memcpy(file_metadata_str+strlen(dir->d_name)+1+strlen(filesize_str)+1, "file", 4);
				file_metadata_str[strlen(dir->d_name)+1+strlen(filesize_str)+1+4] = '\t';
				memcpy(file_metadata_str+strlen(dir->d_name)+1+strlen(filesize_str)+1+4+1, modified_time_str, 19);
				file_metadata_str[tmp_strlen] = '\0';
			} else {
				printf("filepath points to unknown type\n");
			}		       

			raw_strlen += strlen(file_metadata_str);
			
			if (raw_str == NULL) {
				raw_str = malloc(sizeof(char) * (raw_strlen + 1));
				if (raw_str == NULL) {
					printf("could not allocate memory! exiting..");
					exit(1);
				}
				memcpy(raw_str, file_metadata_str, raw_strlen);
			} else {
				/* increment raw_strlen by 1 to make room for
				 * delimiting newline character */
				raw_strlen += 1;
				raw_str_tmp = realloc(raw_str, sizeof(char) * raw_strlen);
				if (raw_str_tmp == NULL) {
					printf("could not allocate memory! exiting..");
					exit(1);
				}
				raw_str = raw_str_tmp;
				raw_str[raw_strlen - strlen(file_metadata_str) - 2] = '\n';
				memcpy(raw_str+(raw_strlen - strlen(file_metadata_str))-1,
				       file_metadata_str, strlen(file_metadata_str)+1);	      
			}
			free(file_metadata_str);
		}
		closedir(d);
	}

	lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
	osc_str = osc_string_build_osc_str(&osc_str_len, raw_str);
	multipart_total = osc_str_len;	
	if (osc_str_len > 1) {
		for (i=0; i<osc_str_len - 1; i++) {
			multipart_no = i+1;
			lo_send(lo_addr_send,"/cyperus/list/filesystem/path", "siiiss", request_id, 0, multipart_no, multipart_total, dirpath, osc_str[i]);
			free(osc_str[i]);
		}
	} else {
		i = 0;
	}
	multipart_no = i+1;
	lo_send(lo_addr_send,"/cyperus/list/filesystem/path", "siiiss", request_id, 0, multipart_no, multipart_total, dirpath, osc_str[i]);
	lo_address_free(lo_addr_send);

	free(osc_str[i]);
	free(osc_str);
  
  return 0;
  
} /* osc_list_filesystem_path_handler */

int _write_append_filesystem_file(char *openmode, char *filepath, char *content) {
	FILE *fp;
	int error;

	if (strcmp(openmode, "w")  == 0) {
	} else if (strcmp(openmode, "a") == 0) {
	} else {
		printf("unknown open mode: %s (must be 'w' or 'a')!, exiting..\n", openmode);
		exit(1);
	}
	fp = fopen(filepath, openmode);
	error = fprintf(fp, "%s", content);
	if (error < 0)
		printf("error writing to filesystem! returning error..\n");
	fclose(fp);
	return error;
} /* _write_append_filesystem_file */

int osc_write_filesystem_file_handler(const char *path, const char *types, lo_arg ** argv,
				     int argc, void *data, void *user_data)
{
	
	char *request_id, *filepath, *content;
	int error;
	FILE *fp;
	bool multipart;
	
	request_id = filepath = content = NULL;
	error = 0;
  
	request_id = (char *)argv[0];
	filepath = (char *)argv[1];
	content = (char *)argv[2];
	
	if (_write_append_filesystem_file("w", filepath, content) < 0) {
		/* do some error-handling here */
		printf("error-handling!\n");
		error = -1;
	}

	multipart = false;
	lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);	
	lo_send(lo_addr_send,"/cyperus/write/filesystem/file", "siis", request_id, error, 0, filepath);
	lo_address_free(lo_addr_send);
  
  return 0;
  
} /* osc_write_filesystem_file_handler */

int osc_append_filesystem_file_handler(const char *path, const char *types, lo_arg ** argv,
				     int argc, void *data, void *user_data)
{	
	char *request_id, *filepath, *content;
	int error;
	FILE *fp;
	bool multipart;
	
	request_id = filepath = content = NULL;
	error = 0;
  
	request_id = (char *)argv[0];
	filepath = (char *)argv[1];
	content = (char *)argv[2];
	
	if (_write_append_filesystem_file("a", filepath, content) < 0) {
		/* do some error-handling here */
		printf("error-handling!\n");
		error = -1;
	}
	
	lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);	
	lo_send(lo_addr_send,"/cyperus/append/filesystem/file", "siis", request_id, error, multipart, filepath);
	lo_address_free(lo_addr_send);
  
  return 0;
} /* osc_append_filesystem_file_handlers */

int osc_read_filesystem_file_handler(const char *path, const char *types, lo_arg ** argv,
				     int argc, void *data, void *user_data)
{	
	char *request_id, *filepath, *raw_str;
	int error;
	unsigned long len;
	FILE *fp;
	char **osc_str;
	int raw_strlen;
	int osc_str_len;
	int i;
	int multipart_no, multipart_total;
	
	request_id = filepath = raw_str = NULL;
	error = 0;
  
	request_id = (char *)argv[0];
	filepath = (char *)argv[1];
	
	fp = fopen(filepath, "r");
	if (fp)
	{
		fseek (fp, 0, SEEK_END);
		len = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		raw_str = malloc(len+1);
		if (raw_str) {
			fread(raw_str, len, 1, fp);
		}
		fclose (fp);
		raw_str[len] = '\0';
	} else {
		printf("error-handling\n");
		error = -1;
	}

	lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
	osc_str = osc_string_build_osc_str(&osc_str_len, raw_str);
	multipart_total = osc_str_len;

	multipart_no = 0;
	if (osc_str_len > 1) {
		for (i=0; i<osc_str_len - 1; i++) {
			multipart_no = i+1;
			lo_send(lo_addr_send,"/cyperus/read/filesystem/file", "siiiss", request_id, error, multipart_no, multipart_total, filepath, osc_str[i]);
			free(osc_str[i]);
		}
	} else {
		i = 0;
	}
	multipart_no = i+1;
	lo_send(lo_addr_send,"/cyperus/read/filesystem/file", "siiiss", request_id, error, multipart_no, multipart_total, filepath, osc_str[i]);
	lo_address_free(lo_addr_send);

	free(osc_str[i]);
	free(osc_str);
  return 0;
} /* osc_read_filesystem_file_handlers */

int osc_remove_filesystem_file_handler(const char *path, const char *types, lo_arg ** argv,
				       int argc, void *data, void *user_data)
{	
	char *request_id, *filepath;
	int error;
	FILE *fp;
	bool multipart;
	
	request_id = filepath = NULL;
	error = 0;
  
	request_id = (char *)argv[0];
	filepath = (char *)argv[1];
	
	if (unlink(filepath) < 0) {
		/* do some error-handling here */
		printf("error-handling!\n");
		error = -1;
	} 

	multipart = false;
	lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);	
	lo_send(lo_addr_send,"/cyperus/remove/filesystem/file", "siis", request_id, error, multipart, filepath);
	lo_address_free(lo_addr_send);
  
  return 0;
} /* osc_remove_filesystem_file_handlers */

int _unlink_callback(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    int rv = remove(fpath);
    if (rv)
        perror(fpath);
    return rv;
}

int osc_remove_filesystem_dir_handler(const char *path, const char *types, lo_arg ** argv,
				       int argc, void *data, void *user_data)
{	
	char *request_id, *dirpath;
	int error;
	FILE *fp;
	bool multipart;
	
	request_id = dirpath = NULL;
	error = 0;
  
	request_id = (char *)argv[0];
	dirpath = (char *)argv[1];

	/* recursively unlink directory and children */
	nftw(dirpath, _unlink_callback, 64, FTW_DEPTH | FTW_PHYS);	

	multipart = false;
	lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);	
	lo_send(lo_addr_send,"/cyperus/remove/filesystem/dir", "siis", request_id, error, multipart, dirpath);
	lo_address_free(lo_addr_send);
  
  return 0;
} /* osc_remove_filesystem_dir_handlers */


int osc_make_filesystem_dir_handler(const char *path, const char *types, lo_arg ** argv,
				    int argc, void *data, void *user_data)
{	
	char *request_id, *dirpath, *dirname, *fullpath;
	int error;
	FILE *fp;
	bool multipart;
	int fullpath_strlen;
	struct stat dirstat = {0};
	
	request_id = dirpath = dirname = fullpath = NULL;
	error = 0;
	fullpath_strlen = 0;
  
	request_id = (char *)argv[0];
	dirpath = (char *)argv[1];
	dirname = (char *)argv[2];

	fullpath_strlen = strlen(dirpath) + strlen(dirname);
	if (dirpath[strlen(dirpath) - 1] != '/') 
		fullpath_strlen += 1;
	fullpath = malloc(sizeof(char) * fullpath_strlen);

	if (dirpath[strlen(dirpath) - 1] != '/')
		snprintf(fullpath, fullpath_strlen + 1, "%s/%s", dirpath, dirname);
	else
		snprintf(fullpath, fullpath_strlen + 1, "%s%s", dirpath, dirname);

	if (mkdir(fullpath, 0755) == -1) {
		printf("cyperus::osc_handlers.c::osc_make_filesystem_dir_handler(), fullpath: %s could not be created, exiting..\n", fullpath);
		exit(1);
	}
	
	multipart = false;
	lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);	
	lo_send(lo_addr_send,"/cyperus/make/filesystem/dir", "siisss", request_id, error, multipart, dirpath, dirname, fullpath);
	lo_address_free(lo_addr_send);
  
  return 0;
} /* osc_make_filesystem_dir */

/* int osc_add_modules_osc_parameter_assignment_handler(const char *path, const char *types, lo_arg ** argv, int argc, void *data, void *user_data) */
/* { */
/*   printf("osc_add_modules_osc_parameter_assignment()..\n"); */

/*   int idx, num_params, temp_port_path_len; */
/*   char *request_id, *osc_path, *type_str, *temp_port_path = NULL, **port_paths; */
/*   osc_handler_user_defined_t* new_handler; */
  
/*   struct dsp_bus *target_bus = NULL; */
/*   struct dsp_module *temp_module, *target_module = NULL; */
  
/*   printf("path: <%s>\n", path); */

/*   request_id = (char *)argv[0]; */
/*   osc_path = (char *)argv[1]; */
/*   type_str = (char *)argv[2]; */
  
/*   num_params = strlen(type_str); */

/*   port_paths = malloc(sizeof(char*)*num_params); */
/*   for(idx=0; idx<num_params; idx++) { */
/*     temp_port_path = (char *)argv[3+idx]; */
/*     temp_port_path_len = strlen(temp_port_path); */
/*     snprintf(port_paths[idx], temp_port_path_len+1, "%s", temp_port_path); */
/*   } */
/*   new_handler = osc_handler_user_defined_init(osc_path, type_str, num_params, port_paths); */
  
/*   pthread_mutex_lock(&global_osc_handlers_user_defined_lock); */
/*   osc_handler_user_defined_insert_tail(global_osc_handlers_user_defined, */
/*                                        new_handler); */
/*   pthread_mutex_unlock(&global_osc_handlers_user_defined_lock); */
  
/*   lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out); */
/*   lo_send(lo_addr_send,"/cyperus/add/module/osc_metronome","siss", request_id, 0, osc_path, type_str); */
/*   free(lo_addr_send); */
/*   return 0; */
/* } /\* osc_add_modules_osc_parameter_assigment_handler *\/ */

/* int */
/* osc_edit_modules_osc_parameter_assigment_handler(const char *path, const char *types, lo_arg ** argv, */
/* 						int argc, void *data, void *user_data) */
/* { */
/*   char *request_id, *module_path, *module_id; */
/*   char *bus_path; */
/*   struct dsp_bus *target_bus; */
/*   struct dsp_module *target_module; */
/*   float beats_per_minute; */
/*   int count; */

/*   request_id = (char *)argv[0]; */
/*   module_path = (char *)argv[1]; */
/*   beats_per_minute=argv[2]->f; */

/*   /\* printf("osc_edit_modules_osc_parameter_assigment_handler::beats_per_minute: %f\n", beats_per_minute); *\/ */
  
/*   /\* bus_path = malloc(sizeof(char) * (strlen(module_path) - 36)); *\/ */
/*   /\* strncpy(bus_path, module_path, strlen(module_path) - 37); *\/ */

/*   /\* module_id = malloc(sizeof(char) * 37);   *\/ */
/*   /\* strncpy(module_id, module_path + strlen(module_path) - 36, 37);  *\/ */

/*   /\* target_bus = dsp_parse_bus_path(bus_path);   *\/ */
/*   /\* target_module = dsp_find_module(target_bus->dsp_module_head, module_id); *\/ */

/*   /\* dsp_edit_osc_metronome(target_module, beats_per_minute); *\/ */

/*   /\* lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out); *\/ */
/*   /\* lo_send(lo_addr_send,"/cyperus/edit/module/osc_metronome","sisf", request_id, 0, module_id, beats_per_minute); *\/ */
/*   /\* free(lo_addr_send); *\/ */
  
/*   return 0; */
/* } /\* osc_edit_modules_osc_parameter_assigment_handler *\/ */


int
osc_get_graph_id_handler(const char *path, const char *types, lo_arg **argv,
                         int argc, void *data, void *user_data)
{
	char *request_id;
	bool multipart;
	request_id = (char *)argv[0];
	multipart = false;
	lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
	lo_send(lo_addr_send,"/cyperus/get/graph/id","sis", request_id, 0, multipart, dsp_graph_id_get());
	free(lo_addr_send);
  
	return 0;
} /* osc_get_graph_id_handler */


int cyperus_osc_handler(const char *path, const char *types, lo_arg ** argv,
			int argc, struct lo_message_  *data, void *user_data)
{
	/* int i; */
	/* printf("path: <%s>\n", path); */
	/* for (i = 0; i < argc; i++) { */
	/*   printf("arg %d '%c' ", i, types[i]); */
	/*   lo_arg_pp((lo_type)types[i], argv[i]); */
	/*   printf("\n"); */
	/* } */
	int (*handler_ptr)(const char *path, const char *types, lo_arg ** argv,
			   int argc, void *data, void *user_data);
	handler_ptr = NULL;
	if (strcmp(path, "/cyperus/address") == 0)
		handler_ptr = osc_address_handler;
	else if (strcmp(path, "/cyperus/list/main") == 0)
		handler_ptr = osc_list_main_handler;
	else if (strcmp(path, "/cyperus/add/bus") == 0)
		handler_ptr = osc_add_bus_handler;
	else if (strcmp(path, "/cyperus/list/bus") == 0)
		handler_ptr = osc_list_bus_handler;
	else if (strcmp(path, "/cyperus/list/bus_port") == 0)
		handler_ptr = osc_list_bus_port_handler;
	
	else if (strcmp(path, "/cyperus/add/connection") == 0)
		handler_ptr = osc_add_connection_handler;
	else if (strcmp(path, "/cyperus/remove/connection") == 0)
		handler_ptr = osc_remove_connection_handler;
	
	else if (strcmp(path, "/cyperus/list/modules") == 0)
		handler_ptr = osc_list_modules_handler;
	else if (strcmp(path, "/cyperus/list/module_port") == 0)
		handler_ptr = osc_list_module_port_handler;
	
	else if (strcmp(path, "/cyperus/add/module/delay/simple") == 0)
		handler_ptr = osc_add_modules_delay_simple_handler;
	else if (strcmp(path, "/cyperus/edit/module/delay/simple") == 0)
		handler_ptr = osc_edit_modules_delay_simple_handler;
  
	else if (strcmp(path, "/cyperus/add/module/envelope/follower") == 0)
		handler_ptr = osc_add_modules_envelope_follower_handler;
	else if (strcmp(path, "/cyperus/edit/module/envelope/follower") == 0)
		handler_ptr = osc_edit_modules_envelope_follower_handler;
	
	else if (strcmp(path, "/cyperus/add/module/filter/bandpass") == 0)
		handler_ptr = osc_add_modules_filter_bandpass_handler;
	else if (strcmp(path, "/cyperus/edit/module/filter/bandpass") == 0)
		handler_ptr = osc_edit_modules_filter_bandpass_handler;
	
	else if (strcmp(path, "/cyperus/add/module/network/oscsend") == 0)
		handler_ptr = osc_add_modules_network_oscsend_handler;
	else if (strcmp(path, "/cyperus/edit/module/network/oscsend") == 0)
		handler_ptr = osc_edit_modules_network_oscsend_handler;	
	
	else if (strcmp(path, "/cyperus/add/module/oscillator/sine") == 0)
		handler_ptr = osc_add_modules_oscillator_sine_handler;
	else if (strcmp(path, "/cyperus/edit/module/oscillator/sine") == 0)
		handler_ptr = osc_edit_modules_oscillator_sine_handler;
	
	else if (strcmp(path, "/cyperus/add/module/oscillator/triangle") == 0)
		handler_ptr = osc_add_modules_oscillator_triangle_handler;
	else if (strcmp(path, "/cyperus/edit/module/oscillator/triangle") == 0)
		handler_ptr = osc_edit_modules_oscillator_triangle_handler;
	
	else if (strcmp(path, "/cyperus/add/module/oscillator/clock") == 0)
		handler_ptr = osc_add_modules_oscillator_clock_handler;
	else if (strcmp(path, "/cyperus/edit/module/oscillator/clock") == 0)
		handler_ptr = osc_edit_modules_oscillator_clock_handler;
	
	else if (strcmp(path, "/cyperus/add/module/utils/float") == 0)
		handler_ptr = osc_add_modules_utils_float_handler;
	else if (strcmp(path, "/cyperus/edit/module/utils/float") == 0)
		handler_ptr = osc_edit_modules_utils_float_handler;
	
	else if (strcmp(path, "/cyperus/add/module/utils/counter") == 0)
		handler_ptr = osc_add_modules_utils_counter_handler;
	else if (strcmp(path, "/cyperus/edit/module/utils/counter") == 0)
		handler_ptr = osc_edit_modules_utils_counter_handler;
	
	else if (strcmp(path, "/cyperus/add/module/utils/equals") == 0)
		handler_ptr = osc_add_modules_utils_equals_handler;
	else if (strcmp(path, "/cyperus/edit/module/utils/equals") == 0)
		handler_ptr = osc_edit_modules_utils_equals_handler;
	
	else if (strcmp(path, "/cyperus/add/module/utils/spigot") == 0)
		handler_ptr = osc_add_modules_utils_spigot_handler;
	else if (strcmp(path, "/cyperus/edit/module/utils/spigot") == 0)
		handler_ptr = osc_edit_modules_utils_spigot_handler;
	
	else if (strcmp(path, "/cyperus/get/graph/id") == 0)
		handler_ptr = osc_get_graph_id_handler;
	
	else if (strcmp(path, "/cyperus/get/filesystem/cwd") == 0)
		handler_ptr = osc_get_filesystem_cwd_handler;
	
	else if (strcmp(path, "/cyperus/list/filesystem/path") == 0)
		handler_ptr = osc_list_filesystem_path_handler;
	
	else if (strcmp(path, "/cyperus/write/filesystem/file") == 0)
		handler_ptr = osc_write_filesystem_file_handler;
	
	else if (strcmp(path, "/cyperus/append/filesystem/file") == 0)
		handler_ptr = osc_append_filesystem_file_handler;
	
	else if (strcmp(path, "/cyperus/read/filesystem/file") == 0)
		handler_ptr = osc_read_filesystem_file_handler;
	
	else if (strcmp(path, "/cyperus/remove/filesystem/file") == 0)
		handler_ptr = osc_remove_filesystem_file_handler;

	else if (strcmp(path, "/cyperus/remove/filesystem/dir") == 0)
		handler_ptr = osc_remove_filesystem_dir_handler;	

	else if (strcmp(path, "/cyperus/make/filesystem/dir") == 0)
		handler_ptr = osc_make_filesystem_dir_handler;

	
	if (handler_ptr)
		handler_ptr(path, types, argv, argc, data, user_data);
	else {
		printf("we should raise some warning message, default handling, etc\n");
		printf("path: %s\n", path);
		return 1;
	}

	/* if (global_osc_handlers_user_defined != NULL) { */
	/*   osc_handler_user_defined_t *temp_handler; */
	/*   temp_handler = global_osc_handlers_user_defined; */
	/*   while(temp_handler != NULL) { */
	/*     if (strcmp(path, temp_handler->osc_path) == 0) */
	/*       osc_execute_handler_parameter_assignment(temp_handler, argv); */
	/*     temp_handler = temp_handler->next; */
	/*   } */
	/* } */
	
	return 0;
}
