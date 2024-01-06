The zip file consists of "Main.java" file along with three more trace files.

1) Run "javac Main.java" to create the required classes in the directory.
2) To run the code, use "java Main <cache_size> <associativity> <block_size> <tracefile_path>".

The Total hits and Misses are orinted at the end after setwise hits and misses.
The Flow of the code is:
1)Taking Input and Initialising the variables and calculation the number of bits.
2)Reading the file address by address and converting the hex-address into a binary string using parseLong.
3)Calculating setIndex and tag in decimal and checking if it is a hit and updating its relative time(index) or adding into the set by following LRU in the case of miss.
4)The LRU  is done y taking another 2D ArrayList that stores the relative time. HashMap also could be used.
 
