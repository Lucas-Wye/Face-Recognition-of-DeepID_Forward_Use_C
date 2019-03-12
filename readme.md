#### Download 
	https://github.com/Lustrew/Face-Recognition-of-DeepID_Forward_Use_C
#### License
	GPLv3
#### Summary
*本项目为用C语言实现的DeepID卷积神经网络的前向传播。目的是要把人脸识别在板子上实现，但是主流的caffe和tensorflow都无法在板子上安装，因此写了这份代码，用C语言实现了前向传播，开发环境为gcc和python3，C语言环境不需要任何其他的依赖包，因此可以在机器上直接编译运行。通过将图片输入到前向传播网络中，得到最后一层输出的特征向量。输入多张图片进行特征提取，再对特征向量进行比较，计算余弦夹角距离，可以判断两张图片的人是否为同一个。其中的caffemodel为训练好的人脸分类器，经验证，本项目对人脸的识别率为90%*
#### Environment
	GCC, python3
#### Introduction
#### (1)usage 
	cd net_data && python3 read_para.py	
	cd .. 
	make
	sh run image_1_path image_2_path

#### (2)./net_data: 
	存放网络各层参数的二进制文件
##### read_para.py:
	获得网络各层参数并保存到二进制文件中
	python3 read_para.py
#### (3)./pic_rgb:
	存放图片和图片的二进制文件
#### (4)./src:
	存放源代码
##### get_image_pixel:
	获得图片的RGB参数并保存到二进制文件中
	python3 get_image_pixel.py image_flie_name	
##### compare.py:
	对进行前向传播后的fc160进行比较 
	python3 compare.py 1.txt 2.txt
##### transform:
	对图片大小进行调整
##### JpegCodecs：
	解码jpg
##### \*.c:
	c语言实现DeepID网络的前向传播

#### (5)network parameters
网络结构如下图：
![](net_data/DeepID.png)  
	
	r -> width 96
	c -> height 112
	w[r][c]

	data(1,3,112,96)-> conv1(20,3,4,4)(1,20,109,93) -> relu1 -> 
	pool1(1,20,55,47)-> conv2(40,20,3,3)(1,40,53,45) -> relu2 -> pool2(1,40,52,44)-> conv3(60,40,3,3)(1,60,50,42) -> relu3 -> pool3(1,60,25,21) = 31500
	| -> fc160_1(160,31500)(1,160)
	| -> conv4(80,60,2,2)(1,80,24,20) -> relu4-> fc160_2(160,38400)(1,160)
	-> fc160(1,160)

	20*3*4*4+20+40*20*3*3+40+60*40*3*3+60+160*31500+160+80*60*2*2+80+160*38400+160 = 11233480


##### 4.1.1 conv1
	w(20,3,4,4)
	kernel_num:20
	kernel_size:4*4
	stride:1

	b(20)


##### 4.1.2 relu1

##### 4.1.3 pool1
	MAX
	kernel_size:2
	stride:2



##### 4.2.1 conv2
	w(40,20,3,3)
	kernel_num:40
	kernel_size:3*3
	stride:1

	b(40)


##### 4.2.2 relu2

##### 4.2.3 pool2
	MAX
	kernel_size:2
	stride:1

##### 4.3.1 conv3
	w(60,40,3,3)
	kernel_num:60
	kernel_size:3*3
	stride:1

	b(60)

##### 4.3.2 relu3

##### 4.3.3 pool3
	MAX
	kernel_size:2
	stride:2

##### 4.4.1 conv4
	w(80,60,2,2)
	kernel_num:80
	kernel_size:2*2
	stride:1

	b(80)

##### 4.4.2 relu4

##### 4.5 fc160_1
	w(160,31500)
	b(160)

##### 4.6 fc160_2
	w(160,38400)
	b(160)

##### 4.7 fc160
	fc160_1 + fc160_2
