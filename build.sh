echo 'Naive Word Count'
clang++ NaiveWordCount.cpp 
./a.out

echo 'Map Reduce Word Count'
clang++ -Xpreprocessor -fopenmp MapReduceWordCount.cpp -lomp
./a.out