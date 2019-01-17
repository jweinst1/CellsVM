# Main CMake File for copper #
cmake_minimum_required(VERSION 3.5)

# Project cellsvm begins here #
project(cellsvm)

add_library(cellsvm STATIC cvml.c)
