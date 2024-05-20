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
        
    @make_method('/cyperus/add/module/lowpass', 'sff')
    def osc_add_module_lowpass(self, path, args):
        print("received '/cyperus/add/module/lowpass'")
        responses.put(args)

    @make_method('/cyperus/edit/module/lowpass', 'sff')
    def osc_edit_module_lowpass(self, path, args):
        print("received '/cyperus/edit/module/lowpass'")
        responses.put(args)

    @make_method('/cyperus/list/module_port', 'ss')
    def osc_list_module_port(self, path, args):
        print("received '/cyperus/list/module_port'")
        responses.put(args)
        
    @make_method(None, None)
    def fallback(self, path, args):
        print("fallback, received '{}'".format(path))
        print("fallback, args '{}'".format(args))
        
def test_single_channel_single_bus_sine_follower_lowpass(dest):
    mains = {'in': [],
             'out': []}
    bus_main0_uuid = None
    bus_ports  = {'in': [],
                  'out': []}
    lowpass_module_uuid = None
    lowpass_module_ports = {'in': [],
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

    liblo.send(dest, "/cyperus/add/module/lowpass", "/{}".format(bus_main0_uuid), 1.0, 440.0)
    response = responses.get()
    lowpass_module_uuid = response[0]    
    
    liblo.send(dest, "/cyperus/list/module_port", "/{}?{}".format(bus_main0_uuid,
                                                                  lowpass_module_uuid))
    response = responses.get()
    print('bloc_processor response: {}'.format(response))
    raw_lowpass_module_ports = response[1].split('\n')
    print(raw_lowpass_module_ports)
    outs = False
    for elem in filter(None, raw_lowpass_module_ports):
        if 'out:' in elem:
            outs = True
        elif 'in:' in elem:
            pass
        elif outs:
            lowpass_module_ports['out'].append(elem)
        else:
            lowpass_module_ports['in'].append(elem)
    print('lowpass_module_ports', lowpass_module_ports)


    liblo.send(dest,
               "/cyperus/add/connection",
               mains['in'][0],
               "/{}:{}".format(bus_main0_uuid,
                               bus_ports['in'][0].split('|')[0]))
    response = responses.get()


    liblo.send(dest,
               "/cyperus/add/connection",
               "/{}:{}".format(bus_main0_uuid,
                               bus_ports['in'][0].split('|')[0]),
               "/{}?{}<{}".format(bus_main0_uuid,
                                  lowpass_module_uuid,
                                  lowpass_module_ports['in'][0].split('|')[0]))
    response = responses.get()

    
    liblo.send(dest,
               "/cyperus/add/connection",
               "/{}?{}>{}".format(bus_main0_uuid,
                                  lowpass_module_uuid,
                                  lowpass_module_ports['out'][0].split('|')[0]),
               "/{}:{}".format(bus_main0_uuid,
                               bus_ports['out'][0].split('|')[0]))
    response = responses.get()

    liblo.send(dest,
               "/cyperus/add/connection",
               "/{}:{}".format(bus_main0_uuid,
                               bus_ports['out'][0].split('|')[0]),
               mains['out'][0])

    response = responses.get()
    
    liblo.send(dest, "/cyperus/edit/module/lowpass", "/{}?{}".format(bus_main0_uuid, lowpass_module_uuid), 1.0, 1200.0)
    response = responses.get()

    time.sleep(5)

    for num in range(0, 1000):
        print("sending /cyperus/edit/module/lowpass", "/{}?{}".format(bus_main0_uuid, lowpass_module_uuid), 1.0, float(num))
        liblo.send(dest, "/cyperus/edit/module/lowpass", "/{}?{}".format(bus_main0_uuid, lowpass_module_uuid), 1.0, float(num))
        response = responses.get()
        time.sleep(0.01)

if __name__ == '__main__':
    #outgoing connection
    dest = liblo.Address(97211)

    #incoming server
    server = OscServer()

    server.start()

    test_single_channel_single_bus_sine_follower_lowpass(dest)

    input("press enter to quit...\n")
