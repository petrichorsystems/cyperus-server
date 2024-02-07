#! /usr/bin/python3

import json
import liblo
from liblo import *
import os
import queue, sys, time
import time
import uuid

responses = queue.Queue()

class OscServer(ServerThread):
    def __init__(self):
        ServerThread.__init__(self, 7400)

    @make_method('/cyperus/get/filesystem/cwd', 'siis')
    def osc_get_filesystem_cwd(self, path, args):
        print("received '/cyperus/get/filesystem/cwd'")
        responses.put(args)

    @make_method('/cyperus/list/filesystem/path', 'siiiss')
    def osc_list_filesystem_path(self, path, args):
        print("received '/cyperus/list/filesystem/path'")
        responses.put(args)

    @make_method('/cyperus/write/filesystem/file', 'siis')
    def osc_write_filesystem_file(self, path, args):
        print("received '/cyperus/write/filesystem/file'")
        responses.put(args)

    @make_method('/cyperus/append/filesystem/file', 'siis')
    def osc_append_filesystem_file(self, path, args):
        print("received '/cyperus/append/filesystem/file'")
        responses.put(args)

    @make_method('/cyperus/read/filesystem/file', 'siiiss')
    def osc_read_filesystem_file(self, path, args):
        print("received '/cyperus/read/filesystem/file'")
        responses.put(args)
        
    @make_method('/cyperus/remove/filesystem/file', 'siis')
    def osc_remove_filesystem_file(self, path, args):
        print("received '/cyperus/remove/filesystem/file'")
        responses.put(args)

    @make_method('/cyperus/remove/filesystem/dir', 'siis')
    def osc_remove_filesystem_dir(self, path, args):
        print("received '/cyperus/remove/filesystem/dir'")
        responses.put(args)        

    @make_method('/cyperus/make/filesystem/dir', 'siisss')
    def osc_make_filesystem_dir(self, path, args):
        print("received '/cyperus/make/filesystem/dir'")
        responses.put(args)
        
    @make_method('/cyperus/dsp/load', 'f')
    def osc_dsp_load(self, path, args):
        # print("received '/cyperus/dsp/load'")
        # print('path', path)
        # print('args', args)
        pass
        
    @make_method(None, None)
    def fallback(self, path, args):
        print("fallback, received '{}'".format(path))
        print("fallback, args '{}'".format(args))
        
def osc_get_filesystem_cwd():
    request_id = str(uuid.uuid4())    
    liblo.send(dest, "/cyperus/get/filesystem/cwd", request_id)
    response = responses.get()
    return response[-1]

def osc_list_filesystem_path(path):
    request_id = str(uuid.uuid4())    
    liblo.send(dest, "/cyperus/list/filesystem/path", request_id, path)

    fragments = []
    response0 = responses.get()
    fragments.append(response0)

    print(response0)
    
    total_responses = response0[3]
    
    idx = 1
    while(True):
        if idx == total_responses:
            break
        tmp = responses.get()
        fragments.append(tmp)
        idx += 1

    complete_msg = ''
    for fragment in fragments:
        complete_msg += fragment[5]
        
    return complete_msg


def osc_write_filesystem_file(filepath, content):
    request_id = str(uuid.uuid4())
    liblo.send(dest, "/cyperus/write/filesystem/file", request_id, filepath, content)
    response = responses.get()
    
    if response[1] == 0:
        return response[3]
    else:
        raise Exception("failed!")

def osc_append_filesystem_file(filepath, content):
    request_id = str(uuid.uuid4())
    liblo.send(dest, "/cyperus/append/filesystem/file", request_id, filepath, content)
    response = responses.get()
    if response[1] == 0:
        return response[3]
    else:
        raise Exception("failed!")


def osc_read_filesystem_file(filepath):
    request_id = str(uuid.uuid4())    
    liblo.send(dest, "/cyperus/read/filesystem/file", request_id, filepath)

    fragments = []
    response0 = responses.get()

    print('response0', response0)
    
    fragments.append(response0)
    total_responses = response0[3]

    print('total_responses', total_responses)
    
    idx = 1
    while(True):
        if idx == total_responses:
            break
        tmp = responses.get()
        fragments.append(tmp)
        idx += 1

    complete_msg = ''
    for fragment in fragments:
        complete_msg += fragment[5]
        
    return complete_msg

    
def osc_remove_filesystem_file(filepath):
    request_id = str(uuid.uuid4())
    liblo.send(dest, "/cyperus/remove/filesystem/file", request_id, filepath)
    response = responses.get()
    
    if response[1] == 0:
        return response[3]
    else:
        raise Exception("failed!")

def osc_remove_filesystem_dir(dirpath):
    request_id = str(uuid.uuid4())
    liblo.send(dest, "/cyperus/remove/filesystem/dir", request_id, dirpath)
    response = responses.get()
    
    if response[1] == 0:
        return response[3]
    else:
        raise Exception("failed!")
    

def osc_make_filesystem_dir(dirpath, dirname):
    request_id = str(uuid.uuid4())
    liblo.send(dest, "/cyperus/make/filesystem/dir", request_id, dirpath, dirname)
    response = responses.get()
    
    if response[1] == 0:
        return response[5]
    else:
        raise Exception("failed!")

    
def test_get_filesystem_cwd():
    print(osc_get_filesystem_cwd())
    

def test_list_filesystem_path():
    print(osc_list_filesystem_path('/'))    
    print(osc_list_filesystem_path('.'))
    paths = osc_list_filesystem_path('/home/mfoster/')
    print(paths)
    print('len', len(paths))


def test_write_filesystem_file():
    content = None
    with open('test_osc_filesystem_content.json', 'r') as fp:
        content = fp.read()
    print(osc_write_filesystem_file('test/test_preset.json', content))


def test_append_filesystem_file():
    content = None
    content = {}
    print(osc_append_filesystem_file('test/test_preset.json', json.dumps(content)))
    print(osc_append_filesystem_file('test/test_preset.json', json.dumps(content)))
    with open('test_preset.json', 'r') as fp:
        print(fp.read())

def test_read_filesystem_file():
    print(osc_read_filesystem_file('test/test_preset.json'))
        

def test_remove_filesystem_file():
    print(osc_remove_filesystem_file('test/test_preset.json'))

    
def test_remove_filesystem_dir():
    os.makedirs('test/presets');
    print(osc_remove_filesystem_dir('test/presets'))
    

def test_make_filesystem_dir():
    new_dirpath = osc_make_filesystem_dir('../test', 'file.test')
    expected_dirpath = "../test/file.test"
    if new_dirpath != expected_dirpath:
        raise Exception(f"ERROR!, expected dirpath '{expected_dirpath}', but got '{new_dirpath}")
    new_dirpath = osc_make_filesystem_dir('../test/', 'file.test')
    expected_dirpath = "../test/file.test"
    if new_dirpath != expected_dirpath:
        raise Exception(f"ERROR!, expected dirpath '{expected_dirpath}', but got '{new_dirpath}")    

    
if __name__ == '__main__':
    #outgoing connection
    dest = liblo.Address(7500)

    #incoming server
    server = OscServer()

    server.start()

    # test_get_filesystem_cwd()
    # test_list_filesystem_path()
    # test_write_filesystem_file()
    # test_append_filesystem_file()
    # test_read_filesystem_file()
    # test_remove_filesystem_file()
    test_remove_filesystem_dir()
    # test_make_filesystem_dir()
