#!/usr/bin/env python

import asyncio
import sys
import asyncio.subprocess
import time
import os
import random
import re

TIMEOUT = 10

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

    with open("testsuite.py","w") as fp:
        print("import subprocess",file=fp)
        print("def copy(python,imgfile):",file=fp)
        print('    subprocess.check_call([python, "fool.pyz", imgfile,',file=fp)

        toDelete = random.randrange(1,7)
        deletedFname = f"ARTICLE{toDelete}.TXT"

        for i,n in enumerate([7,1,2,3,4,5,6]):
            fname = "article{}.txt".format(n)
            mixedcasefname = fname[:i] + fname[i].upper()+fname[i+1:]
            print('"cp","{}","{}",'.format(fname,mixedcasefname), file=fp)
            if i != toDelete:
                names.append( (fname.upper(),mixedcasefname) )
        print('"cp","const.txt","cOnst.txt",', file=fp)
        names.append( ("CONST.TXT","cOnst.txt") )
        print('"cp","article2.txt","my fancy filename.txt",',file=fp)
        names.append( ("MYFANC~1.TXT","my fancy filename.txt" ) )
        print('"cp","article3.txt","this is a really long filename.c",',file=fp)
        names.append( ("THISIS~1.C","this is a really long filename.c") )
        print('"cp","billofrights.txt","BILL.TXT",',file=fp)
        names.append( ("BILL.TXT","BILL.TXT") )
        print('"cp","billofrights.txt","BILL.XA",',file=fp)
        names.append( ("BILL.XA","BILL.XA") )

        now=time.time()
        t=now-int(now)
        t *= 1000
        t = int(t)
        t = "%03d" % t
        n = "A"+t+".TXT"
        print('"cp","billofrights.txt","{}",'.format(n),file=fp )
        names.append( (n,n) )

        name2L = "a" + t + "c def ghij klmn opq.txt"
        name2S = "A" + t + "CD~1.TXT"
        print('"cp","billofrights.txt","{}",'.format(name2L),file=fp )
        names.append( ( name2S, name2L ) )

        print(f'"rm","{deletedFname}"',file=fp )

        print("])",file=fp)


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
        testsuite = fp.read()

    try:
        modifytestsuite()

        P = await asyncio.create_subprocess_exec(
                    sys.executable, "make.py",
                    stdin=asyncio.subprocess.PIPE,
                    stdout=asyncio.subprocess.PIPE,
                    limit=65536
        )


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

    finally:
        with open("testsuite.py","wb") as fp:
            fp.write(testsuite)
        await terminate()

    doCheck(data)


def doCheck(lines):
    global expectedDate
    global names

    now = time.mktime(time.localtime(time.time()))

    expected=[]
    for s,l in names:
        expected.append( Expectation( s,l ) )


    gotTimes = True;
    timerex = re.compile(r"(\d{4})-(\d{2})-(\d{2})\s+(\d{1,2}):(\d{2}):(\d{2})");

    if expectedDate == None:
        expectedDate = now

    else:
        expectedDate = time.mktime(time.strptime(expectedDate,"%Y-%b-%d %H:%M:%S"))

    shortnamesok=True
    longnamesok=True
    datesok=True

    for exp in expected:
        shortname = exp.shortname
        longname = exp.longname

        foundShort=False
        foundLong=False
        foundDate=False
        for line in lines:
            if longname in line:
                found=longname
                i = line.find(longname)
                after = line[i+len(longname):]
                break
            if shortname in line:
                found=shortname
                i = line.find(shortname)
                after = line[i+len(shortname):]
                break
        else:
            print("Did not find",shortname,"or",longname,"in output")
            return

        if len(after) > 0 and not after[0].isspace():
            print("Found name",found,"but it had trailing garbage after it")
            return

        if longnamesok:
            #after the first failure, don't try long names again
            if not longname in line:
                print("Missing long name",longname)
                longnamesok=False

        if datesok:
            #after one failure, don't try any more
            M = timerex.search(line)
            if M:
                year = int(M.group(1))
                month = int(M.group(2))
                day = int(M.group(3))
                hour = int(M.group(4))
                minute = int(M.group(5))
                second = int(M.group(6))
                T = time.mktime((
                    year,
                    month,
                    day,
                    hour,
                    minute,
                    second,
                    0,
                    0,
                    -1
                ))
                timeDiff = abs( expectedDate - T )
                if(timeDiff > 4):
                    datesok = False
                    print("Bad time on line " + line)
            else:
                datesok=False
                print("No creation time on line " + line)

    for line in lines:
        if line.find(deletedFname[1:]) != -1:
            print("Deleted file (",deletedFname,") appears in output.")
            return

    print()
    print()
    print()

    if expectedDate == now:
        print("Note: Used current date/time for checks")
    else:
        print("Note: Using forged date/time")

    print()
    print()
    print()

    score = 100
    print(    "Basic lab                     +100%")

    if(longnamesok):
        print("Long name bonus:              + 75%")
        score += 75
    else:
        print("No long name bonus:           +  0%")

    if(datesok) :
        print("Creation time bonus:          + 25%")
        score += 25
    else:
        print("No creation time bonus:       +  0%")

    print("Total score:                  ",str(score)+"%")




asyncio.run(main())
