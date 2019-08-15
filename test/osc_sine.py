#! /usr/bin/python3

import liblo
from liblo import *
import queue, sys, time
import time

responses = queue.Queue()

class OscServer(ServerThread):
    def __init__(self):
        ServerThread.__init__(self, 97217)
        
    @make_method('/cyperus/address', 'ss')
    def osc_address_handler(self, path, args):
        s = args
        responses.put(s)
        print("received '/cyperus/address'")

    @make_method('/cyperus/list/main', 's')
    def osc_list_main_handler(self, path, args):
        s = args
        responses.put(s)
        print("received '/cyperus/list/main'")

    @make_method('/cyperus/add/bus', 'ssssi')
    def osc_add_bus(self, path, args):
        print("received '/cyperus/add/bus'")

    @make_method('/cyperus/list/bus', 'siis')
    def osc_list_bus(self, path, args):
        print("received '/cyperus/list/bus'")
        responses.put(args)

    @make_method('/cyperus/list/bus_port', 'ss')
    def osc_list_bus_port(self, path, args):
        print("received '/cyperus/list/bus_port'")
        responses.put(args)

    @make_method('/cyperus/add/connection', 'ssi')
    def osc_add_connection(self, path, args):
        print("received '/cyperus/add/connection'")
        print('path', path)
        print('args', args)
        responses.put(args)
        
    @make_method('/cyperus/add/module/sine', 'sfff')
    def osc_add_module_sine(self, path, args):
        print("received '/cyperus/add/module/sine'")
        responses.put(args)

    @make_method('/cyperus/edit/module/sine', 'sfff')
    def osc_edit_module_sine(self, path, args):
        print("received '/cyperus/edit/module/sine'")
        responses.put(args)

    @make_method('/cyperus/list/module_port', 'ss')
    def osc_list_module_port(self, path, args):
        print("received '/cyperus/list/module_port'")
        responses.put(args)
        
    @make_method(None, None)
    def fallback(self, path, args):
        print("fallback, received '{}'".format(path))
        print("fallback, args '{}'".format(args))
        
def test_single_channel_single_bus_sine_follower_delay(dest):
    mains = {'in': [],
             'out': []}
    bus_main0_uuid = None
    bus_ports  = {'in': [],
                  'out': []}
    sine_module_uuid = None
    sine_module_ports = {'in': [],
                          'out': []}
    
    liblo.send(dest, "/cyperus/list/main")
    response = responses.get()
    print('response', response)
    raw_mains = response[0].split('\n')
    outs = False
    for elem in filter(None, raw_mains):
        if elem in 'out:':
            outs = True
        elif elem in 'in:':
            pass
        elif outs:
            mains['out'].append(elem)
        else:
            mains['in'].append(elem)

    liblo.send(dest, "/cyperus/add/bus", "/", "main0", "in", "out")
    liblo.send(dest, "/cyperus/list/bus", "/", 1)
    response = responses.get()
    print('response list bus', response)
    bus_main0_uuid = response[3].split('|')[0]

    print('bus_main0_uuid', bus_main0_uuid)
    
    liblo.send(dest, "/cyperus/list/bus_port", "/{}".format(bus_main0_uuid))
    response = responses.get()

    raw_bus_ports = response[1].split('\n')
    print(raw_bus_ports)
    outs = False
    for elem in filter(None, raw_bus_ports):
        if 'out:' in elem:
            outs = True
        elif 'in:' in elem:
            pass
        elif outs:
            bus_ports['out'].append(elem)
        else:
            bus_ports['in'].append(elem)

    print('bus_ports', bus_ports)
    print('mains', mains)

    print(bus_main0_uuid)
    print(bus_main0_uuid)

    liblo.send(dest, "/cyperus/add/module/sine", "/{}".format(bus_main0_uuid),440.0, 1.0, 0.0)
    response = responses.get()
    sine_module_uuid = response[0]    
    
    liblo.send(dest, "/cyperus/list/module_port", "/{}?{}".format(bus_main0_uuid,
                                                                  sine_module_uuid))
    response = responses.get()
    print('response: {}'.format(response))

    raw_sine_module_ports = response[1].split('\n')
    print(raw_sine_module_ports)

    outs = False
    for elem in filter(None, raw_sine_module_ports):
        if 'out:' in elem:
            outs = True
        elif 'in:' in elem:
            pass
        elif outs:
            sine_module_ports['out'].append(elem)
        else:
            sine_module_ports['in'].append(elem)
    print('sine_module_ports', sine_module_ports)
    
    liblo.send(dest,
               "/cyperus/add/connection",
               "/{}?{}>{}".format(bus_main0_uuid,
                                  sine_module_uuid,
                                  sine_module_ports['out'][0].split('|')[0]),
               "/{}:{}".format(bus_main0_uuid,
                               bus_ports['out'][0].split('|')[0]))
    response = responses.get()

    liblo.send(dest,
               "/cyperus/add/connection",
               "/{}:{}".format(bus_main0_uuid,
                               bus_ports['out'][0].split('|')[0]),
               mains['out'][0])

    response = responses.get()

    time.sleep(5)
    
    liblo.send(dest, "/cyperus/edit/module/sine", "/{}?{}".format(bus_main0_uuid, sine_module_uuid), 220.0, 1.0, 0.0)
    response = responses.get()

    time.sleep(5)
    
    liblo.send(dest, "/cyperus/edit/module/sine", "/{}?{}".format(bus_main0_uuid, sine_module_uuid), 110.0, 1.0, 0.0)
    response = responses.get()

if __name__ == '__main__':
    #outgoing connection
    dest = liblo.Address(97211)

    #incoming server
    server = OscServer()

    server.start()

    test_single_channel_single_bus_sine_follower_delay(dest)

    input("press enter to quit...\n")
