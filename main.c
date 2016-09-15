#include <stdio.h>
#include <stdlib.h>

// ================================================================================(read and built header)
void ppmHeader(FILE *fh, int c, char *writeBuffer){
    
    *writeBuffer++ = c;
    
    while (1){
        c = fgetc(fh);
        *writeBuffer++ = c;
        if (c == 10){
            c = fgetc(fh);
            *writeBuffer++ = c;
            if (c != 35){
                break;
            }
        }
    }
    int spaceTrigger = 0;
    while (1){
        if (c == 32){
            spaceTrigger = 1;
        }
        c = fgetc(fh);
        *writeBuffer++ = c;
        if (c == 10 && spaceTrigger == 1){
            break;
        }
        else if (c == 10 && spaceTrigger == 0){
            spaceTrigger = 1;
        }
    }
    while (1){
        c = fgetc(fh);
        *writeBuffer++ = c;
        if (c == 10){
            break;
        }
    }
    *writeBuffer = '\0';
}

// ================================================================================(convert file to P3)
void bufferP3(FILE *fh, char *writeBuffer, int inputNumber){
    if (inputNumber == 51){              // convert P3 to P3
        while (!feof(fh)){
            *writeBuffer++ = fgetc(fh);
        }
        *writeBuffer = EOF;
    }else{                              // convert from P6 to P3
        while (!feof(fh)){
            int multiplier = 128;
            int total = 0;
            int c;
            c = fgetc(fh);
            for (int i = 0; i < 8; i++) {
                if (!!((c << i) & 0x80)){
                    total += multiplier;
                }
                multiplier = multiplier / 2;
            }
            multiplier = 100;
            int zeroTrigger = 0;
            for(int i = 0; i < 3; i++){
                int digit = total / multiplier;
                total -= multiplier * digit;
                multiplier = multiplier / 10;
                if (digit > 0){
                    zeroTrigger = 1;
                }
                if (zeroTrigger == 1){
                    *writeBuffer++ = (char) (digit+48);
                }
                if (i == 2 && zeroTrigger == 0){
                    *writeBuffer++ = (char) (48);
                }
            }
            *writeBuffer++  = (char) 10;
        }
        *writeBuffer = EOF;
    }
}

// ================================================================================(convert file to P6)
void bufferP6(FILE *fh, unsigned char *writeBuffer, int inputNumber){
    if (inputNumber == 54){              // convert P6 to P6
        while (!feof(fh)){
            *writeBuffer++ = fgetc(fh);
        }
        for (int i = 0; i <= 200; i+=50){
            *writeBuffer++ = i;
        }
    }else{                              // convert from P3 to P6
        while (1){
            char *intBuffer1 = malloc(sizeof(char)*3);
            char *intBuffer2 = intBuffer1;
            int c = fgetc(fh);
            if (c == EOF)
                break;
            while (c != 10){
                *intBuffer2 = c;
                
                intBuffer2++;
                c = fgetc(fh);
            }
            *writeBuffer++ = atoi(intBuffer1);
        }
        for (int i = 0; i <= 200; i+=50){
            *writeBuffer++ = i;
        }
    }
}

// ================================================================================(combine header and data to build file)
void buildFile(char *headerBuffer, char *dataBuffer, char *outputFile){
    FILE *newFH = fopen(outputFile, "w+");
    
    while(*headerBuffer){
        fputc(*headerBuffer++, newFH);
    }
    while(*dataBuffer != EOF){
        fputc(*dataBuffer++, newFH);
    }
    fclose(newFH);
}

// ================================================================================(main)
int main(int argc, const char * argv[]) {
    
    // ==================================================(open & test file)
    FILE *fh = fopen(argv[1], "rw");                            // open file
    char *headerBuffer = malloc(sizeof(char)*50);               // create header buffer
    
    if (fh == NULL) {                                           // check if file exists
        fprintf(stderr, "Error: File doesn't exist.\n");
        return 1;
    }
    
    int c = fgetc(fh);                                          // read first character
    if (c != 80) {                                              // check if first char is 'P'
        fprintf(stderr, "Error: This is not a PPM file.\n");
        return 1;
    }
    
    char *writeBuffer;
    writeBuffer = headerBuffer;
    *writeBuffer++ = c;                                         // assign 'P' to header
    c = fgetc(fh);                                              // get next character
    int inputNumber = c;
    c = atoi(argv[3]) + 48;
    
    // ==================================================(build header)
    ppmHeader(fh, c, writeBuffer);
    
    // ==================================================(build data)
    
    if (atoi(argv[3]) == 3){
        char *dataBuffer = malloc(sizeof(char)*10000000);
        writeBuffer = dataBuffer;
        bufferP3(fh, writeBuffer, inputNumber);
        
        
        FILE *newFH = fopen(argv[2], "w+");
        
        while(*headerBuffer){
            fputc(*headerBuffer++, newFH);
        }
        while(*dataBuffer != EOF){
            fputc(*dataBuffer++, newFH);
        }
        fclose(newFH);
        
        
    }else if (atoi(argv[3]) == 6){
        unsigned char *dataBuffer = malloc(sizeof(char)*10000000);
        unsigned char *writeBuffer2 = dataBuffer;
        bufferP6(fh, writeBuffer2, inputNumber);
        
        FILE *newFH = fopen(argv[2], "w+");
        
        while(*headerBuffer){
            fputc(*headerBuffer++, newFH);
        }
        int count = 0;
        while(1){
            fputc(*dataBuffer++, newFH);
            if(*dataBuffer == count){
                count += 50;
            }else{
                count = 0;
            }
            if (count == 200)
                break;
        }
        fclose(newFH);
        
        
    }else{
        fprintf(stderr, "Error: wrong PPM format.\n");
        return 1;
    }
    
    fclose(fh);
    return 0;
}
