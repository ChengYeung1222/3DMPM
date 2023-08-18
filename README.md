# 3DMPM
 
This is the implementation of the 3DMPM architecture described in this paper: 

_Learning 3D Mineral Prospectivity from 3D Geological Models Using Convolutional Neural Networks,_

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
1. Computation of Model Feature Functions (MATLAB)
   1. Data Preprocessing
      
      - Prior to calculating the Laplace-Beltrami feature functions, it is imperative to ensure that the triangulation of the geological body model is regular. Two types of data are crucial for the computation of feature functions: `face.txt`, which contains the indices of vertices forming each triangle, and `vert.txt`, which contains the vertex data for each triangle.
   2. Feature Function Computation
      
      - Subsequently, the eigenfunctions program is employed to calculate the feature functions of the model. The computed feature functions are stored within the PHI table. Selecting a specific number of feature functions influences the resulting projection's number of channels. In this instance, 16 feature functions are considered. Columns two through seventeen are saved as a CSV file, denoted as `P.csv` for subsequent projections.

2. Projection using ScanProjection (C++)
   1. Configuration File Modification
      - `params.ini`: Adjust paths. Open the `params.ini` file and primarily modify the last five paths, which correspond to the storage path for the model file, model feature function files, pending projection voxel data files, generated binary projection files, and generated image projection files.
      - `params.h`: Open the `params.h` header file. The parameter "WITHOUT_NORMAL" indicates whether normal vectors are enabled, with a value of 0 indicating the output of normal vectors. The parameter "KDims" signifies the number of feature functions. The total number of channels in the generated projection is calculated as follows: channels for normal vectors (3 channels) + distance (1 channel) + number of feature functions (assuming 16) = 20 channels.
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


