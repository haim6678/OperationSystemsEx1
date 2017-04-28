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
    while ((checkMatch != 3) && (readFromFirst > 0) && (readFromSecond > 0)) {

        //check if need to read from first file
        if (firstCurPosition == readFromFirst) {
            //read from first file
            readFromFirst = fread(buff1, Size, 1, first);
            firstCurPosition = 0;
        }
        //check if need to read from second file
        if (secondCurPosition == readFromSecond) {
            //read from second file
            readFromSecond = fread(buff2, Size, 1, second);
            secondCurPosition = 0;
        }

        //compare what is currently in the buufs
        checkMatch = checkBuffs(buff1, buff2, &firstCurPosition,
                                &secondCurPosition, readFromFirst, readFromSecond);


    }

    /*if you finished reading then check if's because we got a un match character
    /or because we finished with one file and there is still more to read
    /from the other*/
    if (checkMatch != 3) {
        //we finished with the first
        if (readFromFirst == 0) {
            //check if there anything in second
            if (readFromSecond != 0) {
                //handle the rest of the file
                checkMatch = HandleRemainFile(second, buff2, secondCurPosition, readFromSecond);
            }
            //we finished with the second
        } else if (readFromSecond == 0) {
            //check if there anything in first
            if (readFromFirst != 0) {
                //handle the rest of the file
                checkMatch = HandleRemainFile(first, buff1, firstCurPosition, readFromFirst);
            }
        }
    }
    //finish the program and release resources
    close(first);
    close(second);
    exit(checkMatch);
}

/**
 * the func that handle the rest of one file
 * if we finished with the other
 * @param file - the file to handle
 * @param buff - the buff we read to
 * @param currPosInBuff - current location in buff
 * @param readFromFile - how much was the last read
 * @return
 */
int HandleRemainFile(FILE *file, char *buff, int currPosInBuff, ssize_t readFromFile) {
    char temp;
    int res = 1;

    //clear what's left in the buffer
    while (currPosInBuff < readFromFile) {
        temp = buff[currPosInBuff];

        //check the character
        if (isspace(temp) == 0) {
            res = 3;
        } else {
            res = 2;
        }
        currPosInBuff++;
    }

    //handle the rest of the file
    //read more
    currPosInBuff = 0;
    readFromFile = fread(buff, Size, 1, file);
    //handle what we read
    while (readFromFile > 0) {
        //run all over the buffer and check
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
/**
 * a function that compares 2 buffers.
 * @param first - first buffer
 * @param sec - sec buffer
 * @param firstPos - location in first
 * @param secPos - location in sec
 * @param firstSize -amount read from first
 * @param secSize -amount read from sec
 * @return
 */
int checkBuffs(char *first, char *sec, int *firstPos, int *secPos, ssize_t firstSize, ssize_t secSize) {

    int result = 1;
    //run the buffers until one ends or we got a strike
    while ((*firstPos < firstSize) && (*secPos < secSize) &&(result !=3)) {

        //check if un matching characters
        if (first[*firstPos] != sec[*secPos]) {
            //check capital letter
            if ((first[*firstPos] >= 97) && (first[*firstPos] <= 122) && (first[*firstPos] == sec[*secPos] + 32) ||
                ((first[*firstPos] >= 65) && first[*firstPos] <= 90) && (first[*firstPos] == sec[*secPos] - 32)) {
                ++(*firstPos);
                ++(*secPos);
                result = 2;
            //it's not a capital
            } else {
                //first ia space
                if (isspace(first[*firstPos])) {

                    ++(*firstPos);
                    result = 2;
                    continue;
                }
                //second is space
                if (isspace(sec[*secPos])) {

                    ++(*secPos);
                    result = 2;
                    continue;
                }
                result = 3;
                break;
            }
        }

    }
    return result;
}



