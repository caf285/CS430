#include <stdio.h>
#include <stdlib.h>

// ================================================================================(read and built header)
void ppmHeader(FILE *fh, int c, char *writeBuffer){
    
    *writeBuffer++ = c;                             // save output format in headerBuffer
    
    while (1){                                      // save comments to header
        c = fgetc(fh);
        *writeBuffer++ = c;
        if (c == 10){                               // if new line doesn't start with comment, BREAK
            c = fgetc(fh);
            *writeBuffer++ = c;
            if (c != 35){
                break;
            }
        }
    }
    
    int spaceTrigger = 0;                           // save x and y to headerBuffer
    while (1){
        if (c == 32){                               // if space detected, set trigger to read rest of line
            spaceTrigger = 1;
        }
        c = fgetc(fh);
        *writeBuffer++ = c;
        if (c == 10 && spaceTrigger == 1){          // if x and y on same line, BREAK
            break;
        }
        else if (c == 10 && spaceTrigger == 0){     // if x and y on seperate lines, set trigger for second loop
            spaceTrigger = 1;
        }
    }
    
    while (1){                                      // save alpha to headerBuffer
        c = fgetc(fh);
        *writeBuffer++ = c;
        if (c == 10){
            break;
        }
    }
    *writeBuffer = '\0';                            // set end of headerBuffer
}

// ================================================================================(convert file to P3)
void bufferP3(FILE *fh, char *writeBuffer, int inputNumber){
    if (inputNumber == 51){             // convert P3 to P3
        while (!feof(fh)){              // simply copy file to buffer
            *writeBuffer++ = fgetc(fh);
        }
        *writeBuffer = EOF;
    }else{                              // convert from P6 to P3
        while (!feof(fh)){
            int multiplier = 128;
            int total = 0;
            int c;
            c = fgetc(fh);
            for (int i = 0; i < 8; i++) {           // convert binary data to number
                if (!!((c << i) & 0x80)){
                    total += multiplier;
                }
                multiplier = multiplier / 2;
            }
            multiplier = 100;
            int zeroTrigger = 0;
            for(int i = 0; i < 3; i++){             // finds single digits and copies them to buffer
                int digit = total / multiplier;
                total -= multiplier * digit;
                multiplier = multiplier / 10;
                if (digit > 0){                     // sets trigger if first digit not 0
                    zeroTrigger = 1;
                }
                if (zeroTrigger == 1){              // if not 0, write number to buffer
                    *writeBuffer++ = (char) (digit+48);
                }                                   // if entire number = 0, then just write 0 to buffer
                if (i == 2 && zeroTrigger == 0){
                    *writeBuffer++ = (char) (48);
                }
            }
            *writeBuffer++  = (char) 10;            // write line return to buffer
        }
        *writeBuffer = EOF;                         // write end of file to buffer
    }
}

// ================================================================================(convert file to P6)
void bufferP6(FILE *fh, unsigned char *writeBuffer, int inputNumber){
    if (inputNumber == 54){                             // convert P6 to P6
        while (!feof(fh)){              // copy file to buffer
            *writeBuffer++ = fgetc(fh);
        }
        for (int i = 0; i <= 200; i+=50){ // add code at end of buffer to end when reading later
            *writeBuffer++ = i;
        }
    }else{                                              // convert from P3 to P6
        while (1){
            char *intBuffer1 = malloc(sizeof(char)*3);  // buffer used to read single line, and reset for next line
            char *intBuffer2 = intBuffer1;
            int c = fgetc(fh);
            if (c == EOF)
                break;
            while (c != 10){            // read entire line; if line return detected, BREAK
                *intBuffer2 = c;
                intBuffer2++;
                c = fgetc(fh);
            }
            *writeBuffer++ = atoi(intBuffer1); // copy number to dataBuffer
        }
        for (int i = 0; i <= 200; i+=50){   // add code at end of buffer to end when reading later
            *writeBuffer++ = i;
        }
    }
}

// ================================================================================(main)
int main(int argc, const char * argv[]) {
    
    // ==================================================(open & test file)
    FILE *fh = fopen(argv[1], "rw");                            // open file
    char *headerBuffer = malloc(sizeof(char)*50);               // create header buffer
    
    if (fh == NULL) {                                           // ERROR if file doesn't exists
        fprintf(stderr, "Error: File doesn't exist.\n");
        return 1;
    }
    
    int c = fgetc(fh);                                          // read first character
    if (c != 80) {                                              // ERROR if first char is not 'P'
        fprintf(stderr, "Error: This is not a PPM file.\n");
        return 1;
    }
    
    char *writeBuffer;                                          // create node to traverse headerBuffer
    writeBuffer = headerBuffer;
    *writeBuffer++ = c;                                         // assign 'P' to header
    c = fgetc(fh);                                              // get next character
    int inputNumber = c;                                        // save input file type to check conversion requirement
    c = atoi(argv[3]) + 48;                                     // save output file type to add to headerBuffer
    
    // ==================================================(build header)
    ppmHeader(fh, c, writeBuffer);                              // build header (file path, output format, headerBuffer)
    
    // ==================================================(build P3 data)
    
    if (atoi(argv[3]) == 3){                                        // convert to P3
        char *dataBuffer = malloc(sizeof(char)*10000000);   // create buffer for data
        writeBuffer = dataBuffer;
        
        bufferP3(fh, writeBuffer, inputNumber);             // build dataBuffer with P3 data
        
        FILE *newFH = fopen(argv[2], "w+");                 // open output file
        while(*headerBuffer){                               // write header to file
            printf("%c", *headerBuffer);
            fputc(*headerBuffer++, newFH);
        }
        while(*dataBuffer != EOF){                          // write data to file
            printf("%c", *dataBuffer);
            fputc(*dataBuffer++, newFH);
        }
        fclose(newFH);                                      // close output file
        
        // ==============================================(build P3 data)
        
    }else if (atoi(argv[3]) == 6){                                  // convert to P6
        unsigned char *dataBuffer = malloc(sizeof(char)*10000000);  // create data buffer
        unsigned char *writeBuffer2 = dataBuffer;
        
        bufferP6(fh, writeBuffer2, inputNumber);            // build dataBuffer with P6 data
        
        FILE *newFH = fopen(argv[2], "w+");                 // open output file
        while(*headerBuffer){                               // write header to file
            printf("%c", *headerBuffer);
            fputc(*headerBuffer++, newFH);
        }
        int count = 0;
        while(1){                                           // write data to file
            printf("%c", *dataBuffer);
            fputc(*dataBuffer++, newFH);
            if(*dataBuffer == count){
                count += 50;
            }else{
                count = 0;
            }
            if (count == 200)
                break;
        }
        fclose(newFH);                                      // close output file
        
    }else{                                                      // ERROR if wrong output format chosen
        fprintf(stderr, "Error: wrong PPM format.\n");
        return 1;
    }
    
    fclose(fh);                                                 // close file
    return 0;
}
