# 3DMPM
 
This is the implementation of the 3DMPM architecture described in this paper: 

_[Learning 3D Mineral Prospectivity from 3D Geological Models Using Convolutional Neural Networks](https://www.sciencedirect.com/science/article/pii/S0098300422000383)._
	
## Hardware requirements
- GeForce GTX 1050 Ti or higher

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
1. Generating Eigenfunctions (MATLAB)
   1. Data Preprocessing
      
      - Prior to calculating the Laplace-Beltrami eigenfunctions, it is imperative to ensure that the triangulation of the geological model is regular. Two types of data are crucial for the computation of eigenfunctions: `face.txt`, which contains the indices of vertices forming each triangle, and `vert.txt`, which contains the vertex data for each triangle.
        
   2. Computation of Eigenfunctions 
      
      - Subsequently, the eigenfunctions program is employed to calculate the eigenfunctions of the model. The computed eigenfunctions are stored within the PHI table. Selecting a specific number of eigenfunctions influences the resulting projection's number of channels. In this instance, 16 eigenfunctions are considered. Columns two through seventeen are saved as a CSV file, denoted as `P.csv` for subsequent projections.

2. Projection using ScanProjection (C++)
   1. Configuration File Modification
      - `params.ini`: Adjust paths. Open the `params.ini` file and primarily modify the last five paths, which correspond to the storage path for the model file, model eigenfunction files, pending projection voxel data files, generated binary projection files, and generated image projection files.
      - `params.h`: Open the `params.h` header file. The parameter "WITHOUT_NORMAL" indicates whether normal vectors are enabled, with a value of 0 indicating the output of normal vectors. The parameter "KDims" signifies the number of eigenfunctions. The total number of channels in the generated projection is calculated as follows: channels for normal vectors (3 channels) + distance (1 channel) + number of eigenfunctions (assuming 16) = 20 channels.
      - `params.cpp`: Fine-tune parameters like "DepthOffset" based on geological considerations.
   2. Execution
      - To ensure data accuracy, preliminary output checks are recommended. Inspect generated PNG files for texture variations and validate the presence of fluctuating patterns. Confirm appropriate projection angles; excessive black regions indicate potential algorithm adjustments. Noise indicates a model-file mismatch.

3. Training and Inference (Python)
   1. Data Preparation
      - Organize data into lists, associating voxel labels with binary file paths. Split known region voxels into training and validation sets.
   2. Model Training
      - `alexnet.py`: the backbone network architecture.
      - `finetune.py`: Involves network training. Modify data paths and the model's storage location, then execute the network training procedure `finetune.py`.
   3. Inference
       - Initial execution of `classifier_v4.py` ensures functionality, followed by executing `for_cycle_2.sh` for batch processing.

## Citation
```/angular2/
@article{DENG2022105074,
title = {Learning 3D mineral prospectivity from 3D geological models using convolutional neural networks: Application to a structure-controlled hydrothermal gold deposit},
journal = {Computers & Geosciences},
volume = {161},
pages = {105074},
year = {2022},
issn = {0098-3004},
doi = {https://doi.org/10.1016/j.cageo.2022.105074},
url = {https://www.sciencedirect.com/science/article/pii/S0098300422000383},
author = {Hao Deng and Yang Zheng and Jin Chen and Shuyan Yu and Keyan Xiao and Xiancheng Mao},
keywords = {Mineral prospectivity mapping, Convolutional neural networks, 3D geological models, Gold deposits, Structure-controlled deposits},
abstract = {Three-dimensional (3D) geological models are typical data sources in 3D mineral prospectivity modeling. However, identifying prospectivity-informative predictor variables from 3D geological models is a challenging and work-intensive task. Motivated by the ability of convolutional neural networks (CNNs) to learn intrinsic features, in this paper, we present a novel method that leverages CNNs to learn 3D mineral prospectivity from 3D geological models. By exploiting this learning ability, the proposed method simplifies the complex correlations of mineralization and circumvent the need for designing the predictor variables. Specifically, to analyze unstructured 3D geological models using CNNs—whose inputs should be structured—we develop a 2D CNN framework where the geometry of geological boundary is compiled and reorganized into multi-channel images and fed into the CNN. This ensures the effective and efficient training of the CNN while facilitating the representation of mineralization control. The presented method is applied to a typical structure-controlled hydrothermal deposit, the Dayingezhuang gold deposit in eastern China; the presented method is compared with prospectivity modeling methods using designed predictor variables. The results show that the presented method has a performance boost in terms of the 3D prospectivity modeling and decreases the workload and prospecting risk in the prediction of deep-seated orebodies.}
}
```


