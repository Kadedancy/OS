import ctypes
import zipfile

class BitmapHeader(ctypes.Structure):
    _pack_ = 1
    _fields_ = [
        ( "sig"                 ,  ctypes.c_char*2 ), #"BM"
        ( "size"                ,  ctypes.c_uint32 ), #size of whole bitmap
        ( "dummy"               ,  ctypes.c_uint32 ),
        ( "offset_to_data"      ,  ctypes.c_uint32 ), #offset from file start to data
        ( "header_size"         ,  ctypes.c_uint32 ), #sizeof(header)-14: num in rest of header
        ( "width"               ,  ctypes.c_uint32 ),
        ( "height"              ,  ctypes.c_uint32 ),
        ( "planes"              ,  ctypes.c_uint16 ), #1
        ( "bits_per_pixel"      ,  ctypes.c_uint16 ), #24=true color
        ( "compression"         ,  ctypes.c_uint32 ), #0=none
        ( "img_size"            ,  ctypes.c_uint32 ), #Number of bytes in image
        ( "pixels_per_meter_x"  ,  ctypes.c_uint32 ), #Most any value OK
        ( "pixels_per_meter_y"  ,  ctypes.c_uint32 ),
        ( "num_colors"          ,  ctypes.c_uint32 ), #0=truecolor
        ( "important_colors"    ,  ctypes.c_uint32 )  #0=truecolor
    ]


def loadppm(filename):
    i=0

    def token():
        nonlocal fp,i

        #discard leading whitespace
        while True:
            c = fp.read(1)
            if not c.isspace():
                break

        #accumulate non-whitespace characters
        t = c.decode()
        while True:
            c = fp.read(1)
            if c.isspace():
                break
            t += c.decode()

        #consume whitespace
        while True:
            c = fp.read(1)
            if not c.isspace():
                break
        fp.seek(-1,1)
        return t

    with open(filename,"rb") as fp:
        magic = token()
        assert magic == "P6",f"Expected magic number P6; got {magic}"
        width=int(token())
        height=int(token())
        ccv=token()
        assert ccv == "255"
        pixels = fp.read(width*height*3)
        if len(pixels) != width*height*3:
            print(f"For {width}x{height} pixels: Got {len(pixels)} bytes")
            assert 0

    return (width,height,pixels)

def savebmp(filename,w,h,pixels, flip):

    assert w % 4 == 0

    tmp = bytearray(ctypes.sizeof(BitmapHeader))
    H = BitmapHeader.from_buffer(tmp)
    tmp[0]=ord("B")
    tmp[1]=ord("M")
    H.size = w*h*3+ctypes.sizeof(H)
    H.offset_to_data = ctypes.sizeof(H)
    H.header_size = ctypes.sizeof(H)-14
    H.width = w
    H.height = h
    H.planes = 1
    H.bits_per_pixel = 24
    H.compression = 0
    H.img_size = w*h*3
    H.pixels_per_meter_x = 72
    H.pixels_per_meter_y = 72
    H.num_colors=0
    H.important_colors = 0

    with open(filename,"wb") as fp:
        fp.write(tmp)
        pitch = w*3
        if flip:
            idx = len(pixels)-pitch
            delta = -pitch
        else:
            idx=0
            delta = pitch

        for y in range(h):
            row = pixels[idx:idx+pitch]
            for i in range(0,len(row),3):
                fp.write(bytes((
                    row[i+2],
                    row[i+1],
                    row[i+0]
                )))
            idx += delta

def unzip(filename):
    assert filename.endswith(".zip")
    filename2 = filename[:-4]
    zfp = zipfile.ZipFile(filename)
    with zfp.open(filename2) as ifp:
        with open(filename2,"wb") as ofp:
            while True:
                data = ifp.read(4096)
                if not data:
                    break
                ofp.write(data)

def loadbmp(filename):
    with open(filename,"rb") as fp:
        tmp = bytearray(fp.read(ctypes.sizeof(BitmapHeader)))
        H = BitmapHeader.from_buffer(tmp)
        assert H.sig[0] == ord("B")
        assert H.sig[1] == ord("M")
        assert H.width % 4 == 0
        fp.seek(H.offset_to_data)
        pixels = fp.read(H.width*H.height*3)
        return (H.width,H.height,pixels)

def compare(bmp1, bmp2, deltafile):
    w1,h1,pix1 = loadbmp(bmp1)
    w2,h2,pix2 = loadbmp(bmp2)
    if w1 != w2 or h1 != h2:
        print(f"Sizes differ: {w1}x{h1} versus {w2}x{h2}")
        return False

    assert len(pix1) == len(pix2),f"Lengths differ: {len(pix1)} {len(pix2)}"

    diff = bytearray(w1*h1*3)
    match=True

    #do in chunks to make faster
    NUMPIX=1
    NUMBYTES=NUMPIX*3
    for i in range(0,len(pix1),NUMBYTES):
        c1 = pix1[i:i+NUMBYTES]
        c2 = pix2[i:i+NUMBYTES]
        if c1 == c2:
            pass
        else:
            match=False
            for j in range(i,i+NUMBYTES,3):
                r1 = pix1[j]
                g1 = pix1[j+1]
                b1 = pix1[j+2]
                r2 = pix2[j]
                g2 = pix2[j+1]
                b2 = pix2[j+2]

                if r1 != r2 or g1 != g2 or b1 != b2:
                    diff[j] = 255
                    diff[j+1] = 255
                    diff[j+2] = 255

    savebmp(deltafile, w1,h1,diff, False )
    return match
