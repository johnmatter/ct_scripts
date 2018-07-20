#!/bin/bash
# Add dir to (DY)LD_LIBRARY_PATH so that root can find the rootmap
this_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$this_dir
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$this_dir
