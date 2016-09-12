#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char * argv[]) {
    // insert code here...
    
    FILE *fh = fopen("face.ppm", "rw");
    if (fh == NULL) {
        fprintf(stderr, "Error: File doesn't exist.\n");
    }
    int c = fgetc(fh);
    if (c != 'P') {
        fprintf(stderr, "Error: This is not a PPM file.\n");
    }
    
    FILE *newFH = fopen("face2.ppm", "a+");
    
    while (c != EOF){
        fputc(c, newFH);
        printf("%c", c);
        c = fgetc(fh);
    }
    fclose(fh);
    return 0;
}
