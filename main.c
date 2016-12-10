#include <OpenGL/gl.h>
#include <GLFW/glfw3.h>

#include "deps/linmath.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <assert.h>

typedef struct {
    float Position[2];
    float TexCoord[2];
} Vertex;

// (-1, 1)  (1, 1)
// (-1, -1) (1, -1)

Vertex vertexes[] = {
    {{1, -1}, {0.99999, 0}},
    {{1, 1},  {0.99999, 0.99999}},
    {{-1, 1}, {0, 0.99999}},
    {{-1, -1}, {0, 0}}
};

static const char* vertex_shader_text =
"uniform mat4 MVP;\n"
"attribute vec2 TexCoordIn;\n"
"attribute vec2 vPos;\n"
"varying vec2 TexCoordOut;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    TexCoordOut = TexCoordIn;\n"
"}\n";

static const char* fragment_shader_text =
"varying vec2 TexCoordOut;\n"
"uniform sampler2D Texture;\n"
"void main()\n"
"{\n"
"    gl_FragColor = texture2D(Texture, TexCoordOut);\n"
"}\n";

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void glCompileShaderOrDie(GLuint shader) {
    GLint compiled;
    glCompileShader(shader);
    glGetShaderiv(shader,
                  GL_COMPILE_STATUS,
                  &compiled);
    if (!compiled) {
        GLint infoLen = 0;
        glGetShaderiv(shader,
                      GL_INFO_LOG_LENGTH,
                      &infoLen);
        char* info = malloc(infoLen+1);
        GLint done;
        glGetShaderInfoLog(shader, infoLen, &done, info);
        printf("Unable to compile shader: %s\n", info);
        exit(1);
    }
}

void skipLine(FILE *FH) {
    int c = fgetc(FH);
    while (c != '\n'){
        c = fgetc(FH);
    }
}

char* getNextLine(FILE *FH) {
    char* string = malloc(sizeof(char)*10);
    char* stringPoint = string;
    int c = fgetc(FH);
    while (!isspace(c) && c != '\n'){
        *stringPoint++ = c;
        printf("%c\n", c);
        c = fgetc(FH);
    }
    *stringPoint = '\0';
    return string;
}

unsigned char* buildFile(FILE *FH, int x, int y){
    unsigned char* image = malloc(sizeof(unsigned char)*100000000);
    unsigned char* imagePoint = image;
    char* string = malloc(sizeof(char)*10);
    char* stringPoint = string;
    int c = fgetc(FH);
    int alphaCount = 0;
    for(int i = x*y*3; i>0; i--){
        
        while (!isspace(c) && c != '\n'){
            *stringPoint++ = c;
            //printf("%c\n", c);
            c = fgetc(FH);
        }
        stringPoint = string;
        *imagePoint++ = atoi(stringPoint);
        //printf("%i\n", *imagePoint++);
        stringPoint = string;
        while (*stringPoint){
            *stringPoint++ = ' ';
        }
        stringPoint = string;
        c = fgetc(FH);
        if (alphaCount == 2){
            alphaCount = 0;
            *imagePoint++ = atoi("255");
            //printf("---\n");
        } else {
            alphaCount += 1;
            //printf("+++\n");
        }
    }
    return image;
}



int main(int argc, const char * argv[])
{
    FILE *FH = fopen("input.ppm", "rw");                       // open ppm file
    if (FH == NULL) {                                           // ERROR if file doesn't exists
        fprintf(stderr, "Error: File doesn't exist.\n");
        exit(1);
    }
    skipLine(FH);
    int ppmX = atoi(getNextLine(FH));
    printf("%i", ppmX);
    printf("\n");
    int ppmY = atoi(getNextLine(FH));
    printf("%i", ppmY);
    printf("\n");
    printf("\n");
    skipLine(FH);
    unsigned char *image = malloc(sizeof(unsigned char)*100000);
    image = buildFile(FH, ppmX, ppmY);
    
    
    GLFWwindow* window;
    GLuint vertex_buffer, vertex_shader, fragment_shader, program;
    GLint mvp_location, vpos_location, vcol_location;
    
    glfwSetErrorCallback(error_callback);
    
    if (!glfwInit())
        exit(EXIT_FAILURE);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    
    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    glfwSetKeyCallback(window, key_callback);
    
    glfwMakeContextCurrent(window);
    // gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1);
    
    // NOTE: OpenGL error checks have been omitted for brevity
    
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexes), vertexes, GL_STATIC_DRAW);
    
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShaderOrDie(vertex_shader);
    
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShaderOrDie(fragment_shader);
    
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    // more error checking! glLinkProgramOrDie!
    
    mvp_location = glGetUniformLocation(program, "MVP");
    assert(mvp_location != -1);
    
    vpos_location = glGetAttribLocation(program, "vPos");
    assert(vpos_location != -1);
    
    GLint texcoord_location = glGetAttribLocation(program, "TexCoordIn");
    assert(texcoord_location != -1);
    
    GLint tex_location = glGetUniformLocation(program, "Texture");
    assert(tex_location != -1);
    
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Vertex),
                          (void*) 0);
    
    glEnableVertexAttribArray(texcoord_location);
    glVertexAttribPointer(texcoord_location,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Vertex),
                          (void*) (sizeof(float) * 2));
    
    int image_width = ppmX;
    int image_height = ppmY;
    
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, image);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);
    glUniform1i(tex_location, 0);
    
    // Parameters for all image manipulation
    float objAngle = 0;
    float objWidth = 1;
    float objHeight = 1;
    
    while (!glfwWindowShouldClose(window))
    {
        
        // All Key Bindings for Image Control
        int leftRotation = glfwGetKey(window, GLFW_KEY_Q);
        int rightRotation = glfwGetKey(window, GLFW_KEY_E);
        int widthIncrease = glfwGetKey(window, GLFW_KEY_A);
        int widthDecrease = glfwGetKey(window, GLFW_KEY_D);
        int heightIncrease = glfwGetKey(window, GLFW_KEY_S);
        int heightDecrease = glfwGetKey(window, GLFW_KEY_W);
        
        // All Actions For Key Binding Control
        if (leftRotation == GLFW_PRESS)
            objAngle += .05;
        if (rightRotation == GLFW_PRESS)
            objAngle -= .05;
        if (widthIncrease == GLFW_PRESS)
            objWidth += .05;
        if (widthDecrease == GLFW_PRESS)
            objWidth -= .05;
        if (heightIncrease == GLFW_PRESS)
            objHeight += .05;
        if (heightDecrease == GLFW_PRESS)
            objHeight -= .05;
        
        float ratio;
        int width, height;
        mat4x4 m, p, mvp;
        
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
        
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        
        mat4x4_identity(m);
        
        mat4x4_rotate_Z(m, m, (float) objAngle);
        mat4x4_ortho(p, -objWidth, objWidth, objHeight, -objHeight, 1.f, -1.f);
        //mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        
        mat4x4_mul(mvp, p, m);
        
        
        glUseProgram(program);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
        glDrawArrays(GL_QUADS, 0, 4);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwDestroyWindow(window);
    
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

//! [code]
