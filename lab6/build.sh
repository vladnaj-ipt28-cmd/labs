#!/bin/bash
echo "Початок компіляції проєкту..."
mkdir -p build
cd build
cmake ..
make
echo "Компіляцію завершено!"