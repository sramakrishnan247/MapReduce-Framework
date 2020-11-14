#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>
#include <cctype>
#include <omp.h>
#include <vector>
#include <cstdio>
#include <cstdlib>

#define NUM_OF_FILES 3000 
#define TABLE_SIZE 100000
#define MAX_WORD_COUNT 1000000

std::map<std::string, int> globalMap;
std::vector<std::vector<std::string> >  words(NUM_OF_FILES, std::vector<std::string>(0));

typedef struct node{
    std::string key; 
    int value;
    struct node* next;
} node;

typedef struct hashtable{
    node ** entries;
} hashtable;

hashtable* table[NUM_OF_FILES + 1];


unsigned int calculateHash(const std::string s){
    std::hash<std::string> hash_fn;
    unsigned int num = (unsigned int) hash_fn(s) % TABLE_SIZE;
    return num;
}

void createTable(void){
    for(int i = 0; i < NUM_OF_FILES; i++){
        hashtable * item = new hashtable;
        node** entries = new node*[TABLE_SIZE];
        item->entries = entries;
        for(int j = 0; j < TABLE_SIZE; j++){
            item->entries[j] = nullptr;
        }
        table[i] = item;
    }
}

void deleteTable(void){
   for(int i = 0; i < NUM_OF_FILES; i++){
       for(int j = 0; j < TABLE_SIZE; j++){
            node * ptr = table[i]->entries[j];
            delete(ptr); 
       }
       delete [] table[i]->entries;
   }
}

void printTable(void){
   for(int i = 0; i < NUM_OF_FILES; i++){
       for(int j = 0; j < TABLE_SIZE; j++){
           if(table[i]->entries[j] != nullptr){
               std::cout<<table[i]->entries[j]->key<<" ";
               std::cout<<table[i]->entries[j]->value<<" ";
           }
       }
       std::cout<<std::endl;
   } 
}

void insertTable(const std::string word, int fileIndex){

    node * newNode = new node;
    newNode->key = word;
    newNode->value = 1;
    newNode->next = nullptr;

    unsigned int hashIndex = calculateHash(word);

    #pragma omp critical
    {
        if(table[fileIndex]->entries[hashIndex] == nullptr){
            table[fileIndex]->entries[hashIndex] = newNode;        
        }
        else{
            node * ptr = table[fileIndex]->entries[hashIndex];  

            while(ptr && ptr->key != word){
                ptr = ptr->next;
            }
            if(ptr != nullptr){
                ptr->value += 1;
            }
            else{
                node * ptr = table[fileIndex]->entries[hashIndex];   
                newNode->next = ptr;
                ptr = newNode; 
            }
        }
    }
}

bool isNotAlpha(char c){
    return isalpha(c) == 0;
}
void removePunctuation(std::string &s){
    s.erase(remove_if(s.begin(), s.end(), isNotAlpha), s.end());
}

std::string formatWord(std::string input){

    //Remove punctuation and keep only alphabets
    removePunctuation(input);
    std::string output;
    output.resize(input.size());

    //Transform uppercase to lowercase
    std::transform(input.begin(),
                 input.end(),
                 output.begin(),
                 ::tolower);
    return output;
}

void mapper(int fileIndex){
    int len = words[fileIndex].size();
    for(int i = 0; i < len; i++){            
        insertTable(words[fileIndex][i],fileIndex);
    }
}

void reducer(void){
    for(int i = 0; i < NUM_OF_FILES; i++){
        for(int j = 0; j < TABLE_SIZE; j++){
            if(table[i]->entries[j] != nullptr){
                globalMap[table[i]->entries[j]->key]+=table[i]->entries[j]->value;
            }
        }
    }
}

void printMap(){
    std::fstream fptr;
    fptr.open("./mapreduce_word_count.txt");
    long wordCount = 0;
    std::map<std::string, int>::iterator it;
    for(it = globalMap.begin(); it != globalMap.end(); it++){
        // std::string line = "Key: " + it->first + ", Value: " + std::to_string(it->second) + "\n";
        std::string line = it->first + " " + std::to_string(it->second) + "\n";
        fptr << line; 
        wordCount++;
    }
    fptr.close();
    // std::cout<<std::endl;
    // std::cout<<"Total words: "<<wordCount<<std::endl;
}

void readFile(std::string filename, int fileIndex){
    // std::cout<<filename;
    std::fstream fptr;
    fptr.open(filename);
    std::string word;
    while(fptr >> word){
        word = formatWord(word);
        words[fileIndex].push_back(word);
    }
    fptr.close();
}

int main(int argc, char const *argv[])
{

    std::fstream fptr;
    fptr.open("./file_list.txt");

    std::string file[NUM_OF_FILES];
    createTable();

    clock_t start,end;
	start = clock();

    //Read file_list.txt for file names 
    for(int i = 0; i < NUM_OF_FILES; i++ ){
        std::string line;
        std::getline(fptr, line);
        file[i] = line;
    }

    for(int i = 0; i < NUM_OF_FILES; i++)
        readFile(file[i], i);

    #pragma omp parallel for
    for(int i = 0; i < NUM_OF_FILES; i++){
        mapper(i);
    }

    reducer();

    printMap();

    end = clock();
    double time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Time taken: %f sec\n", time);
    //free memory allocated in free store
    deleteTable();
    fptr.close();
    
    return 0;
}
