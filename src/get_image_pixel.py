# usage: python3 get_image_pixel.py image_flie_name
from PIL import Image
import sys
import struct

filename = sys.argv[1]

img = Image.open(filename).resize((96,112))

w,h= img.size
print(w,h)
# new_pic = Image.new("RGB",(w,h))
f = open(filename+'.bin','wb')

f.write(struct.pack('i',int(w)))
f.write(struct.pack('i',int(h)))

for i in range(w):
	for j in range(h):
		pixel_r = int(img.getpixel((i,j))[0])			
		f.write(struct.pack('i',pixel_r))

for i in range(w):
	for j in range(h):
		pixel_g = int(img.getpixel((i,j))[1])
		f.write(struct.pack('i',pixel_g))

for i in range(w):
	for j in range(h):
		pixel_b = int(img.getpixel((i,j))[2])
		f.write(struct.pack('i',pixel_b))

img.close()
f.close()
		# new_pic.putpixel((i,j),(int(pixel_r),int(pixel_g),int(pixel_b)))
# new_pic.show()