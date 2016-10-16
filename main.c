#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>


// line for error
int line = 1;

// allocates large block of memory for structs
// union allows use of array of structs (seperate structs require array of pointers)
typedef struct {
    int kind; // 0 = camera, 1 = cylinder, 2 = sphere, 3 = plane
    double color[3];
    double height;
    double width;
    double radius;
    double position[3];
    double normal[3];
} Object;

// squared^2 function
// make static for consistent behavior
static inline double sqr(double v){
    return v*v;
}

// dot product
static inline double dot(double* x, double* y){
    return x[0] * y[0] + x[1] * y[1] + x[2] * y[2];
}

// normalize
static inline void normalize(double* v){
    double len = (sqr(v[0]) + sqr(v[1]) + sqr(v[2]));
    v[0] /= len;
    v[1] /= len;
    v[2] /= len;
}

// (Ray Origin, Ray Direction, Center, Radius)
double cylinderIntersection(double* Ro, double* Rd, double* C, double r) {
    /*
     ==> Step 1: Find equation for the object you are interested in (cylinder)
     x^2 + z*2 = r^2 (z axis makes cylider up and down)
     
     ==> Step 2: Parameterize the equation with a center point if needed
     (x-Cx)^2 + (z-Cz)^2 = r^
     
     ==> Step 3: Substitute the equation for a ray into our object equation
     (xRo + t*xRd - Cx)^2 + (xRo + t*xRd - Cz)^2 -r^2 = 0
     
     ==> Step 4: Solve for t
     => Step 4a: Rewrite equation (flatten)
     -r^2 +
     t^2 * xRd^2 +
     t^2 * zRd^2 +
     2*t * xRo * xRd -
     2*t * xRd * Cx +
     2*t * zRo * zRd -
     2*t * zRd * Cz +
     xRo^2 -
     2*xRo*Cx +
     Cx^2 +
     Roz^2 -
     2*Rox*Cz +
     Cz^2 = 0
     
     => Step 4b: Rewrite equation in terms of t
     t^2 (xRd^2 + zRd^2) +
     t (2 * (xRo * xRd - xRd * Cx + zRo * zRd - zRd * Cz)) +
     Rox^2 - 2*Rox*Cx + Cx^2 + Roz^2 - 2*Rox*Cz + Cz^2 = 0
     
     Use quadratic equation to solve for t
     */
    
    // double a = (Rdx^2 + Rdz^2)
    double a = (sqr(Rd[0]) + sqr(Rd[2]));
    double b = (2 * (Ro[0] * Rd[0] - Rd[0] * C[0] + Ro[2] * Rd[2] - Rd[2] * C[2]));
    double c = sqr(Ro[0]) - 2*Ro[0]*C[0] + sqr(C[0]) + sqr(Ro[2]) - 2*Ro[2]*C[2] + sqr(C[2]) - sqr(r);
    
    // quadratic equation
    double det = sqr(b) - 4 * a * c;
    if (det < 0) return -1;
    
    det = sqrt(det);
    
    // return lowest t
    double t0 = (-b - det) / (2*a);
    double t1 = (-b + det) / (2*a);
    if (t0 > 0 || t1 > 0){
        if (t0 < t1) return t0;
        return t1;
    }
    return -1;
}

// (Ray Origin, Ray Direction, Center, Radius)
double sphereIntersection(double* Ro, double* Rd, double* C, double r) {
    
    // intersection
    double a = sqr(Rd[0])+sqr(Rd[1])+sqr(Rd[2]);
    double b = 2*(Rd[0]*(Ro[0]-C[0])+Rd[1]*(Ro[1]-C[1])+Rd[2]*(Ro[2]-C[2]));
    double c = sqr(Ro[0]-C[0])+sqr(Ro[1]-C[1])+sqr(Ro[2]-C[2])-sqr(r);
    
    // determinant
    double det = sqr(b) - 4 * a * c;
    
    // quadratic
    det = sqrt(det);
    double t0 = (-b - det) / (2*a);
    double t1 = (-b + det) / (2*a);
    
    // return lowest t
    if (t0 > 0 || t1 > 0){
        if (t0 < t1) return t0;
        return t1;
    }
    return -1;
}

// (Ray Origin, Ray Direction, Center, Radius)
double planeIntersection(double* Ro, double* Rd, double* C, double* n) {
    
    // set center and origin for dot product
    double l[] = {C[0]-Ro[0], C[1]-Ro[1], C[2]-Ro[2]};
    
    // dot top and bottom
    double numerator = dot(l, n);
    double denominator = dot(Rd, n);
    
    // error if div by 0
    if (denominator == 0){
        fprintf(stderr, "Error: Illegal plane. Cannot divide by zero.\n");
        exit(1);
    }
    
    // find t and return
    double t = numerator / denominator;
    if (t > 0) return t;
    return -1;
}

// nextC
int nextC(FILE* json) {
    int c = fgetc(json);
    /*#ifdef DEBUG
     printf("nextC: '%c'\n", c);
     #endif*/
    if (c == '\n') {
        line += 1;
    }
    if (c == EOF) {
        fprintf(stderr, "Error: Unexpected end of file on line number %d.\n", line);
        exit(1);
    }
    return c;
}

// expectC (pass in FILE, and expected character) (fail if not expected)
void expectC(FILE* json, int d) {
    int c = nextC(json);
    if (c == d){
        return;
    } else {
        fprintf(stderr, "Error: Expected '%c' on line %d.\n", d, line);
        exit(1);
    }
}

// reads ahead in file until no whitespace
void skipWS(FILE* json) {
    int c = fgetc(json);
    while (isspace(c)){
        
        if (c == '\n'){
            line ++;
        }
        c = fgetc(json);
    }
    ungetc(c, json); //backtracks pointer in file
}

// reads string buffer until end of string
char* nextString(FILE* json) {
    char buffer[129];
    int c = nextC(json);
    if (c != '"') {
        fprintf(stderr, "Error: Expected string on line %d.\n", line);
        exit(1);
    }
    c = nextC(json);
    int i = 0;
    while (c != '"') {
        if (i >= 128) {
            fprintf(stderr, "Error: Strings longer than 128 characters in length are not supported.\n");
            exit(1);
        }
        if (c == '\\') {
            fprintf(stderr, "Error: Strings with escape codes are not supported.\n");
            exit(1);
        }
        if (c < 32 || c > 126) {
            fprintf(stderr, "Error: Strings may contain only ascii characters.\n");
            exit(1);
        }
        buffer[i] = c;
        i += 1;
        c = nextC(json);
    }
    buffer[i] = 0;
    /*#ifdef DEBUG
     printf("%s\n", buffer);
     #endif*/
    return strdup(buffer);
}

// return next number
float nextNumber(FILE* json) {
    float value;
    fscanf(json, "%f", &value);
    //Error check this..
    return value;
}

// return next vector
float* nextVector(FILE* json){
    float* v = malloc(sizeof(float)*3);
    expectC(json, '[');
    skipWS(json);
    v[0] = nextNumber(json);
    skipWS(json);
    expectC(json, ',');
    skipWS(json);
    v[1] = nextNumber(json);
    skipWS(json);
    expectC(json, ',');
    skipWS(json);
    v[2] = nextNumber(json);
    skipWS(json);
    expectC(json, ']');
    return v;
}

// readScene (open and parse json file)
Object** readScene(char* fileName){
    Object** objects;
    objects = malloc(sizeof(Object*)*128);
    
    int c;
    FILE* json = fopen(fileName, "r");
    
    if (json == NULL){
        fprintf(stderr, "Error: Could not open file \"%s\"\n", fileName);
    }
    
    skipWS(json);
    expectC(json, '[');
    skipWS(json);
    
    // objects[i];
    int i = 0;
    
    // Find the objects
    while (1) {
        objects[i] = malloc(sizeof(Object));
        c = fgetc(json);
        if (c == ']') {
            fprintf(stderr, "Error: Json file contains no data.\n");
            fclose(json);
            exit(1);
        }
        if (c == '{') {
            skipWS(json);
            
            // Parse the object
            char* key = nextString(json);
            if (strcmp(key, "type") != 0) {
                fprintf(stderr, "Error: Expected \"type\" key on line number %d.\n", line);
                exit(1);
            }
            
            skipWS(json);
            
            expectC(json, ':');
            
            skipWS(json);
            
            char* value = nextString(json);
            
            if (strcmp(value, "camera") == 0) {
                objects[i]->kind = 0;
            } else if (strcmp(value, "cylinder") == 0) {
                objects[i]->kind = 1;
            } else if (strcmp(value, "sphere") == 0) {
                objects[i]->kind = 2;
            } else if (strcmp(value, "plane") == 0) {
                objects[i]->kind = 3;
            } else {
                fprintf(stderr, "Error: Unknown type, \"%s\", on line number %d.\n", value, line);
                exit(1);
            }
            
            skipWS(json);
            
            while (1) {
                // , }
                c = nextC(json);
                if (c == '}') {
                    // stop parsing this object
                    break;
                } else if (c == ',') {
                    // read another field
                    skipWS(json);
                    char* key = nextString(json);
                    skipWS(json);
                    expectC(json, ':');
                    skipWS(json);
                    
                    // assign all object values
                    if ((strcmp(key, "width") == 0) ||
                        (strcmp(key, "height") == 0) ||
                        (strcmp(key, "radius") == 0)) {
                        float value = nextNumber(json);
                        if (strcmp(key, "width") == 0){
                            objects[i]->width = value;
                        } else if (strcmp(key, "height") == 0){
                            objects[i]->height = value;
                        } else if (strcmp(key, "radius") == 0){
                            objects[i]->radius = value;
                        }
                    } else if ((strcmp(key, "color") == 0) ||
                               (strcmp(key, "position") == 0) ||
                               (strcmp(key, "normal") == 0)) {
                        float* value = nextVector(json);
                        if (strcmp(key, "color") == 0){
                            objects[i]->color[0] = value[0];
                            objects[i]->color[1] = value[1];
                            objects[i]->color[2] = value[2];
                        } else if (strcmp(key, "position") == 0) {
                            objects[i]->position[0] = value[0];
                            objects[i]->position[1] = value[1];
                            objects[i]->position[2] = value[2];
                        } else if (strcmp(key, "normal") == 0) {
                            objects[i]->normal[0] = value[0];
                            objects[i]->normal[1] = value[1];
                            objects[i]->normal[2] = value[2];
                        }
                        
                    } else {
                        fprintf(stderr, "Error: Unknown property, \"%s\", on line %d.\n",
                                key, line);
                        //char* value = next_string(json);
                    }
                    skipWS(json);
                } else {
                    fprintf(stderr, "Error: Unexpected value on line %d\n", line);
                    exit(1);
                }
            }
            i++;
            
            skipWS(json);
            c = nextC(json);
            if (c == ',') {
                // noop
                skipWS(json);
            } else if (c == ']') {
                fclose(json);
                objects[++i] = NULL;
                return objects;
            } else {
                printf("---------------------------------");
                fprintf(stderr, "Error: Expecting ',' or ']' on line %d.\n", line);
                exit(1);
            }
        }
    }
}

// write P6 header to buffer
char* buildHeader(Object** objects, int M, int N){
    char* header = malloc(sizeof(char)*50);
    char* headerNode = header;
    sprintf(headerNode, "P6\n%d\n%d\n255\n", M, N);
    
    return header;
}

// build image buffer based on objects
unsigned char* buildBuffer(Object** objects, int M, int N){
    
    // camera center
    double cx = 0;
    double cy = 0;
    
    // camera width and height
    double h = objects[0]->height;
    double w = objects[0]->width;
    
    // open output file && write header
    unsigned char* buffer = malloc(sizeof(char)*M*N*10);
    unsigned char* bufferNode = buffer;
    
    
    // build scene
    double pixheight = h / M;
    double pixwidth = w / N;
    for (int y = M; y >0; y -= 1) {
        for (int x = 0; x < N; x += 1) {
            
            // space for single pixel
            int color[3] = {0, 0, 0};
            double Ro[3] = {0, 0, 0};
            
            // Rd = normalize(P - Ro)
            double Rd[3] = {
                cx - (w/2) + pixwidth * (x + 0.5),
                cy - (h/2) + pixheight * (y + 0.5),
                1
            };
            normalize(Rd);
            
            // paint pixel based on type
            double best_t = INFINITY;
            for (int i=1; objects[i] != 0; i ++) {
                double t = 0;
                
                // find closest intersection based on objects
                switch(objects[i]->kind) {
                    case 0:
                        break;
                    case 1:
                        t = cylinderIntersection(Ro, Rd, objects[i]->position, objects[i]->radius);
                        if (t > 0 && t < best_t){
                            color[0] = objects[i]->color[0] * 255;
                            color[1] = objects[i]->color[1] * 255;
                            color[2] = objects[i]->color[2] * 255;
                            best_t = t;
                        }
                        break;
                    case 2:
                        t = sphereIntersection(Ro, Rd, objects[i]->position, objects[i]->radius);
                        if (t > 0 && t < best_t){
                            color[0] = objects[i]->color[0] * 255;
                            color[1] = objects[i]->color[1] * 255;
                            color[2] = objects[i]->color[2] * 255;
                            best_t = t;
                        }
                        break;
                    case 3:
                        t = planeIntersection(Ro, Rd, objects[i]->position, objects[i]->normal);
                        if (t > 0 && t < best_t){
                            color[0] = objects[i]->color[0] * 255;
                            color[1] = objects[i]->color[1] * 255;
                            color[2] = objects[i]->color[2] * 255;
                            best_t = t;
                        }
                        break;
                    default:
                        fprintf(stderr, "Error: Invalid type number: %i", objects[i]->kind);
                        exit(1);
                }
            }
            
            // correct for color value exceeding 255
            for (int i = 0; i < 3; i++){
                if (color[i] > 255){
                    color[i] = 255;
                }
            }
            
            // write image buffer for RGB
            *bufferNode++ = color[0];
            *bufferNode++ = color[1];
            *bufferNode++ = color[2];
        }
    }
    
    // end buffer
    *bufferNode = '\0';
    return buffer;
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

int main(int argc, char* argv[]) {
    
    // scene width and height
    int M = 500;
    int N = 500;
    
    // read json, and build objects
    Object** objects = readScene(argv[1]);
    
    // build header buffer
    char* header = buildHeader(objects, M, N);
    
    // build image buffer
    unsigned char* buffer = buildBuffer(objects, M, N);
    
    // dump buffer to file
    buildFile(header, buffer, argv[2], M, N);
    return 0;
}














