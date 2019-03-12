# This code is use the rgb data to creat an image file and show the image.
import sys
from PIL import Image

def open_file(file_name):
	f1 = open(file_name,'r')
	v1 = []
	while(True):
		line = f1.readline().strip().split()
		if not line:
			break
		v1.append(float(line[0]))
	f1.close()
	return v1

if __name__ == '__main__':
	v1 = open_file(sys.argv[1])
	new_pic = Image.new("RGB",(96,112))
	count = 0
	for i in range(96):
		for j in range(112):
			new_pic.putpixel((i,j),(int(v1[count]),int(v1[count+1]),int(v1[count+2])))
			count += 3

	new_pic.show()
