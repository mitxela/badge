#!/bin/env python3

import sys, os
from PIL import Image

if len(sys.argv) != 2:
	print("Usage: ",sys.argv[0],"dir")
	exit()

maxes = []
gamma = 2
scale = 233**gamma

def image_to_bytes(i):
	im = Image.open(i)
	w,h = im.size
	pixels = [p[0] for p in list(im.getdata())]
	maxes.append(max(pixels))
	print("")

	b = []
	for y in range(h):
		b.append( [0xFF,0xFF,0xFF,0xFF] )
		for x in range(w):
			#p = 255-int(pixels[y*8+x]*scale)
			p = pixels[y*8+x]**gamma
			if p > scale*4/5:
				b[y][3] &= ~(1<<x)
			elif p > scale*3/5:
				b[y][2] &= ~(1<<x)
			elif p > scale*2/5:
				b[y][1] &= ~(1<<x)
			elif p > scale*1/5:
				b[y][0] &= ~(1<<x)

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
