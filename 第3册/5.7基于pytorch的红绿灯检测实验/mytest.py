import mmcv
from mmcv.runner import load_checkpoint
from mmdet.models import build_detector
from mmdet.apis import inference_detector, show_result
# print('1111')
cfg = mmcv.Config.fromfile('configs/faster_rcnn_r50_fpn_1x.py')
cfg.model.pretrained = None

# 构建网络，载入模型
model = build_detector(cfg.model, test_cfg=cfg.test_cfg)
_ = load_checkpoint(model, '/data2/zzj/mmdetection/work_dirs/faster_rcnn_r50_fpn_1x/latest.pth')
# _ = load_checkpoint(model, 'https://s3.ap-northeast-2.amazonaws.com/open-mmlab/mmdetection/models/faster_rcnn_r50_fpn_1x_20181010-3d1b3351.pth')
# 如果通过网盘下载，取消下一行代码的注释，并且注释掉上一行
# _ = load_checkpoint(model, 'faster_rcnn_r50_fpn_1x_20181010-3d1b3351.pth')

# 测试一张图片
img = mmcv.imread('/data2/szh/coco/images/test2017/00000.jpg')
result = inference_detector(model, img, cfg)

show_result(img, result)
