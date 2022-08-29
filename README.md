# MapReduce-Framework

## About
MapReduce Framework implemented in C++. Supports multi-threading in a shared memory model.<br>
[![License MIT](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

### Motivation:
[MapReduce](https://en.wikipedia.org/wiki/MapReduce) is a popular programming paradigm used for processing and generating big-data
on clusters using distributed and parallel algorithms. It consists of a map phase and a reduce
phase. In the map phase, a map job takes a set of data as the input and converts it to another
set of data as tuples generally in the form of <key,value> pairs. The reduce job takes the output
of the map phase as the input and combines these into a smaller set of tuples, generally this is
a summary operation which results in a single value for each key. 
#### Word counting
This paradigm is generally introduced with the word counting problem which is defined as follows: " Given a huge list of
words, return the count/frequency of occurrence of each word in the input list". It is important to
note the fact that a single-threaded implementation of MapReduce will not enhance
performance and the usefulness of this paradigm comes into play only when a multi-threaded
implementation is done on a system with multiple cores. In the current world of big-data, this
framework would increase speed, flexibility, simplicity and scalability.

### Requirements 🔧
* gcc/g++ version 4.2.1 or higher.
* OpenMP support in the Cpp compiler

### Installation 🔌
1. Press the **Fork** button (on the top right corner of the page) to save a copy of this project on your account.

2. Download the repository files (project) from the download section or clone this project by typing in the bash the following command:

       git clone https://github.com/sramakrishnan247/MapReduce-Framework
3. Import the project using VSCode(or any other Ide or your favorite text editor)
4. Download the Gutenberg dataset from [here](https://web.eecs.umich.edu/~lahiri/gutenberg_dataset.html)
5. Extract the dataset to the project srce directory and run preprocess.py
6. Run the build.sh script(For Linux/Windows please make changes to build.sh similar to [this](https://stackoverflow.com/questions/12002304/how-to-compile-openmp-using-g)) 

### Demo
![Demo](https://github.com/sramakrishnan247/MapReduce-Framework/blob/main/execution.gif)

### Implementation outline
The dataset has been used for this project has been taken from [Project Gutenberg](http://www.gutenberg.org/).  The data is stored across text files. The program input is a file that has the name of each text file and the output is a text file with the words and their respective counts. The input is read using a reader thread which will add the file pointer for each file to a worker queue. For the map job and reduce job, there is a mapper thread and a reducer thread respectively. Finally, a writer thread writes the final word count to the output file. In order to store the count for each word, a hash table that is accessible globally is used to store the data in the form of <key,count> pairs. Updates to values in the hash table will be atomic in nature. Although the mapper phase can be run in parallel, the final reduce phase writes everthing to a global std::map.


### References
* [Project Gutenberg](http://www.gutenberg.org/)
* [Dataset: Complexity of Word Collocation Networks: A Preliminary Structural Analysis, Lahiri, Shibamouli](https://web.eecs.umich.edu/~lahiri/gutenberg_dataset.html)
* [MapReduce: Simplified Data Processing on Large Clusters, Jeff Dean et al, Google Inc](https://research.google/pubs/pub62/)
