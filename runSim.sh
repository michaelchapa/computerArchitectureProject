#!/bin/bash

# Forces a remake of cacheSimulator.exe
make -B 

# Replaces the files in "Output" directory
cacheSimulator.exe -f A-9_new_1.5.pdf.trc -s 1024 -b 16 -a 16 -r RND >> output/O1.txt
cacheSimulator.exe -f A-9_new_1.5.pdf.trc -s 8192 -b 64 -a 4 -r RND >> output/O2.txt
cacheSimulator.exe -f A-9_new_1.5.pdf.trc -s 4096 -b 32 -a 1 -r RND >> output/O3.txt
cacheSimulator.exe -f A-9_new_1.5.pdf.trc -s 1024 -b 4 -a 8 -r RND >> output/O4.txt
cacheSimulator.exe -f A-9_new_1.5.pdf.trc -s 4096 -b 8 -a 32 -r RND >> output/O5.txt
# In Man pages read about navigating "less" if you're having trouble
