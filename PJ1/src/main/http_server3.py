#!/usr/bin/env python

from common import errprint
import socket
import http_server1
import select
import fcntl
import os
import Queue

ERROR = -1
READING_SCOKET = 0
READING_FILE = 1
WRITING_SOCKET = 2
BLOCK = 3
FINISHED = 4

SOCKET = 0
FILE = 1

def readServer(inputs, messages, s):
    recv, addr = s.accept()
    errprint('A new socket')
    recv.setblocking(0)
    messages[recv] = {'status': READING_SCOKET, 'content': '', 'type': SOCKET}
    inputs.append(recv)


def readSocketData(messages, data, s):
    if messages[s].get('content', None) is None:
        messages[s]['content'] = ''
    messages[s]['content'] += data

def readFileData(messages, data, s):
    if messages[s].get('content', None) is None:
        messages[s]['content'] = Queue.Queue()
    messages[s]['content'].put(data)


def openFile(inputs, messages, s):
    errprint('Finish reading, start to read file')
    messages[s]['file_content'] = Queue.Queue()
    messages[s]['file_content'].put(http_server1.successHeader())
    header_dict = http_server1.parseHeader(messages[s]['content'])
    if header_dict is None:
        errprint('Error during parsing header')
        messages[s]['status'] = ERROR
        return
    f = http_server1.openFile(header_dict.get('pathname', None))
    if f is None:
        errprint('Server3: Can not find the file, return the \"Not Found\" page')
        f = http_server1.openFile(http_server1.ERROR_PATH)
        messages[s]['file_content'].put(http_server1.errorHeader())
    # Set the file to be non-blocking and add connection to the socket and the file
    fcntl.fcntl(f, fcntl.F_SETFL, os.O_NONBLOCK)
    messages[s]['status'] = READING_FILE
    messages[s]['file'] = f
    messages[f] = {'status': READING_FILE, 'content': Queue.Queue(), 'socket': s, 'type': FILE}
    inputs.append(f)


def closeStream(inputs, outputs, messages, s):
    if s in inputs:
        inputs.remove(s)
    if s in outputs:
        outputs.remove(s)
    del messages[s]
    s.close()


def readSocket(messages, inputs, s):
    if messages[s]['status'] != BLOCK and messages[s]['status'] != READING_SCOKET:
        errprint('Invalid status in socket %s' % s.getpeername())
        messages[s]['status'] = ERROR
        return
    messages[s]['status'] = READING_SCOKET
    try:
        while True:
            data = s.recv(1024)
            if data:
                readSocketData(messages, data, s)
            if not data or (len(messages[s]['content']) >= 4 and messages[s]['content'][-4:] == '\r\n\r\n'):
                inputs.remove(s)
                openFile(inputs, messages, s)
                break
    except socket.error:
        errprint('The socket %s is blocked' % (s.getpeername(),))
        messages[s]['status'] = BLOCK


def finishReadingFile(messages, inputs, outputs, f):
    s = messages[f]['socket']
    content = messages[f]['content']
    while not content.empty():
        messages[s]['file_content'].put(content.get())
    outputs.append(messages[f]['socket'])
    inputs.remove(f)
    del messages[f]
    f.close()


def readFile(messages, inputs, outputs, f):
    if messages[f]['status'] != BLOCK and messages[f]['status'] != READING_FILE:
        errprint('Invalid status in file %s' % f.name)
        messages[f]['status'] = ERROR
        return
    messages[f]['status'] = READING_FILE
    try:
        while True:
            data = f.read(1024)
            if data == '' or data == '\n' or data == '\r\n' \
                    or (len(data) >= 2 and data[-2:] == '\n\n') or (len(data) >= 4 and data[-4:] == '\r\n\r\n'):
                finishReadingFile(messages, inputs, outputs, f)
                break
            readFileData(messages, data, f)
    except IOError:
        errprint('The file %s is blocked' % f.name)
        messages[f]['status'] = BLOCK


def writeSocket(messages, s):
    content = messages[s].get('file_content', None)
    if content is None:
        errprint('The connecting doesn\'t have content')
        messages[s]['status'] = ERROR
        return
    messages[s]['status'] = WRITING_SOCKET
    try:
        while not content.empty():
            con = content.queue[0]
            s.send(con)
            content.get()
        if content.empty():
            messages[s]['status'] = FINISHED
    except socket.error:
        errprint('Block during writing socket')
        messages[s]['status'] = BLOCK


def listen(server):
    inputs = [server]
    outputs = []
    messages = {}
    while inputs:
        readable, writable, exceptional = select.select(inputs, outputs, inputs)
        for s in readable:
            if s is server:
                readServer(inputs, messages, s)
            elif messages[s]['type'] == SOCKET:
                errprint('Reading socket')
                readSocket(messages, inputs, s)
                if messages.get(s, None) is not None and messages[s]['status'] == ERROR:
                    closeStream(inputs, outputs, messages, s)
                    continue
            elif messages[s]['type'] == FILE:
                errprint('Reading file')
                readFile(messages, inputs, outputs, s)
                if messages.get(s, None) is not None and messages[s]['status'] == ERROR:
                    closeStream(inputs, outputs, messages, s)
                    continue
            else:
                errprint('Unknown stream status')
                closeStream(inputs, outputs, messages, s)
        for s in writable:
            if messages[s]['type'] != SOCKET:
                errprint('Wrong type of output stream')
                closeStream(inputs, outputs, messages, s)
            errprint('Respond to the client')
            writeSocket(messages, s)
            if messages[s]['status'] == ERROR:
                errprint('Unknown error during writing socket')
            if messages[s]['status'] == FINISHED:
                errprint('Finish writing, end')
                closeStream(inputs, outputs, messages, s)
        for s in exceptional:
            errprint('Exceptional stream')
            closeStream(inputs, outputs, messages, s)


def main():
    errprint('%s start running' % __file__)
    s = http_server1.initServer(5)
    s.setblocking(0)
    listen(s)


if __name__ == '__main__':
    main()
