import lzo
import sys
import struct

fc = open(sys.argv[1],'rb')
cd = fc.read()
fc.close()
d_len , c_len ,ecrc32= struct.unpack("III",cd[24:36])
print("Compressed data size: %x"%c_len)
print("Decompressed data size: %x"%d_len)

dd = lzo.decompress(cd[36:c_len+36],False,d_len)

print("Actual decompressed len: %x"%len(dd))
ccrc32 = lzo.crc32(dd)
print("Expected CRC32: %x"%ecrc32)
print("Computed CRC32: %x"%ccrc32)
print("CRC32 OK? ",ecrc32==ccrc32)

fd = open(sys.argv[1]+'.elf','wb')

fmagic = struct.unpack("I",dd[:4])[0]
if fmagic == 0x464c457f: #elf
	fd.write(dd) 
elif fmagic == 0xb8b2bb81: #xored elf
	for b in dd:
		fd.write(bytes([b^0xfe]))
else:
	print("Error: unknown magic 0x%x"%fmagic)
fd.close()
print("Saved as %s.elf"%sys.argv[1])