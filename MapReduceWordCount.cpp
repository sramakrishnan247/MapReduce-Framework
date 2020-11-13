#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>
#include <cctype>
#include <omp.h>
#include <vector>
#include <cstdio>
#include <cstdlib>

#define NUM_OF_FILES 20 
#define TABLE_SIZE 10000

std::map<std::string, int> globalMap;

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
        // hashtable * item = (hashtable *)malloc(sizeof(hashtable) * 1);
        // item->entries = (node**) malloc(sizeof(node*) * TABLE_SIZE);
        hashtable * item = new hashtable;
        node** entries = new node*[TABLE_SIZE];
        item->entries = entries;
        for(int j = 0; j < TABLE_SIZE; j++){
            item->entries[j] = nullptr;
        }
        table[i] = item;
        std::cout<<table[i];
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

void mapper(const std::string word, int fileIndex){
    if(word.length() > 0){
        insertTable(word, fileIndex);
    }
}

void printMap(){
    std::fstream fptr;
    fptr.open("./mapreduce_word_count.txt");

    for(int i = 0; i < NUM_OF_FILES; i++){
        for(int j = 0; j < TABLE_SIZE; j++){
            if(table[i]->entries[j] != nullptr){
                std::string line  = table[i]->entries[j]->key + " ";
                line += std::to_string(table[i]->entries[j]->value) + "\n";
                fptr << line;
            }
        }
        std::cout<<std::endl;
    } 

    fptr.close();
}

void readFile(std::string filename, int fileIndex){
    std::cout<<filename;
    std::fstream fptr;
    fptr.open(filename);
    std::string word;
    int numOfWords = 0;
    while(fptr >> word){
        word = formatWord(word);
        mapper(word, fileIndex);
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
    //Read files 
    #pragma omp parallel for
    for(int i = 0; i < NUM_OF_FILES; i++ ){
        std::cout<<i<<std::endl; 
        std::string line;
        std::getline(fptr, line);
        std::cout<<line<<std::endl;
        file[i] = line;
    }

    #pragma omp parallel for
    for(int i = 0; i < NUM_OF_FILES; i++){
        readFile(file[i], i);
    }

    #pragma omp barrier 
    printMap();
	end = clock();
    double time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("With gpu - %f sec\n", time);
    fptr.close();
    
    
    printTable();
    deleteTable();
    return 0;
}
