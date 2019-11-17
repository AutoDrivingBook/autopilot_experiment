import torch
import torch.utils.data as data
import numpy as np
import os, sys, h5py, subprocess, shlex
import glob
import pdb
BASE_DIR = os.path.dirname(os.path.abspath(__file__))


def _get_data_files(list_filename):
    with open(list_filename) as f:
        return [line.rstrip() for line in f]


def _load_data_file(name):
    f = h5py.File(name)
    data = f['data'][:]
    label = f['label'][:]
    return data, label


class Indoor3DSemSeg(data.Dataset):

    def __init__(self, num_points, train=True, download=True, data_precent=1.0):
        super().__init__()
        self.data_precent = data_precent
        self.train, self.num_points = train, num_points

        pcd_path = '/home1/xuhui/Pointnet2_PyTorch-master/pointnet2/data/3d-sample/pcd_files/'
        label_path = '/home1/xuhui/Pointnet2_PyTorch-master/pointnet2/data/3d-sample/label/'
        pcd_list,label_list = [],[]
        points,labels = [],[]
        pcd_list = glob.glob(os.path.join(pcd_path,'*'))
        pcd_list.sort()
        label_list = glob.glob(os.path.join(label_path,'*'))
        label_list.sort()
        for i,files in enumerate(pcd_list):
            # print(files)
            f = open(files)
            for n,line in enumerate(f):
                if n >=11 and n<=90122:
                    # data = f.readlines()[8:]
                    data = line.strip().split()
                    data =[float(d) for d in data]
                    points.append(data)
                
                else:
                    continue
            # print(np.array(points).shape)
        points = np.array(points)
        self.points = np.reshape(points,(100,90112,3))
        # print(points.shape)

        for i,files in enumerate(label_list):
            f = open (files)
            for n,line in enumerate(f):
                if n >=0 and n<=90111:
                    # data = f.readlines()[8:]
                    data = line.strip().split()
                    data =[float(d) for d in data]
                    labels.append(data)
                
                else:
                    continue

        labels = np.array(labels)
        self.labels = np.reshape(labels,(100,90112))
        # print(labels.shape)

            # data_batches = np.concatenate(p, 0)

        # self.folder = "indoor3d_sem_seg_hdf5_data"
        # self.data_dir = os.path.join(BASE_DIR, self.folder)
        # self.url = "https://shapenet.cs.stanford.edu/media/indoor3d_sem_seg_hdf5_data.zip"
        # if download and not os.path.exists(self.data_dir):
        #     zipfile = os.path.join(BASE_DIR, os.path.basename(self.url))
        #     subprocess.check_call(
        #         shlex.split("curl {} -o {}".format(self.url, zipfile))
        #     )

        #     subprocess.check_call(
        #         shlex.split("unzip {} -d {}".format(zipfile, BASE_DIR))
        #     )
        #     subprocess.check_call(shlex.split("rm {}".format(zipfile)))

        # all_files = _get_data_files(
        #     os.path.join(self.data_dir, "all_files.txt")
        # )
        # room_filelist = _get_data_files(
        #     os.path.join(self.data_dir, "room_filelist.txt")
        # )
        # data_batchlist, label_batchlist = [], []
        # for f in all_files:
        #     d, l = _load_data_file(os.path.join(BASE_DIR, f))
        #     label_batchlist.append(l)
        # print(label_batchlist)
            # print(l.shape)
        # data_batches = np.concatenate(data_batchlist, 0)
        # labels_batches = np.concatenate(label_batchlist, 0)

        # test_area = 'Area_5'


        # if self.train:
        #     self.points = data_batches[train_idxs, ...]
        #     self.labels = labels_batches[train_idxs, ...]
        # else:
        #     self.points = data_batches[test_idxs, ...]
        #     self.labels = labels_batches[test_idxs, ...]
        # fo = open('/home1/xuhui/Pointnet2_PyTorch-master/pointnet2/train/indoor.txt','w')
        # fo.write(str(self.labels)+'\n')
        # fo.close()





    def __getitem__(self, idx):
        pt_idxs = np.arange(0, self.num_points)
        np.random.shuffle(pt_idxs)

        current_points = torch.from_numpy(self.points[idx, pt_idxs].copy()
                                         ).type(torch.FloatTensor)
        current_labels = torch.from_numpy(self.labels[idx, pt_idxs].copy()
                                         ).type(torch.LongTensor)

        return current_points, current_labels

    def __len__(self):
        return int(self.points.shape[0] * self.data_precent)

    def set_num_points(self, pts):
        self.num_points = pts

    def randomize(self):
        pass


if __name__ == "__main__":
    dset = Indoor3DSemSeg(16, "./", train=True)
    print(dset[0])
    print(len(dset))
    dloader = torch.utils.data.DataLoader(dset, batch_size=32, shuffle=True)
    for i, data in enumerate(dloader, 0):
        inputs, labels = data
        if i == len(dloader) - 1:
            print(inputs.size())
