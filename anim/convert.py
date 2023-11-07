#!/bin/env python3

import sys, os
from PIL import Image

if len(sys.argv) != 2:
	print("Usage: ",sys.argv[0],"dir")
	exit()

maxes = []

def image_to_bytes(i):
	im = Image.open(i)
	w,h = im.size
	pixels = [p[0] for p in list(im.getdata())]
	maxes.append(max(pixels))
	print("")

	b = []
	for y in range(h):
		b.append( [0,0,0,0] )
		for x in range(w):
			p = 255-pixels[y*8+x]
			p4 = (p>>7)&1
			p3 = (p>>6)&1
			p2 = (p>>5)&1
			p1 = (p>>4)&1
			#print(p4,p3,p2,p1,end=", ")
			b[y][0] |= p1<<x
			b[y][1] |= p2<<x
			b[y][2] |= p3<<x
			b[y][3] |= p4<<x
		#print(list(map(bin, b)))
	for s in range(4):
		for y in range(h):
			print(f"0x{b[y][s]:02X}",end=",")
		print("")

files = [os.path.join( sys.argv[1], f) for f in os.listdir( sys.argv[1] )]
files.sort()


print("const uint8_t frames[]= {")
for f in files:
	image_to_bytes(f)

print("};")

#print(f"// Max:0x{max(maxes):02X}")
print(f"Max:{max(maxes)}",file=sys.stderr)
