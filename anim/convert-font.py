#!/bin/env python3

from PIL import Image

print("const uint8_t font[] = {")

im = Image.open("font.png")
w,h = im.size
ch=8 # character height
cs=11 # character spacing

pixels = [p[0] for p in list(im.getdata())]

for c in range(95):
	b = [0xFF,0xFF,0xFF,0xFF,0xFF]
	for x in range(w):
		for y in range(ch):
			p = pixels[ y*w + c*w*cs +w +x ]
			if p<128:
				b[ x ] &= ~(1<<y)

	for y in range(w):
		print(f"0x{b[y]:02X}",end=",")

	print(" /* ",chr(c+ord(" ")), " */")

print("};")
