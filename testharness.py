#!/usr/bin/env python

import asyncio
import sys
import asyncio.subprocess
import time
import os
import random
import re

TIMEOUT = 4

P=None


class Expectation:
    def __init__(self,a,b):
        self.shortname=a
        self.longname=b
        self.shortok=False
        self.longok=False



#os.environ["FOOL_NOW"] = "2023-Sep-08 01:02:04"
expectedDate = os.getenv("FOOL_NOW",None)
deletedFname=None
names=[]

def modifytestsuite():
    global deletedFname
    global names

    with open("testsuite.py","r") as fp:
        py = fp.read()



    toRemove = f"article{random.choice([1,2,3,4,5,6,7])}.txt"
    magicnumber = random.randrange(0,10000)

    i = py.find("#!!")
    assert i != -1
    j = py.find("#!!",i+1)
    assert j != -1
    py = py[:i] + f'REMOVE="{toRemove}"\n' + py[j:]

    with open("testsuite.c","r") as fp:
        c = fp.read()

    c = f'#define REMOVED "{toRemove}"\n#define MAGICNUMBER "{magicnumber}"\n' + c

    with open("testsuite.py","w") as fp:
        fp.write(py)
    with open("testsuite.c","w") as fp:
        fp.write(c)

    return magicnumber

async def getLine(timeLeft):
    line = ""

    while True:

        if timeLeft <= 0:
            if len(line) == 0:
                return None
            else:
                return line

        try:
            c = await asyncio.wait_for(P.stdout.read(1), timeLeft)
        except TimeoutError:
            if len(line) == 0:
                return None
            else:
                return line

        if len(c) == 0:
            return line

        c = c.decode(errors="replace")
        sys.stdout.write(c)
        sys.stdout.flush()

        line += c

        if c == "\n":
            return line

async def terminate():
    if P and P.returncode == None:
        P.stdin.write(b"`"); time.sleep(0.1); P.stdin.write(b"cquit\n")
        await asyncio.wait_for(P.wait(), 1)
        if P.returncode == None:
            P.terminate()
            await asyncio.wait_for(P.wait(), 1)
            if P.returncode == None:
                P.kill()
                await asyncio.wait_for(P.wait())

async def qprompt():
    lookingFor = "\n(qemu)"
    tmp = [""] * len(lookingFor)
    while True:
        c = await P.stdout.read(1)
        c=c.decode(errors="replace")
        tmp.pop(0)
        tmp.append(c)
        if "".join(tmp) == lookingFor:
            return



async def screencap(filename):
    tf = tempfile.NamedTemporaryFile(delete=False)
    tf.close()
    NAME = tf.name.replace("/","//")

    # ~ NAME = "debug"
    try:
        P.stdin.write(b"`"); time.sleep(0.1); P.stdin.write(b"c")
        await qprompt()
        P.stdin.write(f"screendump {NAME}\n".encode())
        await qprompt()
        P.stdin.write(b"`"); time.sleep(0.1); P.stdin.write(b"c")
        w,h,pix = bmputils.loadppm(tf.name)
        bmputils.savebmp(filename,w,h,pix,True)
    finally:
        os.unlink(tf.name)


async def main():
    global P

    with open("testsuite.py","rb") as fp:
        testsuitepy = fp.read()
    with open("testsuite.c","rb") as fp:
        testsuitec = fp.read()

    try:
        ok=False
        magicnumber = modifytestsuite()
        goal = f"All OK! {magicnumber}"

        P = await asyncio.create_subprocess_exec(
                    sys.executable, "make.py",
                    stdin=asyncio.subprocess.PIPE,
                    stdout=asyncio.subprocess.PIPE,
                    limit=65536
        )

        deadline = time.time() + TIMEOUT

        data = []

        while True:
            timeLeft = deadline - time.time()
            line = await getLine(timeLeft)
            if line == None:
                print("Timed out")
                return
            line=line.strip()
            if line == goal:
                ok=True
                break
            data.append(line)

        await terminate()

    finally:
        with open("testsuite.py","wb") as fp:
            fp.write(testsuitepy)
        with open("testsuite.c","wb") as fp:
            fp.write(testsuitec)
        await terminate()


    if ok:
        print("\n\nTest harness says OK!\n\n")
    else:
        print("\n\nNot OK\n\n")


asyncio.run(main())
