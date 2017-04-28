#include <stdio.h>
#include <memory.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

#define Size 100

/**
 * the main function.
 * compares the 2 given files.
 * @param argc - number of args for main
 * @param argv - the args
 * @return - hte compare result.
 */
int main(int argc, char *argv) {

    //declare variables
    int checkMatch = 1;
    int i = 0;
    ssize_t readFromFirst = 1;
    ssize_t readFromSecond = 1;
    int firstCurPosition = Size;
    int secondCurPosition = Size;
    char buff1[Size + 1];
    char buff2[Size + 1];

    //check we got correct arguments
    if (argc < 3) {
        perror("not enough arguments");
        exit(-1);
    }

    //open first file
    FILE *first = fopen(argv[1], "r");
    if (!first) {
        perror("failed to open file");
        exit(-1);
    }

    //open second file
    FILE *second = fopen(argv[2], "r");
    if (!second) {
        perror("failed to open file");
        exit(-1);
    }

    //initialize the file to beginning
    int seek1 = fseek(first, SEEK_SET, 0);
    int seek2 = fseek(second, SEEK_SET, 0);

    //compare the files
    while ((checkMatch != 3) && (readFromFirst > 0) && (readFromSecond > 0)) { //todo 0??

        if (firstCurPosition == readFromFirst) {
            readFromFirst = fread(buff1, Size, 1, first);
            firstCurPosition = 0;
        }
        if (secondCurPosition == readFromSecond) {
            readFromSecond = fread(buff2, Size, 1, second);
            secondCurPosition = 0;
        }

        checkMatch = checkBuffs(buff1, buff2, &firstCurPosition,
                                &secondCurPosition, readFromFirst, readFromSecond);


    }

    if (checkMatch != 3) {//todo 0??
        if (readFromFirst == 0) {
            if (readFromSecond != 0) {
                checkMatch = HandleRemainFile(second, buff2, secondCurPosition, readFromSecond);
            }
        } else if (readFromSecond == 0) {
            if (readFromFirst != 0) {
                checkMatch = HandleRemainFile(first, buff1, firstCurPosition, readFromFirst);
            }
        }
    }
    close(first);
    close(second);
    exit(checkMatch);
}

int HandleRemainFile(FILE *file, char *buff, int currPosInBuff, ssize_t readFromFile) {
    char temp;
    int res = 1;
    //clear what's left in the buffer
    while (currPosInBuff < readFromFile) {
        temp = buff[currPosInBuff];

        if (isspace(temp) == 0) {
            res = 3;
        } else {
            res = 2;
        }
        currPosInBuff++;
    }

    currPosInBuff = 0;
    readFromFile = fread(buff, Size, 1, file);
    while (readFromFile > 0) {

        while (currPosInBuff < readFromFile) {
            temp = buff[currPosInBuff];

            if (isspace(temp) == 0) {
                res = 3;
            } else {
                res = 2;
            }
            currPosInBuff++;
        }
        currPosInBuff = 0;
        readFromFile = fread(buff, Size, 1, file);
    }
    return res;
}

int checkBuffs(char *first, char *sec, int *firstPos, int *secPos, ssize_t firstSize, ssize_t secSize) {

    int result = 1;
    while ((*firstPos < firstSize) && (*secPos < secSize)) {

        if (first[*firstPos] != sec[*secPos]) {
            if ((first[*firstPos] >= 97) && (first[*firstPos] <= 122) && (first[*firstPos] == sec[*secPos] + 32) ||
                ((first[*firstPos] >= 65) && first[*firstPos] <= 90) && (first[*firstPos] == sec[*secPos] - 32)) {
                ++(*firstPos);
                ++(*secPos);
                result = 2;

            } else {
                if (isspace(first[*firstPos])) {

                    ++(*firstPos);
                    result = 2; //todo 2?
                    continue;
                }
                if (isspace(sec[*secPos])) {

                    ++(*secPos);
                    result = 2; //todo 2?
                    continue;
                }
                result = 3;
                break;
            }
        }

    }
    return result;
}



