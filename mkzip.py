#!/usr/bin/python3

import zipfile
import os
import os.path
try:
    import tkinter.messagebox
    import tkinter
    hasTk=True
except ImportError:
    hasTk=False

def message(txt):
    if hasTk:
        r = tkinter.Tk()
        r.withdraw()
        tkinter.messagebox.showinfo(title="mkzip",
            message=txt)
    else:
        print(txt)
        input("Press enter...")

def main():

    try:

        directory = os.path.dirname( os.path.abspath(__file__) )
        dlen = len(directory)

        zipname = os.path.join(directory,"lab.zip")

        with zipfile.ZipFile(zipname,mode="w",compression=zipfile.ZIP_DEFLATED) as zfp:
            for dirpath,dirs,files in os.walk(directory,followlinks=True):
                i=0
                for f in files:
                    fl = f.lower()
                    if fl.endswith(".c") or fl.endswith(".cpp") or fl.endswith(".h") or fl.endswith(".py") or fl.endswith(".ini") or fl.endswith(".txt"):
                        tmp = f.split(".")
                        fullpath=os.path.join(dirpath,f)
                        abbrpath = fullpath[dlen+1:]
                        print(fullpath,"->",abbrpath)
                        zfp.write( fullpath,abbrpath)
        message("Done. Created {}".format(zipname) )

    except Exception as e:
        message( "{}".format(e) )
        raise


if __name__ == "__main__":
    main()
