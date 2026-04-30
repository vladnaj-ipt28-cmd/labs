#!/bin/bash
echo "Встановлення залежностей для OpenCV та збірки..."
sudo apt update
sudo apt install -y libopencv-dev cmake g++ make
echo "Залежності встановлено!"