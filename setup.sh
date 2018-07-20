#!/bin/bash
this_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

ctutil=$this_dir/src/util
deteff=$this_dir/src/detector-efficiency

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ctutil
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$ctutil
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$deteff
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$deteff
