#!/bin/bash 
for ((i=0;i<9;i++))
do
	python classifier_v4.py --iter_epoch $i
done
