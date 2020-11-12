#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>
#include <cctype>


#define NUM_OF_FILES 20 

std::map<std::string, int> globalMap;

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

    //Transofrm uppercase to lowercase
    std::transform(input.begin(),
                 input.end(),
                 output.begin(),
                 ::tolower);
    return output;
}

void addToMap(const std::string word){
    if(word.length() > 0){
        globalMap[word] += 1;
    }
}

void printMap(){
    std::fstream fptr;
    fptr.open("./naive_word_count.txt");

    std::map<std::string, int>::iterator it;
    for(it = globalMap.begin(); it != globalMap.end(); it++){
        // std::string line = "Key: " + it->first + ", Value: " + std::to_string(it->second) + "\n";
        std::string line = it->first + " " + std::to_string(it->second) + "\n";
        fptr << line; 
    }
    fptr.close();
}

void readFile(std::string filename){
    std::cout<<filename;
    std::fstream fptr;
    fptr.open(filename);
    std::string word;
    int numOfWords = 0;
    while(fptr >> word){
        word = formatWord(word);
        addToMap(word);
    }
    fptr.close();
}

int main(int argc, char const *argv[])
{

    std::fstream fptr;
    fptr.open("./file_list.txt");

    std::string file[NUM_OF_FILES];

    //Read files 
    for(int i = 0; i < NUM_OF_FILES; i++ ){
        std::string line;
        std::getline(fptr, line);
        std::cout<<line<<std::endl;
        file[i] = line;
    }

    for(int i = 0; i < NUM_OF_FILES; i++){
        readFile(file[i]);
    }
    printMap();
    fptr.close();
    
    
    return 0;
}
