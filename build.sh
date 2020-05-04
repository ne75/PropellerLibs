#!/bin/bash

colormake -j4 clean MMODEL=cmm; colormake -j4 clean MMODEL=lmm; colormake -j4 install MMODEL=cmm; colormake -j4 install MMODEL=lmm