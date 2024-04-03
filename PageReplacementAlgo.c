#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>
//Name: Page Replacement Algorithm
//Author: Sasha Greene
//School: University of Lethbridge
//Course: Operating Systems


/*
Assignment #3 Operating Systems | Spring 2024
Page Replacement Algorithms
Write a C program that implements the FIFO LRU, and optimal (OPT) page-replacement algorithms. Have your
program initially generate a random page-reference string (of length 100) where page numbers range from 0 to 9.
Apply the generated random page-reference string to each algorithm and record the number of page faults incurred.
Pass the number of frames and the seed used to generate the page reference string, as arguments to the program
(see below for examples).
Use the following reference string whenever the seed parameter is not provided.
2 8 7 2 0 3 1 7 3 1 9 3 6 1 8 4 5 1 8 8 3 4 4 3 4 2 0 6 6 7 0 1 0 9 3 6 4 6 8 0 4 6 2 3 5 7 8 0 3 2 0 0 0 4 6 9 1 4 3 8 8 0
0 9 7 0 7 0 9 7 7 3 8 8 9 2 7 2 1 2 0 9 1 1 1 5 0 7 1 4 9 1 9 5 8 4 4 7 9 6
Your program should behave as follows:
1. If only one argument is provided (number of frames), your program uses the test string above:
./a.out 5
Algorithm # Page faults
FIFO 52
LRU 50
OPT 32
2. If two arguments are provided (number of frames and the seed), generate a random string of length 100.
For example if the seed = 1234, we have:
./a.out 5 1234 // with 5 frames, seed = 1234
Algorithm #Page faults
FIFO 49
LRU 48
OPT 33
*/

int fifo(int pages[], int n, int frames);
int lru(int pages[], int n, int frames);
int opt(int pages[], int n, int frames);


int main(int argc, char *argv[]) {
    int frames;
    int seed;
    
    if (argc == 2) {
        frames = atoi(argv[1]);
        seed = -1; 
    } else if (argc == 3) {
        frames = atoi(argv[1]);
        seed = atoi(argv[2]);

    } else {
        printf("Incorrect amount of arguments!\n");
        return 1;
    }
    

    if(seed == -1){
        int defaultArray[] = {2, 8, 7, 2, 0, 3, 1, 7, 3, 1, 9, 3, 6, 1, 8, 4, 5, 1, 8, 8, 3, 4, 4, 3, 4, 2, 0, 6, 6, 7, 0, 1, 0, 9, 3, 6, 4, 6, 8, 0, 4, 6, 2, 3, 5, 7, 8, 0, 3, 2, 0, 0, 0, 4, 6, 9, 1, 4, 3, 8, 8, 0, 0, 9, 7, 0, 7, 0, 9, 7, 7, 3, 8, 8, 9, 2, 7, 2, 1, 2, 0, 9, 1, 1, 1, 5, 0, 7, 1, 4, 9, 1, 9, 5, 8, 4, 4, 7, 9, 6};
   
        printf("Algorithm\t# Page faults\n");
        printf("FIFO\t\t%d\n", fifo(defaultArray, 100, frames));
        printf("LRU\t\t%d\n", lru(defaultArray, 100, frames));
        printf("OPT\t\t%d\n", opt(defaultArray, 100, frames));
    }else{
         int array[100];
        //printf("DEBUG Seed: %d\n",seed);
        //printf("ARRAY: {");
        srand(seed);
        for (int i = 0; i < 100; i++) {
            array[i] = rand() % 10; // Generate random number between 0 and 9
           // printf("%d,", array[i]);
        }

       // printf("}\n");
        printf("Algorithm\t# Page faults\n");
        printf("FIFO\t\t%d\n", fifo(array, 100, frames));
        printf("LRU\t\t%d\n", lru(array, 100, frames));
        printf("OPT\t\t%d\n", opt(array, 100, frames));


    }
    return 0;
}

int fifo(int pages[], int pageCount, int frameCount) {
   //printf("Fifo: %d, %d, %d, %d\n", pageCount, frameCount, pages[0], pages[pageCount-1]);
    int faults = 0;
    int cache[frameCount];
    
    for(int i = 0; i < frameCount; i++){
        cache[i]=-1;
        //printf("Cache: %d, ", cache[i]);
    }
    //printf("\nCache Initialized:\n");

    //loop through all of the pages
    for(int i = 0; i < pageCount; i++){
        
        //Display the cache
        /*
        for(int k= 0; k < frameCount; k++){
                 printf("Cache: %d, ", cache[k]);
        }
        */

        //printf("\nChecking %d", pages[i]);

        //effectively a bool to check whether we find the value in our cache or not
        int isAlreadyInCache= 0;

        //loop through the cache and see iif its already in the cache
        for(int j = 0; j < frameCount; j++){
            //printf("Checking: %d, %d ", cache[j], pages[i]);
           if(cache[j]==pages[i]){
                isAlreadyInCache = 1;
                //if we found in the cache, no need to check the rest of the cache
                break;
           }
        }

        //already in queue
        if(isAlreadyInCache == 1){
            //not a page fault, do nothing
             //printf("No Fault\n");
        }else{
            //page fault
            //printf("Fault\n");
            faults++;
            //first shift all elements up one
            for(int k = 0; k < frameCount-1; k++){
                cache[k]=cache[k+1];
            }
            //push new value to rear
            cache[frameCount-1] = pages[i];
        }
    }

    return faults;
}

int lru(int pages[], int pageCount, int frameCount) {
   //printf("LRU: %d, %d, %d, %d\n", pageCount, frameCount, pages[0], pages[pageCount-1]);
    int faults = 0;
    int cache[frameCount];
    
    for(int i = 0; i < frameCount; i++){
        cache[i]=-1;
        //printf("Cache: %d, ", cache[i]);
    }
   //printf("\nCache Initialized:\n");

    //loop through all of the pages
    for(int i = 0; i < pageCount; i++){
        
        //Display the cache
        for(int k= 0; k < frameCount; k++){
                 //printf("Cache: %d, ", cache[k]);
        }
      //  printf("\nChecking %d", pages[i]);

        //effectively a bool to check whether we find the value in our cache or not
        int isAlreadyInCache= 0;

        //loop through the cache and see iif its already in the cache
        for(int j = 0; j < frameCount; j++){
            //printf("Checking: %d, %d ", cache[j], pages[i]);
           if(cache[j]==pages[i]){
                isAlreadyInCache = 1;
                //if we found in the cache, no need to check the rest of the cache
                break;
           }
        }

        //already in queue
        if(isAlreadyInCache == 1){
            //not a page fault, update most recently used.

            //keep swapping out the desired element until it is the most recently used.
            for(int k = 0; k < frameCount-1; k++){
                if(cache[k]==pages[i]){
                    int temp = cache[k];
                    cache[k]=cache[k+1];
                    cache[k+1] = temp;
                }
            }


             //printf("No Fault\n");
        }else{
            //page fault
            //printf("Fault\n");
            faults++;
            //first shift all elements up one
            for(int k = 0; k < frameCount-1; k++){
                cache[k]=cache[k+1];
            }
            //push new value to rear
            cache[frameCount-1] = pages[i];
        }
    }
    return faults;
}

int opt(int pages[], int pageCount, int frameCount) {
    //printf("OPT: %d, %d, %d, %d\n", pageCount, frameCount, pages[0], pages[pageCount-1]);
    int faults = 0;
    int cache[frameCount];
    
    for(int i = 0; i < frameCount; i++){
        cache[i]=-1;
    }

    //loop through all of the pages
    for(int i = 0; i < pageCount; i++){
        
        //effectively a bool to check whether we find the value in our cache or not
        int isAlreadyInCache= 0;

        //loop through the cache and see iif its already in the cache
        for(int j = 0; j < frameCount; j++){
           // printf("Checking: %d, %d ", cache[j], pages[i]);
           if(cache[j]==pages[i]){
                isAlreadyInCache = 1;
                //if we found in the cache, no need to check the rest of the cache
                break;
           }
        }

        //already in queue
        if(isAlreadyInCache == 1){
            //not a page fault, do nothing
            //printf("No Fault\n");
        }else{
            //page fault

            //printf("Fault\n");
            
            faults++;
            
            int furthestIndex = -1;
            int furthestDistance = -1;
            //loop through cache to find the element in the cache that will be used farthest into the future
            for(int k = 0; k < frameCount; k++){
                //if theres an uninitialized cache position, just use that!
                if(cache[k] == -1){

                    furthestIndex = k;
                    break;
                }
                //only check future
                int distance = 0;
                //loop through all remaining pages to see how far into the future the element is found
                for(int l = i+1; l <pageCount; l++){
                    //increment distance
                    distance++;
                    if(pages[l] == cache[k]){
                        break;
                    }

                    //if lower, replace highest.
                    if(distance > furthestDistance){
                        furthestDistance = distance;
                        furthestIndex = k;
                    }
                }
            }
            
            //replace our cache at lowest index with the new 
            cache[furthestIndex] = pages[i];
        }
    }

    return faults;
}



    
