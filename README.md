# 3DMPM
 
This is the implementation of the 3DMPM architecture described in this paper: 

_Learning 3D Mineral Prospectivity from 3D Geological Models Using Convolutional Neural Networks,_

by Hao Deng, Yang Zheng, Jin Chen*, Shuyan Yu, Zhankun Liu, Xiancheng Mao

## Hardware requirements
- GeForce RTX 2080 Ti

## Program language 
- MATLAB (eigenfunctions)
- C++ (ScanProjection)
- Python (CNNnetwork)

## Dependencies required
>ScanProjection
> + glad
> + glfw
> + libpng

>CNNnetwork
> + Ubuntu 18.04
> + Python 3.6
> + NumPy 1.14.5
> + TensorFlow 1.14.0
> + TensorBoard 1.10.0
>
## Usage
1) Run `main.m` in "eigenfunctions" library to result in a series of Laplace-Beltrami eigenvalues and eigenfunctions. 
2) Execute the Visual Studio solution file `ScanProjection.sln` in "ScanProjection" library to project shape descriptors into images `*.bin`.
3) Run the network training procedure `finetune.py` with loading parameters pretrained on [ImageNet](https://www.cs.toronto.edu/~guerzhoy/tf_alexnet/bvlc_alexnet.npy) on Linux. 
4) Run the testing procedure (after executing training) on Linux:
```
sh for_cycle_2.sh
```

,specifying 
```
tf.flags.DEFINE_integer('pre_size', <your_prediction_batch_size>, 'prediction size')
tf.flags.DEFINE_integer('iter_epoch', <your_batchs_per_epoch>, 'pre_size data per iter_epoch')
```
in `classifier_v4.py`.
