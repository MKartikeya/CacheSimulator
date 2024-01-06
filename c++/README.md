 Cache Miss Simulator


Contents:
->Two test case files.
->"LAB6.cpp" is the file that contains the main code.
->README.md is the file with instructions  compile.
->"report.pdf" is the report file of the LAB assignment. 

Instructions to Compile:
-> The input must in a .config and .access file as specified. The input files are named as "cache.config" and "cache.access". Their names can be chnaged to that of yours.
-> The config shall be of this format:
        SIZE_OF_CACHE (number)
        BLOCK_SIZE (number)
        ASSOCIATIVITY (number)
        REPLACEMENT_POLICY (FIFO or LRU or RANDOM)
        WRITEBACK_POLICY (WB or WT)
    The input format must be strictly followed and are case sensitive. 
->The code is written in C++ for simplicity. Neccessary compiler must be installed.
    Mode: Address
    Mode: Address
    ** The address must be in hex format and valid. 

->You can use the following commands to run the program:
    g++ -o LAB6 LAB6.cpp
    ./LAB6
