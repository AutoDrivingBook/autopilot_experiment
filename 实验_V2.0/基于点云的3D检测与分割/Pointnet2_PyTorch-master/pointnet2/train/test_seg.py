import torch
import torch.optim as optim
import torch.optim.lr_scheduler as lr_sched
import torch.nn as nn
from torch.utils.data import DataLoader
from torch.utils.data.sampler import SubsetRandomSampler
from torch.autograd import Variable
import numpy as np
import os
import tqdm
import sys
sys.path.append('/home1/xuhui/Pointnet2_PyTorch-master/')
from pointnet2.models import Pointnet2SemMSG as Pointnet
from pointnet2.models.pointnet2_msg_sem import model_fn_decorator
from pointnet2.data import Indoor3DSemSeg
import pointnet2.utils.pytorch_utils as pt_utils
import argparse
preds_path = '/home1/xuhui/Pointnet2_PyTorch-master/pointnet2/data/3d-sample/preds1/'
parser = argparse.ArgumentParser(description="Arg parser")
parser.add_argument("-batch_size", type=int, default=1, help="Batch size [default: 32]")
parser.add_argument("-num_points",type=int,default=90112,help="Number of points to train with [default: 2048]")
parser.add_argument('--gpu', default='0', help='GPU to use [default: GPU 0]')

paras = parser.parse_args()
gpu_index = paras.gpu
os.environ["CUDA_VISIBLE_DEVICES"] = gpu_index 
lr_clip = 1e-5
bnm_clip = 1e-2


if __name__ == "__main__":
    args = parser.parse_args()

    test_set = Indoor3DSemSeg(args.num_points, train=False)
    test_loader = DataLoader(test_set,batch_size=args.batch_size,shuffle=False,pin_memory=True,num_workers=2)

    model = Pointnet(3, input_channels=6).cuda()
    model.load_state_dict(torch.load('/home1/xuhui/Pointnet2_PyTorch-master/pointnet2/train/checkpoints/poitnet2_semseg_best.pth.tar')['model_state'])
    model.eval()

    # for i, data in tqdm.tqdm(enumerate(test_loader, 0), total=len(test_loader),
    #                              leave=False, desc='val'):
    for i,data in enumerate(test_loader):
        f = open(os.path.join(preds_path, str(i).zfill(2)+'.txt'),'w')
        inputs, labels = data 
        inputs = inputs.to('cuda', non_blocking=True)
        labels = labels.to('cuda', non_blocking=True)
        # print(labels)
        preds = model(inputs)
        _, classes = torch.max(preds, -1)

        acc = (classes == labels).float().sum() / labels.numel()
        print(acc.item())

        classes = classes.clone().cpu().numpy()
        classes = classes.reshape(90112).tolist()
        for n in range(90112):
            f.write(str(classes[n])+'\n')
        f.close()
        # print(classes)



        # preds = model(inputs)
        # _, classes = torch.max(preds, -1)
        # labels = labels.clone().cpu().numpy()
        # labels = labels.reshape(90112).tolist()
        # for n in range(90112):
        #     f.write(str(labels[n])+'\n')
        # f.close()
        # acc = (classes == labels).float().sum() / labels.numel()
        # print(acc.item())
