#!/usr/bin/env python

from __future__ import print_function
from common import errprint
import socket
import sys

DEFAULT_PORT = 80
ERROR = -1
ERROR_BUT_DISPLAY = -2
UNDEFINED = 2
REDIRECT = 1
SUCCEED = 0


# Parse the URL
def parseURL(url_str, context):
    url_str = url_str.lower()
    infos = url_str.split(':')
    if len(infos) != 2 and len(infos) != 3:
        errprint('Invalid URL format')
        context['status'] = ERROR
        return -1
    [http, url] = infos[:2]
    port = DEFAULT_PORT
    filename = '/'
    if http != 'http':
        errprint('Invalid protocol')
        context['status'] = ERROR
        return -1
    if url[:2] != '//':
        errprint('Invalid URL')
        context['status'] = ERROR
        return -1
    url = url[2:]
    url = url.split('/', 1)[0]
    tmp = url_str.split('//', 1)[1].split('/', 1)
    if len(tmp) > 1:
        filename = '/' + tmp[1]
    if len(infos) == 3:
        port = infos[2].split('/')[0]
        try:
            port = int(port)
        except ValueError:
            errprint('Invalid port number')
            context['status'] = ERROR
            return -1
    context['status'] = SUCCEED
    context['host'] = url
    context['port'] = port
    context['filename'] = filename
    return 0


# Parse the HTML header
def parseHeader(content, context):
    [header, body] = content.split('\r\n\r\n', 1)
    tmp = header.split('\r\n', 1)
    if len(tmp) != 2:
        errprint('Invalid response')
        context['status'] = ERROR
        return
    [status_line, header_content]  = tmp
    [version, status, message] = status_line.split(' ', 2)
    status = int(status)
    header_dict = {'version': version, 'status': status, 'message': message}
    context['header'] = header_dict
    for h in header_content.split('\r\n'):
        hl = h.split(': ', 1)
        header_dict[hl[0]] = hl[1]

    if status == 200:
        context['status'] = SUCCEED
        content_type = header_dict.get('Content-Type', '')
        if len(content_type) < 9 or content_type.find('text/html') < 0:
            context['status'] = ERROR
    elif status == 301 or status == 302:
        context['status'] = REDIRECT
    elif status >= 400:
        context['status'] = ERROR_BUT_DISPLAY
    else:
        context['status'] = ERROR


def getContent(host, port, filename):
    print(host, port, filename)
    request_header = 'GET ' + filename + ' HTTP/1.0\r\n'
    request_header += 'Host: ' + host + ':' + str(port) + '\r\n\r\n'
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        s.connect((host, port))
        s.send(request_header)
        response = ''
        while True:
            data = s.recv(1024)
            if not data:
                break
            response += data
        s.close()
    except:
        errprint('Unexpected error during the downloading: ', sys.exc_info())
        return None
    return response


def connect(url, context = None, count = 0):
    # The program may redirect infinitely, so I set the maximum redirect times to 15
    if context is None:
        context = {'status': UNDEFINED}
    while True:
        if count > 15:
            errprint('Redirect too much times')
            return context, 'Error'
        parseURL(url, context)
        status = context['status']
        host = context.get('host', None)
        port = context.get('port', -1)
        filename = context.get('filename', '/')
        if status != SUCCEED:
            errprint("Unexpected error in the URL")
            return context, 'Error'
        response = getContent(host, port, filename)
        if response is None:
            context['status'] = ERROR
            return context, 'Error'
        parseHeader(response, context)
        status = context['status']
        # Not redirect
        if status != REDIRECT:
            return context, response
        # Redirect
        errprint('Redirect to: ' + context['header']['Location'])
        url = context['header']['Location']
        count += 1
        del(context['header'])


def get(urls):
    if urls is None:
        errprint('There is no URL to get')
        return -1
    for url in urls:
        context, response = connect(url)
        status = context['status']
        if context.get('header', None) is not None:
            print(context['header']['status'], context['header']['message'])
        if status == SUCCEED or status == ERROR_BUT_DISPLAY:
            print(response)


def getURLs():
    if len(sys.argv) == 1:
        errprint('Don\'t have url')
        return None
    return [sys.argv[i] for i in range(1, len(sys.argv))]


def main():
    urls = getURLs()
    if urls is None:
        return ERROR
    if get(urls) != SUCCEED:
        return ERROR
    return SUCCEED


if __name__ == '__main__':
    main()

