
import os
os.environ['QT_API']='pyside'
import numpy as np
from mayavi import mlab
    
def get_image(file_name,height=64,width=64):
    file_data = np.fromfile(file_name, np.float32)
    # file_data.shape 
    image = np.reshape(file_data, [width,height,4])
    return image

def show(file_path,save_dir = None):
    XYZ = get_image(file_path)
    #len(X)=size,len(X[index])=size
    print(np.max(XYZ[:,:,2]))
    print(np.min(XYZ[:,:,2]))
    x = np.squeeze(XYZ[:,:,0]).tolist()
    y = np.squeeze(XYZ[:,:,1]).tolist()
    z = np.squeeze(XYZ[:,:,2]).tolist()
    
    #print("max value is {},min value is {}".format(max(z),min(z)))
    
    if save_dir != None:
      if not os.path.exists(save_dir):
        os.mkdir(save_dir)
    temp = os.path.splitext(file_path)[0]
    pre_name = os.path.basename(temp)
      
    sub_dir = file_path.split("\\")[-2]
    new_subdir = os.path.join(save_dir,sub_dir)
    if not os.path.exists(new_subdir):
      os.mkdir(new_subdir)  
        
    new_name = os.path.join(new_subdir,pre_name)
    
    with open(new_name+'.txt','w') as f:
      for i in range(len(x)):
        for j in range(len(x)):
          f.write("v"+" "+str(x[i][j])+" "+str(y[i][j])+" "+str(z[i][j])+"\n")   
       
    mlab.points3d(x,y,z,mode='point')
    mlab.show()
if __name__ == '__main__':
    show(r"./3d-sample/bin_files/002_00000000.bin",r"./pointclouds_obj")
