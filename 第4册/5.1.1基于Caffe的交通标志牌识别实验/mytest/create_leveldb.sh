#!/bin/sh

#如果您使用了不同的文件目录，请对IMGDIR进行相应修改
IMGDIR=examples/mytest/classification_data

#数据使用格式，如果使用lmdb格式，修改成BACKEND="lmdb"
BACKEND="leveldb"

#图像转换命令，将训练数据resize并转成BACKEND类型数据
build/tools/convert_imageset --shuffle \
--resize_height=227 --resize_width=227 \
$IMGDIR/ $IMGDIR/train_label.txt  $IMGDIR/train_leveldb --backend=${BACKEND}

#转换测试集数据
build/tools/convert_imageset --shuffle \
--resize_height=227 --resize_width=227 \
$IMGDIR/ $IMGDIR/val_label.txt  $IMGDIR/val_leveldb  --backend=${BACKEND}

echo "finish!"
