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
### Generating Eigenfunctions (MATLAB)
1. Data Preprocessing
      <p>Start with the initial model data, often exported as GOCAD surface files. It's advisable to save these as text files for easy access. Eigenfunction calculation requires two types of data:</p>
      
      - `face.txt`: Contains indices of vertices corresponding to each triangle constituting the model.
      - `vert.txt`: Holds vertex data for each triangle in the model.
        
2. Computation 
      
      Proceed to compute the eigenfunctions of the model using the `eigenfunctions` program. The computed eigenfunctions are stored in the PHI table. Let's consider an example where 16 eigenfunctions are selected. Save the 16 columns of eigenfunction data as a CSV file (`P.csv`) to be used during projection.

### Projection using ScanProjection (C++)
1. Modify Configuration Files
   1. Specify the input and output directories in `params.ini` by updating the paths for the last five entries:
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
   2. Open the `params.h` file:
      - `WITHOUT_NORMAL`: Toggle to enable normal vectors (0 for enabled).
      - `KDims`: Specify the number of eigenfunctions (e.g., 16 in this case).
      <p>The resulting channels in the projection: normals (3 channels) + distance (1 channel) + eigenfunction (16 channels) = 20 channels.</p>
   3. Modify the `params.cpp` file:
      
      Adjust the `DepthOffset` parameter based on geological scale or other relevant factors.


 
2. Initiate Projection
	<p>Before proceeding with deep learning, validate data accuracy by performing an output check:</p>
 
      - Inspect generated PNG files to identify texture variations. Texture-less images indicate data issues.
      - Verify the projection angles; excessive black regions might require further adjustments to the projection algorithm.
      - Widespread noise suggests mismatched model or eigenfunction files.



### Training and Prediction (Python)
   1. Data Preparation

      The multi-channel binary files generated in the previous step serve as raw data for training and prediction. Create a list pairing voxel labels with corresponding binary file paths. Divide known area voxels into training and validation sets.
      
   2. Model Training
      
      - `alexnet.py`: The backbone network architecture.
      - `finetune.py`: Train the model using the prepared data. Adjust data source and model save paths as needed.
      
   3. Prediction
      - Run `classifier_prediction.py` first to ensure error-free execution.
      - Execute the `for_cycle_2.sh` batch file for continuous training and prediction cycles, specifying
        
```
    tf.flags.DEFINE_integer('pre_size', <your_prediction_batch_size>, 'prediction size')
    tf.flags.DEFINE_integer('iter_epoch', <your_batchs_per_epoch>, 'pre_size data per iter_epoch')
```  
in `classifier_v4.py`.


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


