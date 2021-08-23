import tensorflow as tf
import numpy as np
import scipy.io as sio

from tensorflow.python.client.session import Session as sess
# from tensorflow.contrib.learn.python.learn.datasets.base import Dataset
from tensorflow.python.framework import dtypes
from tensorflow.python.framework.ops import convert_to_tensor


class ImageDataGenerator(object):
    """Wrapper class around the new Tensorflows dataset pipeline.

    Requires Tensorflow >= version 1.12rc0
    """

    def __init__(self, txt_file, mode, batch_size, num_classes, depth_num,shuffle=True,
                 buffer_size=1000):
        """Create a new ImageDataGenerator.

        Recieves a path string to a text file, which consists of many lines,
        where each line has first a path string to an image and seperated by
        a space an integer, referring to the class number. Using this data,
        this class will create TensrFlow datasets, that can be used to train
        e.g. a convolutional neural network.

        Args:
            txt_file: Path to the text file.
            mode: Either 'training' or 'validation'. Depending on this value,
                different parsing functions will be used.
            batch_size: Number of images per batch.
            num_classes: Number of classes in the dataset.
            shuffle: Whether or not to shuffle the data in the dataset and the
                initial file list.
            buffer_size: Number of images used as buffer for TensorFlows
                shuffling of the dataset.

        Raises:
            ValueError: If an invalid mode is passed.

        """
        self.txt_file = txt_file
        self.num_classes = num_classes
        self.depth = depth_num
        # retrieve the data from the text file
        self._read_txt_file()

        # number of samples in the dataset
        self.data_size = len(self.labels)

        # initial shuffling of the file and label lists (together!)
        if shuffle:
            self._shuffle_lists()

        # convert lists to TF tensor
        self.img_paths = convert_to_tensor(self.img_paths,dtype=dtypes.string)
        self.labels = convert_to_tensor(self.labels,dtype=dtypes.int32)

        # create dataset
        data = tf.data.Dataset.from_tensor_slices((self.img_paths, self.labels))
        self.data_all = data

        # distinguish between train/infer. when calling the parsing functions
        if mode == 'training':
            data = data.map(self._parse_function_inference, num_parallel_calls=8).prefetch(buffer_size=100 * batch_size)


        elif mode == 'inference':
            data = data.map(self._parse_function_inference, num_parallel_calls=8).prefetch(buffer_size=100 * batch_size)

        else:
            raise ValueError("Invalid mode '%s'." % (mode))

        # shuffle the first `buffer_size` elements of the dataset
        if shuffle:
            data = data.shuffle(buffer_size=buffer_size)

        # create a new dataset with batches of images
        data = data.batch(batch_size)

        self.data = data

    def _read_txt_file(self):
        """Read the content of the text file and store it into lists."""
        self.img_paths = []
        self.labels = []
        with open(self.txt_file, 'r') as f:
            lines = f.readlines()
            for line in lines:
                items = line.split(',')  # .csv
                self.img_paths.append(items[0])
                self.labels.append(int(items[1]))

    def _shuffle_lists(self):
        """Conjoined shuffling of the list of paths and labels."""
        path = self.img_paths
        labels = self.labels
        permutation = np.random.permutation(self.data_size)
        self.img_paths = []
        self.labels = []
        for i in permutation:
            self.img_paths.append(path[i])
            self.labels.append(labels[i])

    def _parse_function_train(self, filename, label):
        """Input parser for samples of the training set."""
        # # convert label number into one-hot-encoding
        one_hot = tf.one_hot(label, self.num_classes)

        # load and preprocess the input file
        # img_paths = self.img_paths
        # labels = self.labels
        # for img_path, label in img_paths, labels:
        #     img = sio.loadmat(img_path)['data_reshaped']
        height = 227
        width = 227
        depth = self.depth  #修改
        image_bytes = height * width * depth * 4

        img_string = tf.read_file(filename)
        bytes = tf.decode_raw(img_string, out_type=tf.float32)
        img = tf.reshape(bytes,[height,width,depth])
        # img=tf.image.resize_images(img,[227,227])
        # img = tf.subtract(img, IMAGENET_MEAN)

        # img=tf.data.FixedLengthRecordDataset([filename],record_bytes=1130344)

        # filename=tf.train.string_input_producer([filename])
        # reader=tf.FixedLengthRecordReader(record_bytes=3*227*227*4)# input size
        # key,value=reader.read(filename)
        # bytes=tf.decode_raw(value,out_type=tf.float32)
        # img=tf.reshape(bytes,[227,227,3])

        # img=tf.reshape(tf.strided_slice(bytes,[1],[3*217*217]),[3,227,227])

        # img = sio.loadmat(filename)
        # img = img['data_reshaped']
        # img = tf.image.resize_images(img, [227, 227])

        # img=tf.decode_raw(filename, out_type=tf.float64)
        # img=tf.reshape(img,[217,217,3])
        # img=tf.image.resize_images(img,[227,227])

        return img, one_hot

        # # load and preprocess the image
        # img_string = tf.read_file(filename)
        # img_decoded = tf.image.decode_png(img_string, channels=3)
        # img_resized = tf.image.resize_images(img_decoded, [227, 227])
        # """
        # Data augmentation comes here.
        # """
        # img_centered = tf.subtract(img_resized, IMAGENET_MEAN)
        #
        # # RGB -> BGR cuDNN acceleration
        # img_bgr = img_centered[:, :, ::-1]

        # return img_bgr, one_hot

    def _parse_function_train_2(self, filename, label):
        """Input parser for samples of the training set."""
        # # convert label number into one-hot-encoding
        one_hot = tf.one_hot(label, self.num_classes)

        # load and preprocess the input file
        # img_paths=self.img_paths
        # labels=self.labels
        # for img_path,label in img_paths,labels:
        #     img=sio.loadmat(img_path)['data_reshaped']

        # img_string = tf.read_file(filename)
        # bytes = tf.decode_raw(img_string, out_type=tf.uint8)
        # img = tf.reshape(tf.slice(bytes, [1], [3 * 227 * 227]), [227, 227, 3])
        # img = tf.subtract(img, IMAGENET_MEAN)

        # img=tf.data.FixedLengthRecordDataset([filename],record_bytes=1130344)

        # filename=tf.train.string_input_producer([filename])
        # reader=tf.FixedLengthRecordReader(record_bytes=3*227*227*4)# input size
        # key,value=reader.read(filename)
        # bytes=tf.decode_raw(value,out_type=tf.float32)
        # img=tf.reshape(bytes,[227,227,3])

        # img=tf.reshape(tf.strided_slice(bytes,[1],[3*217*217]),[3,227,227])

        # img = sio.loadmat(filename)
        # img = img['data_reshaped']
        # img = tf.image.resize_images(img, [227, 227])

        # img=tf.decode_raw(filename, out_type=tf.float64)
        # img=tf.reshape(img,[217,217,3])
        # img=tf.image.resize_images(img,[227,227])

        # return img, one_hot
        # Dimensions of the images in the CIFAR-10 dataset.
        # See http://www.cs.toronto.edu/~kriz/cifar.html for a description of the
        # input format.
        # label_bytes = 1  # 2 for CIFAR-100
        height = 227
        width = 227
        depth = self.depth #修改
        image_bytes = height * width * depth * 8
        # Every record consists of a label followed by the image, with a
        # fixed number of bytes for each.
        # record_bytes = label_bytes + image_bytes

        # Read a record, getting filenames from the filename_queue.  No
        # header or footer in the CIFAR-10 format, so we leave header_bytes
        # and footer_bytes at their default of 0.
        # reader = tf.FixedLengthRecordReader(record_bytes=image_bytes)
        # key, value = reader.read(filename)
        #
        # # Convert from a string to a vector of uint8 that is record_bytes long.
        # img_decoded = tf.decode_raw(value, tf.float32)
        # # todo:!!! remember to reshape the decoded image !!!
        #
        # # load and preprocess the image
        # # img_string = tf.read_file(filename)
        # # img_decoded = tf.image.decode_png(img_string, channels=3)
        # img_resized = tf.image.resize_images(img_decoded, [227, 227])
        # """
        # Data augmentation comes here.
        # """
        # img_centered = tf.subtract(img_resized, IMAGENET_MEAN)
        #
        # # RGB -> BGR cuDNN acceleration
        # img_bgr = img_centered[:, :, ::-1]

        return img_bgr, one_hot

    def _parse_function_inference(self, filename, label):
        """Input parser for samples of the validation/test set."""
        # convert label number into one-hot-encoding
        one_hot = tf.one_hot(label, self.num_classes)

        # load and preprocess the input file
        height = 227
        width = 227
        depth = self.depth #修改
        image_bytes = height * width * depth * 4

        img_string = tf.read_file(filename)
        bytes = tf.decode_raw(img_string, out_type=tf.float32)
        img = tf.reshape(bytes,[height,width,depth])

        return img, one_hot

        # img=tf.image.resize_images(img,[227,227])
        # todo:scaling
        # img = tf.subtract(img, IMAGENET_MEAN)
        # img = tf.data.FixedLengthRecordDataset([filename], record_bytes=1130344)

        # filename=tf.train.string_input_producer([filename])
        # reader=tf.FixedLengthRecordReader(record_bytes=3*227*227*4)# input size
        # key,value=reader.read(filename)
        # bytes=tf.decode_raw(value,out_type=tf.float32)
        # img=tf.reshape(bytes,[227,227,3])

        # filename=tf.train.string_input_producer([filename])
        # reader=tf.FixedLengthRecordReader(record_bytes=3*217*217)# input size
        # value=reader.read(filename)
        # bytes=tf.decode_raw(value,out_type=tf.float32)
        # img=tf.reshape(tf.strided_slice(bytes,[1],[3*217*217]),[3,227,227])
        # img = sio.loadmat(filename)
        # img = img['data_reshaped']
        # img = tf.image.resize_images(img, [227, 227])

        # img=tf.decode_raw(filename, out_type=tf.float64)
        # img=tf.reshape(img,[217,217,3])
        # img=tf.image.resize_images(img,[227,227])
        # # load and preprocess the image
        # img_string = tf.read_file(filename)
        # img_decoded = tf.image.decode_png(img_string, channels=3)
        # img_resized = tf.image.resize_images(img_decoded, [227, 227])
        # img_centered = tf.subtract(img_resized, IMAGENET_MEAN)
        #
        # # RGB -> BGR
        # img_bgr = img_centered[:, :, ::-1]

        ##return img, one_hot

    # def next_batch(images, labels, batch_size):  # todo:next_batch
    #     perm = np.arange(images.shape[0])
    #     np.random.shuffle(perm)
    #     return images[perm[0:batch_size], :], labels[perm[0:batch_size], :]
