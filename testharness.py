#!/usr/bin/env python

import asyncio
import sys
import asyncio.subprocess
import time
import os

TIMEOUT = 20

P=None

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

    P = await asyncio.create_subprocess_exec(
                sys.executable, "make.py",
                stdin=asyncio.subprocess.PIPE,
                stdout=asyncio.subprocess.PIPE,
                limit=65536
    )
    try:
        data = ""
        deadline = time.time() + TIMEOUT

        while True:
            timeLeft = deadline - time.time()
            line = await getLine(timeLeft)
            if line == None:
                print("Timed out")
                return
            line=line.strip()
            if line == "START":
                break

        data = []

        while True:
            timeLeft = deadline - time.time()
            line = await getLine(timeLeft)
            if line == None:
                print("Timed out")
                return
            line=line.strip()
            if line == "DONE":
                break
            data.append(line)

        await terminate()

        data = "".join(data)

        lookFor = [
            "KERNEL  EXE",
            "ARTICLE1TXT",
            "ARTICLE2TXT",
            "ARTICLE3TXT",
            "ARTICLE4TXT",
            "ARTICLE5TXT",
            "ARTICLE6TXT"
        ]
        for x in lookFor:
            if x not in lookFor:
                print("Failed to find what we were looking for:",x)

        print("OK")

    finally:
        await terminate()




asyncio.run(main())
