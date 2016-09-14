#include <stdio.h>
#include <stdlib.h>

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

void bufferP3(FILE *fh, char *writeBuffer, int inputNumber){
    if (inputNumber == 51){              // convert to self
        while (!feof(fh)){
            *writeBuffer++ = fgetc(fh);
        }
        *--writeBuffer = '\0';
    }else{                              // convert from P3 to P6
        
    }
}

void bufferP6(FILE *fh, char *writeBuffer, int inputNumber){
    if (inputNumber == 54){              // convert to self
        while (!feof(fh)){
            *writeBuffer++ = fgetc(fh);
        }
        *--writeBuffer = '\0';
    }else{                              // convert from P6 to P3
        
    }
}

void buildFile(char *headerBuffer, char *dataBuffer, char *outputFile){
    FILE *newFH = fopen(outputFile, "w+");
    while(*headerBuffer){
        printf("%c", *headerBuffer);
        fputc(*headerBuffer++, newFH);
    }
    while(*dataBuffer ){
        printf("%c", *dataBuffer);
        fputc(*dataBuffer++, newFH);
    }
    fclose(newFH);
}

int main(int argc, const char * argv[]) {
    
    // ==================================================(open & test file)
    FILE *fh = fopen(argv[1], "rw");                            // open file
    char *headerBuffer = malloc(sizeof(char)*50);               // create header buffer
    char *dataBuffer = malloc(sizeof(char)*10000000);
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
    
    // ==================================================(build header)
    ppmHeader(fh, c, writeBuffer);
    
    // ==================================================(build data)
    writeBuffer = dataBuffer;
    if (atoi(argv[3]) == 3){
        bufferP3(fh, writeBuffer, inputNumber);
    }else if (atoi(argv[3]) == 6){
        bufferP6(fh, writeBuffer, inputNumber);
    }else{
        fprintf(stderr, "Error: wrong PPM format.\n");
        return 1;
    }
    
    // ==================================================(write new file data)
    buildFile(headerBuffer, dataBuffer, argv[2]);
        fclose(fh);
    
    // ==================================================(print header)
    //writeBuffer = headerBuffer;
    //while(*writeBuffer){
    //    printf("%c", *writeBuffer++);
    //}
    
    //FILE *newFH = fopen(argv[2], "w+");
    //writeBuffer = dataBuffer;
    //while (c != EOF){
    //    c = fgetc(fh);
    //    *writeBuffer++ = c;
    //    for (int i = 0; i < 8; i++) {
    //        printf("%d", !!((c << i) & 0x80));
    //    }
    //    printf("\n");
    //printf("%c", c);
    //}

    return 0;
}
