#!/bin/bash

make -j4 clean MMODEL=cmm; make -j4 clean MMODEL=lmm; make -j4 install MMODEL=cmm; make -j4 install MMODEL=lmm