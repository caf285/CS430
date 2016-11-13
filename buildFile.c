// write P6 header to buffer
char* buildHeader(Object** objects, int M, int N){
    char* header = malloc(sizeof(char)*50);
    char* headerNode = header;
    sprintf(headerNode, "P6\n%d\n%d\n255\n", M, N);
    
    return header;
}

// open new file and dump image buffer
void buildFile(char* header, unsigned char* buffer, char* fileName, int M, int N){
    FILE *FH = fopen(fileName, "w+");
    char* headerNode = header;
    unsigned char* bufferNode = buffer;
    
    while (*headerNode){
        fputc(*headerNode++, FH);
    }
    
    for (int i = 0; i < (M * N * 3); i++){
        fputc(*bufferNode++, FH);
    }
    fputc(EOF, FH);
    fclose(FH);
}
