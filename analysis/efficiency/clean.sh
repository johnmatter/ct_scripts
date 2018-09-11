#!/bin/bash
# Remove files from compiling root macros
find . -type f \( -name "*.d" -o -name "*.so" -o -name "*.pcm" \) -exec rm {} \;
