#include<map>
#include<bits/stdc++.h>
#include<unordered_map>
#include<string>
#include <sys/mman.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <wait.h>
std::unordered_map<std::string,void*> resource_Map;

int main(){

    std::vector<std::string> vec={"a.txt","b.txt"};
    for(auto i:vec){
        int fd=open(i.c_str(),O_RDONLY);
        int size = lseek(fd, 0, SEEK_END); 
        void* ptr=mmap(NULL,size,PROT_READ,MAP_SHARED,fd,0);
        resource_Map[i]=ptr;
    }


    // int fd1 = open("a.txt", O_RDONLY);
    // int size1 = lseek(fd1, 0, SEEK_END); 
    // int fd2=open("b.txt",O_RDONLY);
    // int size1 = lseek(fd2, 0, SEEK_END); 

    // void *ptr1=mmap(NULL,)
    




    return 0;
}