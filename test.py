#!/usr/bin/env python

# http server concurrency test script

import sys
import threading
import httplib

REMOTE_SERVER_HOST = '192.168.1.17:8899'
REMOTE_SERVER_PATH = '/'

class HTTPClient:
    def __init__(self, host):
        self.host = host

    def fetch(self, path):
        http = httplib.HTTP(self.host)
        # Prepare header
        http.putrequest("GET", path)
        http.putheader("User-Agent", __file__)
        http.putheader("Host", self.host)
        http.putheader("Accept", "*/*")
        http.endheaders()
        try:
            errcode, errmsg, headers = http.getreply()
        except Exception, e:
            print "Client failed error code: %s message:%s headers:%s" %(errcode,
                    errmsg, headers)
        else:
            #print "Got homepage from %s" %self.host
            file = http.getfile()
            return file.read()

def doget(id, host, path, cycle, lock):
    #lock.acquire()
    #print "thread %d start!\n"%id
    #lock.release()
    for i in range(cycle):
        client = HTTPClient(host)
        rsp = client.fetch(path)
        lock.acquire()
        print "%d : %s" % (id, rsp)
        lock.release()
    return 

if __name__ == "__main__":
    if len(sys.argv)<4:
        print "usage: %s <host> <path> <threanum> <cycle/thread>"%sys.argv[0]
        print "eg: %s 192.168.1.17:8899 /ping 10 2"%sys.argv[0]
        exit(0)

    host=sys.argv[1]
    path=sys.argv[2]
    threanum=int(sys.argv[3])
    cycle=int(sys.argv[4])

    threads=[]
    lock=threading.Lock()
    for i in range(threanum):
        t = threading.Thread(target=doget, args=(i, host, path, cycle, lock))
        threads.append(t)

    for i in range(threanum):
        threads[i].start()

    for i in range(threanum):
        threads[i].join()

