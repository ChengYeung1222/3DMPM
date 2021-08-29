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
> + glad 0.1.29
> + glfw 3.2.1
> + libpng 1.6.17
> + Zlib 1.2.8

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
The paths and directionaries in params.ini should be specified. And the
dimension of properties is specified by using macro in `params.h` as:

```
    #define KDims 16 // dimension of properties
```
To set the projection program, you need to specify the input and output directories in `params.ini`:  
```/angular2/
    [meshPath]
    YOUR_3D_MODEL_PATH
    
    [propPath]
    YOUR_PROPERTY_CSV_PATH
    
    [voxelPath]
    YOUR_VOXEL_CSV_PATH
    
    [binDir]
    YOUR_BIN_FILE_OUTPUT_DIRECTIONARY
    
    [pngDir]
    YOUR_FILE_FILE_OUTPUT_DIRECTIONARY
```
User can switch off the output of png files in `params.ini` by setting

```
    [withPng]
    0
```
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
