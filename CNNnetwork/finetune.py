import os
from typing import Optional, Any
import numpy as np
import tensorflow as tf
from alexnet import AlexNet
from datagenerator import ImageDataGenerator
from datetime import datetime
from tensorflow.data import Iterator
import time

timea = time.time()

"""
Configuration Part.
"""
os.environ["CUDA_VISIBLE_DEVICES"] = "0"
# Path to the textfiles for the trainings and validation set
train_file = './train_list.csv'  # todo: 修改train_file = './List/jiaojia/28xunlian/jiaojia_train_all_list.csv'
val_file = './val_list.csv'  # todo:修改val_file = './List/jiaojia/28xunlian/jiaojia_val_all_list.csv'

with open(train_file, 'r') as f1, open(val_file, 'r')as f2:
    lines = f1.readlines()
    len_train_ones = 0
    len_train_zeros = 0
    len_train_dataset = 0
    for line in lines:
        items = line.split(',')
        len_train_dataset += 1

        if int(items[1]) == 1:
            len_train_ones += 1
    lines1 = f2.readlines()
    for line in lines1:
        items = line.split(',')
        len_train_dataset += 1

        if int(items[1]) == 1:
            len_train_ones += 1
    len_train_zeros = len_train_dataset - len_train_ones

# with open(val_file, 'r') as f:
#     lines = f.readlines()
#     len_val_ones = 0
#     len_val_zeros = 0
#     len_val_dataset=0
#     for line in lines:
#         items = line.split(',')
#         len_val_dataset+=1
#
#         if int(items[1]) == 1:
#             len_val_ones += 1
#     len_val_zeros = len_val_dataset - len_val_ones

# Learning paramsxiugai
learning_rate = 5e-5  # TODO: 5e-5, 1e-4, 1e-3
num_epochs = 200  # TODO :2:20
batch_size = 256  # TODO: 128
depth = 6  # todo:11
# Network params
dropout_rate = 0.5
weight_decay = 1e-3  # todo:1e-4,1e-3,5e-4
moving_average_decay = 0.99
num_classes = 2
train_layers = ['conv1', 'fc6', 'fc7', 'fc8']  # TODO: 'fc8'
all_layers = ['conv1', 'conv2', 'conv3', 'conv4', 'conv5', 'fc6', 'fc7', 'fc8']
frozen_layers = ['conv2', 'conv3']

# How often we want to write the tf.summary data to disk
display_step = 20

# Path for tf.summary.FileWriter and to store model checkpoints
filewriter_path_train = "./Tensorboard_5c/train"  # TODO 11c
filewriter_path_val = "./Tensorboard_5c/test"  # TODO 11c
checkpoint_path = "./Checkpoints_5c"  # TODO   11c

"""
Main Part of the finetuning Script.
"""

# Create parent path if it doesn't exist
if not os.path.isdir(checkpoint_path):
    os.mkdir(checkpoint_path)

# Place data loading and preprocessing on the cpu
with tf.device('/cpu:0'):
    tr_data = ImageDataGenerator(train_file,
                                 mode='training',
                                 batch_size=batch_size,
                                 num_classes=num_classes,
                                 depth_num=depth,
                                 shuffle=True)
    val_data = ImageDataGenerator(val_file,
                                  mode='inference',
                                  batch_size=batch_size,
                                  num_classes=num_classes,
                                  depth_num=depth,
                                  shuffle=False)

    # create an reinitializable iterator given the dataset structure
    iterator = Iterator.from_structure(tr_data.data.output_types,
                                       tr_data.data.output_shapes)
    next_batch = iterator.get_next()

# Ops for initializing the two different iterators
training_init_op = iterator.make_initializer(tr_data.data)
validation_init_op = iterator.make_initializer(val_data.data)

# TF placeholder for graph input and output
x = tf.placeholder(tf.float32, [batch_size, 227, 227, depth])
y = tf.placeholder(tf.float32, [batch_size, num_classes])
keep_prob = tf.placeholder(tf.float32)
global_step = tf.Variable(0)

# Initialize model
model = AlexNet(x, keep_prob, num_classes, train_layers, weight_decay, moving_average_decay, frozen_layer=frozen_layers)

# Link variable to model output
score = model.fc8
soft_max = tf.nn.softmax(score)

config = tf.ConfigProto()
config.gpu_options.per_process_gpu_memory_fraction = 0.9
config.gpu_options.allow_growth = True

# Start Tensorflow session

sess = tf.Session(config=config)

# y_1 = soft_max[:, 1]
# y_0 = soft_max[:, 0]
# ratio_source = y_1 / y_0 * len_train_zeros / len_train_ones
# soft_max_new = tf.Variable(tf.zeros_like(tensor=soft_max))
# soft_max_new[:, 1].assign(ratio_source / (ratio_source + 1))
# soft_max_new[:, 0].assign(1 / (ratio_source + 1))


# f1score
def f1(y_hat, y_true, model='multi'):
    epsilon = 1e-7
    y_hat = tf.round(y_hat)

    tp = tf.reduce_sum(tf.cast(y_hat * y_true, 'float'), axis=0)
    # tn = tf.sum(tf.cast((1-y_hat)*(1-y_true), 'float'), axis=0)
    fp = tf.reduce_sum(tf.cast((1 - y_hat) * y_true, 'float'), axis=0)
    fn = tf.reduce_sum(tf.cast(y_hat * (1 - y_true), 'float'), axis=0)

    p = tp / (tp + fp + epsilon)
    r = tp / (tp + fn + epsilon)

    f1 = 2 * p * r / (p + r + epsilon)
    f1 = tf.where(tf.is_nan(f1), tf.zeros_like(f1), f1)
    if model == 'single':
        return f1
    if model == 'multi':
        return tf.reduce_mean(f1)


# List of trainable variables of the layers we want to train
var_list = [v for v in tf.trainable_variables() if v.name.split('/')[0] in list(set(all_layers) - set(frozen_layers))]

# ema
variable_averages = tf.train.ExponentialMovingAverage(moving_average_decay, global_step)
variable_averages_op = variable_averages.apply(var_list)

# Op for calculating the loss
with tf.name_scope("cross_ent"):
    empirical_loss = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits_v2(logits=score,
                                                                               labels=tf.clip_by_value(y,
                                                                                                       1e-4,
                                                                                                       tf.reduce_max(
                                                                                                           y))))
    tf.add_to_collection('losses', empirical_loss)

loss = tf.add_n(tf.get_collection('losses'))

# Train op
with tf.name_scope("train"):
    # Get gradients of all trainable variables
    # gradients = tf.gradients(loss, var_list)
    # gradients = list(zip(gradients, var_list))

    # Create optimizer and apply gradient descent to the trainable variables
    lr = tf.train.exponential_decay(learning_rate=learning_rate, global_step=global_step, decay_steps=80,  # todo:100
                                    decay_rate=0.96,
                                    staircase=True)
    optimizer = tf.train.AdamOptimizer(lr)
    gradients = optimizer.compute_gradients(loss)
    capped_gradients = [(tf.clip_by_value(grad, -1., 1.), var) for grad, var in gradients if grad is not None]
    train_step = optimizer.apply_gradients(grads_and_vars=capped_gradients, global_step=global_step)
    with tf.control_dependencies([train_step, variable_averages_op]):
        train_op = tf.no_op(name='train')

# Add gradients to summary
for gradient, var in capped_gradients:
    tf.summary.histogram(var.name + '/gradient', gradient)

# Add the variables we train to/media/yunfeng/4f13cc14-62ff-412f-a335-20b03f7041c3/yunfeng/CODES/ALEXNET_CODES/Tensorboard_11c the summary
for var in var_list:
    tf.summary.histogram(var.name, var)

# Add the loss to summary
tf.summary.scalar('cross_entropy', empirical_loss)

# Evaluation op: Accuracy of the model
with tf.name_scope("accuracy"):
    correct_pred = tf.equal(tf.argmax(soft_max, 1), tf.argmax(y, 1))
    accuracy = tf.reduce_mean(tf.cast(correct_pred, tf.float32))

with tf.name_scope('f1score'):
    f1score = f1(soft_max, y)

with tf.name_scope("auc"):
    softmax=tf.nn.softmax(score)
    # prediction_list = tf.placeholder(tf.float32, [batch_size, num_classes], name='prediction_list')
    auc,auc_op=tf.metrics.auc(labels=y,predictions=soft_max)

# Add the accuracy to the summary
# train_summary=tf.summary.scalar('training_accuracy', accuracy)
# validation_summary=tf.summary.scalar('validation_accuracy',accuracy)
tf.summary.scalar('accuracy', accuracy)
tf.summary.scalar('f1score', f1score)
tf.summary.scalar('auc',auc)


# Merge all summaries together
merged_summary = tf.summary.merge_all()

# Initialize an saver for store model checkpoints
saver = tf.train.Saver()

# Get the number of training/validation steps per epoch
train_batches_per_epoch = int(np.floor(tr_data.data_size / batch_size))
val_batches_per_epoch = int(np.floor(val_data.data_size / batch_size))



# Initialize the FileWriter
train_writer = tf.summary.FileWriter(filewriter_path_train, sess.graph)
validate_writer = tf.summary.FileWriter(filewriter_path_val)

sess.run(tf.global_variables_initializer())
sess.run(tf.local_variables_initializer())

# train_writer.add_graph(graph=sess.graph)
# validate_writer.add_graph(graph=sess.graph)

# Load the pretrained weights into the non-trainable layer
model.load_initial_weights(sess)

print("{} Start training...".format(datetime.now()))
print("{} Open Tensorboard at --logdir {},{}".format(datetime.now(),
                                                     filewriter_path_train, filewriter_path_val))

# Loop over number of epochs
for epoch in range(num_epochs):

    print("{} Epoch number: {}".format(datetime.now(), epoch + 1))

    # Initialize iterator with the training dataset
    sess.run(training_init_op)

    for step in range(train_batches_per_epoch):

        # get next batch of data
        img_batch, label_batch = sess.run(next_batch)

        # And run the training op
        _,_, g_step,softmax = sess.run([train_op,auc_op, global_step,soft_max], feed_dict={x: img_batch,
                                                                 y: label_batch,
                                                                 keep_prob: dropout_rate})

        # Generate summary with the current batch of data and write to file
        if step % display_step == 0:
            train_summary = sess.run(merged_summary, feed_dict={x: img_batch,
                                                                y: label_batch,
                                                                keep_prob: 1.})

            train_writer.add_summary(train_summary, epoch * train_batches_per_epoch + step)

    # Validate the model on the entire validation set
    print("{} Start validation".format(datetime.now()))
    sess.run(validation_init_op)
    val_acc = 0.
    val_count = 0
    for step2 in range(val_batches_per_epoch):
        img_batch, label_batch = sess.run(next_batch)
        # acc = sess.run(accuracy, feed_dict={x: img_batch,
        #                                     y: label_batch,
        #                                     keep_prob: 1.})
        if step2 % display_step == 0:
            val_summary, acc, f1_score,auc_value = sess.run([merged_summary, accuracy, f1score,auc], feed_dict={x: img_batch,
                                                                                                  y: label_batch,
                                                                                                  keep_prob: 1.})

            validate_writer.add_summary(val_summary, epoch * val_batches_per_epoch + step2)

        # predicted_label = pred.argmax(axis=1)
        # predictions.append(predicted_label[0])
        # auc_value= sess.run(auc, feed_dict={prediction_list:predictions,
        #                                y: label_batch,
        #                                keep_prob: 1.})
        val_acc += acc
        val_count += 1
    val_acc /= val_count
    print("{} Validation Accuracy = {:.4f}".format(datetime.now(), val_acc))
    print('{} Validation f1score = {:.4f}'.format(datetime.now(), f1_score))
    print('{} Validation auc_value = {:.4f}'.format(datetime.now(), auc_value))
    print("{} Saving checkpoint of model...".format(datetime.now()))

    # save checkpoint of the model
    checkpoint_name = os.path.join(checkpoint_path,
                                   'model_epoch' + str(epoch + 1) + '.ckpt')
    save_path = saver.save(sess, checkpoint_name)

    print("{} Model checkpoint saved at {}".format(datetime.now(),
                                                   checkpoint_name))
    timeb = time.time() - timea
    print('---------- Epoch %d / %d-%d steps spent %dh %dm %ds ----------' % (epoch + 1,
                                                                              epoch * train_batches_per_epoch,
                                                                              (epoch + 1) * train_batches_per_epoch,
                                                                              timeb // 3600,
                                                                              (timeb // 60) % 60,
                                                                              timeb % 60))
