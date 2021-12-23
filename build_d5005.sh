#!/bin/bash
source /data/intel_fpga/devcloudLoginToolSetup.sh
tools_setup -t S10DS 
cd ~/INT_Bioinformatics/WavefrontVariants
aoc WFO2ColsFPGA.cl > compile_d5005.log
