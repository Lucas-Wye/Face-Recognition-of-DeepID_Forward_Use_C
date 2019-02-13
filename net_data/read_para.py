# -*- coding: UTF-8 -*-
# deploy文件
import caffe
import struct
MODEL_FILE = 'Deploy.prototxt'
# 预先训练好的caffe模型
PRETRAIN_FILE = 'DeepID_solver_iter_240000.caffemodel'

# 让caffe以测试模式读取网络参数
net = caffe.Net(MODEL_FILE, PRETRAIN_FILE, caffe.TEST)

'''
(N, K, H, W)
N: 数据的个数，例如SGD时一次mini-batch的图像个数。
K: 如果是图像，可以理解为通道数量；如果是网络中间结果，就是feature map的数量。
H, W： 如果是图像数据，可以理解为图像的高度和宽度；如果是参数数据，可以理解为滤波核的高度和宽度。
'''


# w(20,3,4,4)
weight = net.params['conv1'][0].data
bias = net.params['conv1'][1].data
f = open('conv1.bin','wb')
for i in range(20):
     for j in range(3):
             for m in range(4):
                     for n in range(4):
                             f.write(struct.pack('f',weight[i][j][m][n]))
for i in range(40):
    f.write(struct.pack('f',bias[i]))
f.close()


# w(40,20,3,3)
weight = net.params['conv2'][0].data
bias = net.params['conv2'][1].data
f = open('conv2.bin','wb')
for i in range(40):
     for j in range(20):
             for m in range(3):
                     for n in range(3):
                             f.write(struct.pack('f',weight[i][j][m][n]))
for i in range(40):
    f.write(struct.pack('f',bias[i]))
f.close()

# w(60,40,3,3)
weight = net.params['conv3'][0].data
bias = net.params['conv3'][1].data
f = open('conv3.bin','wb')
for i in range(60):
     for j in range(40):
             for m in range(3):
                     for n in range(3):
                             f.write(struct.pack('f',weight[i][j][m][n]))
for i in range(60):
    f.write(struct.pack('f',bias[i]))
f.close()

# w(80,60,2,2)
weight = net.params['conv4'][0].data
bias = net.params['conv4'][1].data
f = open('conv4.bin','wb')
for i in range(80):
     for j in range(60):
             for m in range(2):
                     for n in range(2):
                             f.write(struct.pack('f',weight[i][j][m][n]))
for i in range(80):
    f.write(struct.pack('f',bias[i]))
f.close()

# w(160,31500)
weight = net.params['fc160_1'][0].data
bias = net.params['fc160_1'][1].data
f = open('fc160_1.bin','wb')
for i in range(160):
    for j in range(31500):             
        f.write(struct.pack('f',weight[i][j]))
for i in range(160):
    f.write(struct.pack('f',bias[i]))
f.close()


# w(160,38400)
weight = net.params['fc160_2'][0].data
bias = net.params['fc160_2'][1].data
f = open('fc160_2.bin','wb')
for i in range(160):
    for j in range(38400):             
        f.write(struct.pack('f',weight[i][j]))
for i in range(160):
    f.write(struct.pack('f',bias[i]))
f.close()