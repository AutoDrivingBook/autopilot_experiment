#-*- coding:UTF-8 -*-
import sys
import os
import caffe
import numpy as np
import pdb
import pandas as pd
#用于检测模型准确率的caffe配置文件，请注意是否要修改目录
model_def = "/root/caffe/examples/mytest/test_cls/deploy.prototxt"
#检测前训练好的模型文件
model_weights = "/root/caffe/train_iter_2500.caffemodel"

net = caffe.Net(model_def,     
                model_weights,
                caffe.TEST)
#转换均值函数
def convert_mean(binMean,npyMean):
    blob = caffe.proto.caffe_pb2.BlobProto()
    bin_mean = open(binMean, 'rb' ).read()
    blob.ParseFromString(bin_mean)
    arr = np.array( caffe.io.blobproto_to_array(blob) )
    npy_mean = arr[0]
    np.save(npyMean, npy_mean )
#均值文件路径
binMean='/root/caffe/examples/mytest/test_cls/mean.binaryproto' 
npyMean='/root/caffe/examples/mytest/test_cls/mean.npy'
convert_mean(binMean,npyMean)#图片均值转换

#输入图片数据预处理部分

transformer = caffe.io.Transformer({'data': net.blobs['data'].data.shape})
transformer.set_transpose('data', (2,0,1)) 
transformer.set_mean('data', np.load(npyMean).mean(1).mean(1))#图片去均值
transformer.set_raw_scale('data', 255)  #将图片由[0, 1]缩放到[0, 255]区间
transformer.set_channel_swap('data', (2, 1, 0))  #将图片通道由 RGB 变为 BGR
#加载输出的检测文件，用于写入预测结果
file = open('/root/caffe/examples/mytest/test_cls/save_predict_label.txt','w')
#检测图片的文件夹目录
in_dir = "/root/caffe/examples/mytest/classification_data/021/"
#收集符合格式的图片文件名
img_list = [f for f in os.listdir(in_dir) if (f[-4:] == ".jpg" or f[-5:]==".jpeg" or f[-4:]==".png")]
for line in img_list:
    list_name= in_dir + line.rstrip()#图片包的绝对路径
    image = caffe.io.load_image(list_name)
    #图片数据转换
    transformed_image = transformer.preprocess('data', image)
    net.blobs['data'].data[...] = transformed_image
    net.blobs['data'].reshape(1, 3, 227, 227)
    output = net.forward()
    #导入标签数据
    labels_filename = "/root/caffe/examples/mytest/test_cls/labels.txt"
    labels = pd.read_table(labels_filename).values
    prob = net.blobs['prob'].data[0].flatten()
    order = prob.argsort()[-1]
    #检测后得到的分类标签
    class_predict_label = int(str(labels[order-1][0]).split("-")[0])
    #写入输出文件
    file.write(line.rstrip() +" "+str(class_predict_label)+"\n")
file.close()
