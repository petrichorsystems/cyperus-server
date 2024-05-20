#! /usr/bin/python3

import liblo
from liblo import *
import queue, sys, time
import time
import uuid

responses = queue.Queue()

class OscServer(ServerThread):
    def __init__(self):
        ServerThread.__init__(self, 7400)

    @make_method('/cyperus/get/graph/id', 'sis')
    def osc_get_graph_id(self, path, args):
        print("received '/cyperus/get/graph/id'")
        print('path', path)
        print('args', args)
        responses.put(args)
                
    @make_method(None, None)
    def fallback(self, path, args):
        print("fallback, received '{}'".format(path))
        print("fallback, args '{}'".format(args))
        
def test_get_graph_id(dest):
    request_id = str(uuid.uuid4())    
    liblo.send(dest, "/cyperus/get/graph/id", request_id)
    response = responses.get()
    print(response)


if __name__ == '__main__':
    #outgoing connection
    dest = liblo.Address(7500)

    #incoming server
    server = OscServer()

    server.start()

    test_get_graph_id(dest)

    input("press enter to quit...\n")
