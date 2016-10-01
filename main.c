#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

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
     x^2 + z*2 = r^2 (z axis makes cylidar up and down)
     
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
    
    //double a = (Rdx^2 + Rdz^2)
    double a = (sqr(Rd[0]) + sqr(Rd[2]));
    double b = (2 * (Ro[0] * Rd[0] - Rd[0] * C[0] + Ro[2] * Rd[2] - Rd[2] * C[2]));
    double c = sqr(Ro[0]) - 2*Ro[0]*C[0] + sqr(C[0]) + sqr(Ro[2]) - 2*Ro[2]*C[2] + sqr(C[2]) - sqr(r);
    
    double det = sqr(b) - 4 * a * c;
    if (det < 0) return -1;
    
    det = sqrt(det);
    
    double t0 = (-b - det) / (2*a);
    if (t0 > 0) return t0;
    
    double t1 = (-b + det) / (2*a);
    if (t1 > 0) return t1;
    
    return -1;
}

double sphereIntersection() {
    return 0;
}

double planeIntersection() {
    return 0;
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

void buildScene(Object** objects){
    
    // camera center
    double cx = 0;
    double cy = 0;
    
    // camera width and height
    double h = objects[0]->height;
    double w = objects[0]->width;
    
    // scene width and height
    int M = 50;
    int N = 50;
    
    // build scene
    double pixheight = h / M;
    double pixwidth = w / N;
    for (int y = 0; y < M; y += 1) {
        for (int x = 0; x < N; x += 1) {
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
            for (int i=1; objects[i] != 0; i += 1) {
                double t = 0;
                
                switch(objects[i]->kind) {
                    case 0:
                        break;
                    case 1:
                        t = cylinderIntersection(Ro, Rd,
                                                 objects[i]->position,
                                                 objects[i]->radius);
                        break;
                    case 2:
                        break;
                    case 3:
                        break;
                    default:
                        fprintf(stderr, "Error: Invalid type number: %i", objects[i]->kind);
                        exit(1);
                }
                if (t > 0 && t < best_t) best_t = t;
            }
            if (best_t > 0 && best_t != INFINITY) {
                printf("#");
            } else {
                printf(".");
            }
        }
        printf("\n");
    }
    
}

int main(int argc, char* argv[]) {
    Object** objects = readScene(argv[1]);
    buildScene(objects);
    return 0;
}














