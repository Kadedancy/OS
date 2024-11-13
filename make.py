import configparser
import subprocess
import os
import sys

inifile=configparser.ConfigParser(interpolation=None)
inifile.read("config.ini")
conf=inifile["config"]

cc=conf["compiler"]
link=conf["linker"]
qemu=conf["qemu"]
python=conf["python"]
userlinker=conf["userlinker"]
ar=conf["ar"]

cflags=[
    "-target", "i686-pc-win32", #target platform=Windows 32 bit
    "-m32",                     #force 32 bit
    "-Wall",                    #all warnings
    "-Werror",                  #make warnings into errors
    "-fno-builtin",             #don't use any builtin functions
    "-ffreestanding",           #don't use any libraries
    "-mno-sse",                 #don't use cpu's sse registers
    "-nostdinc",                #no standard library includes
    "-nostdlib",                #no standard library functions
    "-I.",                      #use current directory for includes
    "-c"                        #compile files
]

linkflags=[
    "/base:0x100000",       #where kernel gets loaded
    "/machine:x86",         #32 bit
    "/nodefaultlib",        #no standard libraries
    "/out:kernel.exe",      #output filename
    "/subsystem:console",   #console app
    "/fixed",               #not relocatable
    "/entry:start",         #function to be called on startup
    "/map:kernel.syms"      #kernel symbols
]

userlinkflags =[
    "/base:0x400000",       #where file gets loaded
    "/machine:x86",         #32 bit
    "/nodefaultlib",        #no standard libraries
    "/subsystem:console",   #console app
    "/fixed",               #not relocatable
    "/entry:main",          #function to be called on startup
]

def run(*args):
    print(args)
    subprocess.check_call(*args)
    

objectfiles=[]
for filename in os.listdir("."):
    if filename.endswith(".c"):
        obj=filename+".o"
        run(
            [cc] + cflags + ["-o",obj,filename]
        )
        objectfiles.append(obj)

run( [link] + linkflags + objectfiles )

for filename in os.listdir("user"):
    if filename.endswith(".c"):
        filename = os.path.join("user",filename)
        obj=filename+".o"
        exe=filename.replace(".c",".exe")
        run( [cc] + cflags + ["-o", obj, filename] )
        run( [link] + userlinkflags + ["/out:"+exe , obj] )

run( [
    python, "fool.pyz", "hd.img",
        "create", "64",
        "cp", "kernel.exe", "KERNEL.EXE",
        "cp", "user/hello.exe", "HELLO.EXE"
])

temp = [ qemu,

    #virtual hard drive
    "-drive","format=raw,media=disk,file=hd.img,id=disk0",

    #redirect serial port to window
    "-serial","mon:stdio",

    #escape character for controlling qemu
    "-echr","96"
]
print(" ".join(temp))
run(temp)