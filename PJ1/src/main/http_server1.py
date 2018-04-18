#!/usr/bin/env python
from __future__ import print_function
from common import errprint
import socket
import sys
import time

ROOT_DIR = '../../html'
DEFAULT_PATH = '/rfc2616.html'
ERROR_PATH = '/error.html'
DEFAULT_HOST = ''
DEFAULT_PORT = 8000
DEFAULT_HEADER = {'Content-Type': 'text/html'}


def header2str(d):
    s = ''
    for k, v in d.iteritems():
        s += k + ': ' + v + '\r\n'
    s += '\r\n'
    return s


# Parse the HTTP header
def parseHeader(content):
    try:
        [header, body] = content.split('\r\n\r\n', 1)
        tmp = header.split('\r\n', 1)
        if len(tmp) < 1:
            errprint('Invalid header')
            return None
        command_line = tmp[0]
        [method, pathname, version] = command_line.split(' ', 2)
        header_dict = {'method': method, 'pathname': pathname, 'version': version}
        print ('pathname', pathname)
        if len(tmp) < 2:
            return header_dict
        header_content = tmp[1]
        for h in header_content.split('\r\n'):
            hl = h.split(' ', 1)
            header_dict[hl[0][:-1]] = hl[1]
    except:
        errprint('Invalid header with unknown error')
        return None
    return header_dict


def parseSysParam():
    if len(sys.argv) > 2:
        errprint('Invalid command')
        return -1
    if len(sys.argv) == 1:
        return DEFAULT_PORT
    try:
        port = int(sys.argv[1])
    except ValueError:
        errprint('Port is not a number')
        return -1
    return port


def initServer(backlog):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    port = parseSysParam()
    s.bind((DEFAULT_HOST, port))
    s.listen(backlog)
    return s


def listen(s):
    conn, addr = s.accept()
    request = ''
    while True:
        data = conn.recv(1024)
        if not data:
            break
        request += data
        if len(request) > 4 and request[-4:] == '\r\n\r\n':
            break
    return conn, request


def openFile(pathname):
    if pathname == '/':
        pathname = DEFAULT_PATH
    try:
        pathname = pathname.lower()
        f = open(ROOT_DIR + pathname, 'r')
    except:
        errprint('Unexpected error during open file')
        return None
    return f


def sendError(conn):
    f = open(ROOT_DIR + ERROR_PATH)
    header = 'HTTP/1.0 404 NOT FOUND\r\n'
    header += header2str(DEFAULT_HEADER)
    conn.send(header)
    while True:
        data = f.read(1024)
        if data == '':
            break
        conn.send(data)
    f.close()


def sendFile(conn, f):
    try:
        header = 'HTTP/1.0 200 OK\r\n'
        header += header2str(DEFAULT_HEADER)
        conn.send(header)
        while True:
            data = f.read(1024)
            if data == '':
                break
            conn.send(data)
    except IOError as e:
        errprint('Unexpeced error during sending file', e.message)
        return None


def processHtml(conn, content):
    header_dict = parseHeader(content)
    if header_dict is not None:
        f = openFile(header_dict['pathname'])
        if f is not None:
            sendFile(conn, f)
            print("Success")
        else:
            sendError(conn)
            print("Not found")

def main():
    print('start')
    s = initServer(3)

    while True:
        conn, content = listen(s)
        processHtml(conn, content)
        conn.close()


if __name__ == '__main__':
    main()
