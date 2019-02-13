# usage: python3 compare.py 1.txt 2.txt
import sys
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
	count = 0
	for i in range(len(v1)):
		if(abs(v1[i] - v2[i]) < 0.01):
			count += 1
		else:
			print(i+1)
			print(v1[i],v2[i])
	return count

if __name__ == '__main__':
	v1 = open_file(sys.argv[1])
	num = cal_distance(v1,sys.argv[2])
	print(num/len(v1),len(v1)-num)