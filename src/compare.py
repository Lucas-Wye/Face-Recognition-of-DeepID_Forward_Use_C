# usage: python3 compare.py 1.txt 2.txt
import sys
from numpy import linalg
threshold = 0.2
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

def cal_distance(v1,feature_file):
	v2 = open_file(feature_file)
	numerator = 0
	denominador = 0
	for i in range(160):
		numerator += v1[i] * v2[i]
	denominador = linalg.norm(v1) * linalg.norm(v2)
	cos = numerator / denominador
	return cos

if __name__ == '__main__':
	v1 = open_file(sys.argv[1])
	cos1 = cal_distance(v1,sys.argv[2])
	print(cos1)
	if 	cos1 > threshold:
		print('The two picture are from the same person.')
	else:
		print('The two picture aren\'t from the same person.')