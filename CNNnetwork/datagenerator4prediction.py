import tensorflow as tf
import numpy as np
import random

from tensorflow.python.framework import dtypes
from tensorflow.python.framework.ops import convert_to_tensor


class PreDataGenerator(object):
    """Wrapper class around the new Tensorflows dataset pipeline.

    Requires Tensorflow >= version 1.12rc0
    """

    def __init__(self, txt_file, mode, batch_size, num_classes, iterator_size, kth_init_op, classifier_version=1):
        self.txt_file = txt_file
        self.num_classes = num_classes

        self.k = kth_init_op
        self.s = iterator_size

        # retrieve the data from the text file
        if classifier_version == 1:
            self._read_txt_file()
        elif classifier_version == 3:
            self._read_txt_file_v3()
        elif classifier_version == 4:
            self._read_txt_file_v4()
        elif classifier_version ==5:
            self._read_txt_file_randomly()
        else:
            raise ValueError('Invalid version v{}'.format(classifier_version))

        # number of samples in the dataset
        self.data_size = len(self.img_paths)

        # convert lists to TF tensor
        self.img_paths = convert_to_tensor(self.img_paths, dtype=dtypes.string)

        # create dataset
        data = tf.data.Dataset.from_tensor_slices((self.img_paths))  # .repeat(kth_init_op).batch(iterator_size)#todo

        # distinguish between train/infer/pre. when calling the parsing functions
        if mode == 'predicting':
            data = data.map(self._parse_function_prediction, num_parallel_calls=8).prefetch(
                buffer_size=100 * batch_size)

        else:
            raise ValueError("Invalid mode '%s'." % (mode))

        # create a new dataset with batches of images
        data = data.batch(batch_size)

        iterator = data.make_initializable_iterator()
        self.iterator = iterator

        self.data = data

    def _read_txt_file(self):
        """Read the content of the text file and store it into lists."""
        self.img_paths = []
        self.labels = []  # comment it when prediction
        with open(self.txt_file, 'r') as f:
            lines = f.readlines()
            for line in lines:
                items = line.split(',')  # .csv
                self.img_paths.append(items[0])
                self.labels.append(int(items[1]))  # comment it when prediction

    def _parse_function_prediction(self, filename):
        """Input parser for samples of the prediction set."""

        # load and preprocess the input file
        height = 227
        width = 227
        depth = 8#todo: parsing shape
        image_bytes = height * width * depth * 4

        img_string = tf.read_file(filename)
        bytes = tf.decode_raw(img_string, out_type=tf.float32)
        img = tf.reshape(bytes, [height, width, depth])

        return img

    def _read_txt_file_v3(self):  # ,kth_init_op,iterator_size
        """Read the content of the text file and store it into lists."""

        self.img_paths = []
        with open(self.txt_file, 'r') as f:
            lines = f.readlines()[self.s * self.k:self.s * self.k + self.s]
            for line in lines:
                items = line.split(',')  # .csv
                self.img_paths.append(items[0])

    def _read_txt_file_v4(self):

        self.img_paths = []
        with open(self.txt_file, 'r') as f:
            lines = f.readlines()[self.s * self.k:self.s * self.k + self.s]
            for line in lines:
                items = line.split(',')  # .csv
                self.img_paths.append(items[0])
        pass  # todo

    def _read_txt_file_randomly(self):
        """Read the content of the text file and store it into lists."""
        self.img_paths = []
        self.labels = []  # comment it when prediction
        self.flen=len(open(self.txt_file,'r').readlines())
        # self.samlen=2000
        with open(self.txt_file, 'r') as f:
            lines = f.readlines()
            #read_txt_file.ipynb
            for i in range(self.s):
                items = lines[random.randint(0,self.flen-1)].split(',')  # .csv
                self.img_paths.append(items[0])
                self.labels.append(int(items[1]))  # comment it when prediction
