# minimal RAR5 store-mode multi-volume writer, only for testing the unpacker
import sys, zlib, struct, os
def vint(n):
    out = bytearray()
    while True:
        b = n & 0x7f; n >>= 7
        if n: out.append(b | 0x80)
        else: out.append(b); return bytes(out)
def block(htype, hflags, body, datasize=None):
    h = vint(htype) + vint(hflags)
    if datasize is not None: h += vint(datasize)
    h += body
    h = vint(len(h)) + h
    return struct.pack('<I', zlib.crc32(h) & 0xffffffff) + h
src, outbase, volsize, width = sys.argv[1], sys.argv[2], int(sys.argv[3]), int(sys.argv[4])
data = open(src, 'rb').read(); name = os.path.basename(src).encode()
parts = [data[i:i+volsize] for i in range(0, len(data), volsize)]
for i, p in enumerate(parts):
    first, last = i == 0, i == len(parts) - 1
    out = b'Rar!\x1a\x07\x01\x00'
    out += block(1, 0, vint(1 | (0 if first else 2)) + (b'' if first else vint(i)))
    crc = zlib.crc32(data if last else p) & 0xffffffff
    hf = 2 | (0 if first else 8) | (0 if last else 0x10)
    body = vint(4) + vint(len(data)) + vint(0x20) + struct.pack('<I', crc) + vint(0) + vint(0) + vint(len(name)) + name
    out += block(2, hf, body, len(p)) + p
    out += block(5, 0, vint(0 if last else 1))
    open('%s.part%0*d.rar' % (outbase, width, i + 1), 'wb').write(out)
