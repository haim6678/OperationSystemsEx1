/******************************************
*
Student name: Haim Rubinstein
*
Student ID: 203405386
*
Course Exercise Group:01
*
Exercise name:Ex11
******************************************/

#include <stdio.h>
#include <memory.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define SIZE 100
#define READ_FAILED "failed reading from file"
#define FAILED_CLOSE "failed closing file"

//declare the functions
void CheckBuffs(char *first, char *sec, int *firstPos, int *secPos,
                ssize_t firstSize, ssize_t secSize, int *result);

void HandleRemainFile(int file, char *buff, int currPosInBuff,
                      ssize_t readFromFile, int *result);

/********************************************
 * the main function.
 * compares the 2 given files.
 * @param argc - number of args for main
 * @param argv - the args
 * @return - hte compare result.
 *******************************************/
int main(int argc, char *argv[]) {

    //declare variables
    int checkMatch = 1;
    int first;
    int second;
    ssize_t readFromFirst = 0;
    ssize_t readFromSecond = 0;
    int firstCurPosition = 0;
    int secondCurPosition = 0;
    char buff1[SIZE + 1];
    char buff2[SIZE + 1];

    //check we got correct arguments
    if (argc < 3) {
        perror("not enough arguments");
        exit(-1);
    }

    //open first file
    first = open(argv[1], O_RDONLY);
    if (first < 0) {
        perror("failed to open file");
        exit(-1);
    }

    //open second file
    second = open(argv[2], O_RDWR);
    if (second < 0) {
        perror("failed to open file");
        exit(-1);
    }


    //initialize the file to beginning
    off_t seek = lseek(first, 0, SEEK_SET);
    off_t seek2 = lseek(second, 0, SEEK_SET);
    if ((seek < 0) || (seek2 < 0)) {
        perror("failed seeking");
        exit(-1);
    }


    //compare the files
    do {
        //check if need to read from first file
        if (firstCurPosition == readFromFirst) {
            //read from first file
            memset(buff1, 0, SIZE);
            readFromFirst = read(first, buff1, SIZE);
            if (readFromFirst < 0) {
                write(2, READ_FAILED, strlen(READ_FAILED));
                exit(-1);
            }
            firstCurPosition = 0;
        }
        //check if need to read from second file
        if (secondCurPosition == readFromSecond) {
            //read from second file
            memset(buff2, 0, SIZE);
            readFromSecond = read(second, buff2, SIZE);
            if (readFromSecond < 0) {
                write(2, READ_FAILED, strlen(READ_FAILED));
                exit(-1);
            }
            secondCurPosition = 0;
        }

        if ((readFromFirst > 0) && (readFromSecond > 0)) {
            //compare what is currently in the buffs
            CheckBuffs(buff1, buff2, &firstCurPosition,
                       &secondCurPosition, readFromFirst, readFromSecond, &checkMatch);
        }

    } while (((checkMatch != 3) && (readFromFirst > 0) && (readFromSecond > 0)));


    /*if you finished reading then check if's because we got a un match character
    /or because we finished with one file and there is still more to read
    /from the other*/
    if (checkMatch != 3) {
        //we finished with the first
        if (readFromFirst == 0) {
            //check if there anything in second
            if (readFromSecond != 0) {
                //handle the rest of the file
                HandleRemainFile(second, buff2, secondCurPosition,
                                 readFromSecond, &checkMatch);
            }
            //we finished with the second
        } else if (readFromSecond == 0) {
            //check if there anything in first
            if (readFromFirst != 0) {
                //handle the rest of the file
                HandleRemainFile(first, buff1, firstCurPosition,
                                 readFromFirst, &checkMatch);
            }
        }
    }
    //finish the program and release resources
    if (close(first) < 0) {
        write(2, FAILED_CLOSE, strlen(FAILED_CLOSE));
        exit(-1);
    }
    if (close(second) < 0) {
        write(2, FAILED_CLOSE, strlen(FAILED_CLOSE));
        exit(-1);
    }
    return checkMatch;
}

/********************************************
 * the func that handle the rest of one file
 * if we finished with the other
 * @param file - the file to handle
 * @param buff - the buff we read to
 * @param currPosInBuff - current location in buff
 * @param readFromFile - how much was the last read
 *******************************************/
void HandleRemainFile(int file, char *buff, int currPosInBuff,
                      ssize_t readFromFile, int *result) {

    char temp;
    //clear what's left in the buffer
    while (currPosInBuff < readFromFile) {
        temp = buff[currPosInBuff];

        //check the character
        if (isspace(temp) == 0) {
            *result = 3;
            return;
        } else {
            *result = 2;
        }
        currPosInBuff++;
    }

    //handle the rest of the file
    //read more
    currPosInBuff = 0;
    readFromFile = read(file, buff, SIZE);
    //handle what we read
    while (readFromFile > 0) {
        //run all over the buffer and check
        while (currPosInBuff < readFromFile) {
            temp = buff[currPosInBuff];

            if (isspace(temp) == 0) {
                *result = 3;
                return;
            } else {
                *result = 2;
            }
            currPosInBuff++;
        }
        currPosInBuff = 0;
        readFromFile = read(file, buff, SIZE);

        if (readFromFile < 0) {
            write(2, READ_FAILED, strlen(READ_FAILED));
            exit(-1);
        }
    }

}

/********************************************
 * a function that compares 2 buffers.
 * @param first - first buffer
 * @param sec - sec buffer
 * @param firstPos - location in first
 * @param secPos - location in sec
 * @param firstSIZE -amount read from first
 * @param secSIZE -amount read from sec
 *******************************************/
void CheckBuffs(char *first, char *sec, int *firstPos, int *secPos,
                ssize_t firstSize, ssize_t secSize, int *result) {



    //run the buffers until one ends or we got a strike
    while ((*firstPos < firstSize) && (*secPos < secSize)) {

        //check if un matching characters
        if (first[*firstPos] != sec[*secPos]) {
            //check capital letter
            if ((first[*firstPos] >= 97) && (first[*firstPos] <= 122) &&
                (first[*firstPos] == sec[*secPos] + 32) ||
                ((first[*firstPos] >= 65) && first[*firstPos] <= 90) &&
                (first[*firstPos] == sec[*secPos] - 32)) {
                ++(*firstPos);
                ++(*secPos);

                *result = 2;
                //it's not a capital
            } else {
                //first ia space
                if (isspace(first[*firstPos])) {

                    ++(*firstPos);
                    *result = 2;
                    continue;
                }
                //second is space
                if (isspace(sec[*secPos])) {

                    ++(*secPos);
                    *result = 2;
                    continue;
                }
                *result = 3;
                break;
            }
        } else {
            ++(*firstPos);
            ++(*secPos);
        }
    }
}



