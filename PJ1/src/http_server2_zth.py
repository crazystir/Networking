#!/usr/bin/env python

from common import errprint
import http_server1
import socket
import select


def readServer(inputs, messages, s):
    recv, addr = s.accept()
    errprint('A new socket')
    recv.setblocking(0)
    messages[recv] = ''
    inputs.append(recv)


def readData(messages, data, s):
    errprint('receive "%s" from %s' % (data, s.getpeername()))
    messages[s] += data


def closeRead(inputs, outputs, s):
    errprint('Finish reading, start to write')
    inputs.remove(s)
    #outputs.append(s)


def closeWrite(outputs, messages, s):
    errprint('Finish writing, end')
    #outputs.remove(s)
    s.close()
    del messages[s]


def closeServer(inputs, outputs, messages, s):
    errprint('%s goes wrong', (s.getpeername()))
    if s in inputs:
        inputs.remove(s)
    if s in outputs:
        outputs.remove(s)
    s.close()
    del messages[s]


def execute(messages, s):
    try:
        content = messages[s]
        http_server1.processHtml(s, content)
        return 0
    except:
        errprint('Unexpected error')
        return -1


def listen(server):
    inputs = [server]
    outputs = []
    messages = {}
    while inputs:
        readable, writable, exceptional = select.select(inputs, outputs, inputs)
        for s in readable:
            if s is server:
                readServer(inputs, messages, s)
            else:
                try:
                    while True:
                        data = s.recv(1024)
                        print (data, len(data))
                        if data:
                            readData(messages, data, s)
                        if not data or (len(messages[s]) >= 4 and messages[s][-4:] == '\r\n\r\n'):
                            closeRead(inputs, outputs, s)
                            break
                    execute(messages, s)
                    closeWrite(outputs, messages, s)
                except socket.error:
                    errprint('Socket blocked')
        for s in exceptional:
            closeServer(inputs, outputs, messages, s)



def main():
    s = http_server1.initServer(5)
    s.setblocking(0)
    listen(s)


if __name__ == '__main__':
    main()
