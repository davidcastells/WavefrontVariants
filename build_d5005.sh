#!/bin/bash
source /data/intel_fpga/devcloudLoginToolSetup.sh
tools_setup -t S10DS 
cd ~/INT_Bioinformatics/WavefrontVariants
aoc WFO2ColsFPGA.cl -D TILE_LEN=3 -D EXTEND_ALIGNED -D WORKGROUP_SIZE=8 > compile_d5005.log
