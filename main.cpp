﻿// Autor: Tamara Dzambic
// Opis:  projekat 12


#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>

#include <GL/glew.h>   
#include <GLFW/glfw3.h>


#include <cmath>
#define M_PI 3.14159265358979323846
#define _CRT_SECURE_NO_WARNINGS
#define CRES 30 

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

unsigned int compileShader(GLenum type, const char* source);
unsigned int createShader(const char* vsSource, const char* fsSource); 
static unsigned loadImageToTexture(const char* filePath); 

void initialize(unsigned int VAO, unsigned int VBO, float* vertices, int verticesCount);
void initializeTexture(unsigned int VAO, unsigned int VBO, float* vertices, int verticesCount, unsigned indexTexture);

void generateCircle(float circle[], float centerX, float centerY, float radius, float aspectRatio, int beg);
void generateHalfCircle(float vec[], float x0, float y0, bool side, float angleOffset, float radius, float aspectRatio, int beg);
void generateEllipse(float circle[], float centerX, float centerY, float radiusX, float radiusY, int beg);

void generateFire(float circle[], float centerX, float centerY, float radius, float aspectRatio, int beg);
void generatePalmTree(float palmTree[], float aspectRatio);
void generateClouds(float circleVerticesCloud[], float aspectRatio);
void generateFish(float fishX, float fishY, float fishWidth, float fishHeight, float fishVertices[], int beg);

void updateFishPosition(float fishVertices[], int verticesCount, float speed);

int main(void)
{
    //initialization
    if (!glfwInit()){
        std::cout << "GLFW Biblioteka se nije ucitala! :(\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "SV43/2020", primaryMonitor, NULL);

    float aspectRatio = static_cast<float>(mode->width) / mode->height;

    if (window == NULL){
        std::cout << "Prozor nije napravljen! :(\n";
        glfwTerminate();
        return 2;
    }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK){
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        return 3;
    }

    //shader and buffers
    unsigned int unifiedShader = createShader("basic.vert", "basic.frag");

    unsigned int VAO[9];
    glGenVertexArrays(9, VAO);
    unsigned int VBO[9];
    glGenBuffers(9, VBO);


    //sky
    float skyVertices[] = {
        -1.0, -1.0,
        1.0, -1.0,
        -1.0, 1.0,
        1.0, 1.0,
     };
    initialize(VAO[0], VBO[0], skyVertices, sizeof(skyVertices));

    //sun and moon
    float circles[((2 + CRES) * 2) * 2];
    generateCircle(circles, 0.0f, 0.0f, 0.1f, aspectRatio, 0);
    generateCircle(circles, 0.0f, 0.0f, 0.15f, aspectRatio, (2 + CRES) * 2);
    initialize(VAO[1], VBO[1], circles, sizeof(circles));

    // sea
    float seaVertices[] = {
        -1.0, -1.0,
        1.0, -1.0,
        -1.0, 0.0,
        1.0, 0.0,
    };
    initialize(VAO[2], VBO[2], seaVertices, sizeof(seaVertices));

    // index texture
    float indexVertices[] = {
        1.0f, -1.0f,  1.0f, 0.0f,
        1.0f, -0.85f,  1.0f, 1.0f,
        0.7f, -1.0f,  0.0f, 0.0f,
        0.7f, -0.85f,  0.0f, 1.0f
    };
    unsigned indexTexture = loadImageToTexture("index.png");
    initializeTexture(VAO[3], VBO[3], indexVertices, sizeof(indexVertices), indexTexture);
    //unsigned uTexLoc = glGetUniformLocation(unifiedShader, "uTex");
    //glUniform1i(uTexLoc, 0); // Indeks teksturne jedinice (sa koje teksture ce se citati boje)

    // islands
    float ellipses[((2 + CRES) * 2) * 2]{};
    generateEllipse(ellipses, 0.0f, -0.2f, 0.6f, 0.2f, 0);
    generateEllipse(ellipses, -0.6f, -0.8f, 0.3f, 0.1f, (2 + CRES) * 2);
    initialize(VAO[4], VBO[4], ellipses, sizeof(ellipses));

    // palmtree
    float palmTree[6 + ((2 + CRES) * 2) * 5]{};  // triangle + 4 half-circles
    generatePalmTree(palmTree, aspectRatio);
    initialize(VAO[5], VBO[5], palmTree, sizeof(palmTree));

    // fire
    float fireCircles[((2 + CRES) * 2) * 2]{};
    generateFire(fireCircles, 0.2f, -0.1f, 0.08f, aspectRatio, 0);
    generateFire(fireCircles, 0.2f, -0.13f, 0.05f, aspectRatio, (2 + CRES) * 2);
    initialize(VAO[6], VBO[6], fireCircles, sizeof(fireCircles));

    // clouds
    float circleVerticesCloud[((2 + CRES) * 2) * 5 * 9]{};
    generateClouds(circleVerticesCloud, aspectRatio);
    initialize(VAO[7], VBO[7], circleVerticesCloud, sizeof(circleVerticesCloud));
   
    // fishes
    float fishVertices[2 * 3 * 4]{};
    generateFish(-0.7, -0.2, 0.08, 0.1, fishVertices, 0);
    generateFish(-0.2, -0.5, 0.15, 0.15, fishVertices, 6);
    generateFish(0.3, -0.8, 0.1, 0.12, fishVertices, 12);
    generateFish(0.8, -0.4, 0.12, 0.1, fishVertices, 18);
    initialize(VAO[8], VBO[8], fishVertices, sizeof(fishVertices));


    float speed = 0.5;
    float r = 1.0;
    float cloudOffsetX = 0.0f;

    GLuint sunPosLoc = glGetUniformLocation(unifiedShader, "sunPos");
    GLuint seaPosLoc = glGetUniformLocation(unifiedShader, "seaPos");
    GLuint fishPosLoc = glGetUniformLocation(unifiedShader, "fishPos");
    GLuint firePosLoc = glGetUniformLocation(unifiedShader, "firePos");
    GLuint cloudPosLoc = glGetUniformLocation(unifiedShader, "cloudCircleCenter");

    GLuint elementMode = glGetUniformLocation(unifiedShader, "mode");
    GLuint isSun = glGetUniformLocation(unifiedShader, "isSun");
    GLuint isLeaf = glGetUniformLocation(unifiedShader, "isLeaf");
    GLuint useTextureLoc = glGetUniformLocation(unifiedShader, "useTexture");
    clock_t lastKeyPressTime = clock();

    while (!glfwWindowShouldClose(window))
    {
        bool speedKeyPressed = false;

        // tasters
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            glEnable(GL_PROGRAM_POINT_SIZE);
            glPointSize(4);
        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_3) == GLFW_PRESS)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS)
        {
            clock_t currentTime = clock();
            double elapsedTime = static_cast<double>(currentTime - lastKeyPressTime) / CLOCKS_PER_SEC;

            if (!speedKeyPressed && elapsedTime > 0.1) {
                if (speed <= 2.0) {
                    speed += 0.2;
                    speedKeyPressed = true;  // Set the flag to true to indicate key press
                    lastKeyPressTime = currentTime;  // Update the last key press time
                }
            }
   
        }
        if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS)
        {
            clock_t currentTime = clock();
            double elapsedTime = static_cast<double>(currentTime - lastKeyPressTime) / CLOCKS_PER_SEC;

            if (!speedKeyPressed && elapsedTime > 0.1) {
                if (speed >= 0.2) {
                    speed -= 0.2;
                    speedKeyPressed = true;  // Set the flag to true to indicate key press
                    lastKeyPressTime = currentTime;  // Update the last key press time
                }
            }
        }
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        {
            speed = 1.0;
        }

        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(unifiedShader);

        //sky        
        glBindVertexArray(VAO[0]);
        glUniform1i(elementMode, 0);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        //sun, moon
        glBindVertexArray(VAO[1]);
        glUniform1i(elementMode, 1);
        glUniform1i(isSun, GL_FALSE);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 32);
        glUniform2f(sunPosLoc, r * sin(glfwGetTime() * speed), r * (cos(glfwGetTime() * speed)));

        glUniform1i(isSun, GL_TRUE);
        glDrawArrays(GL_TRIANGLE_FAN, 32, 32);
        glUniform2f(sunPosLoc, r * sin(glfwGetTime() * speed + M_PI), r * (cos(glfwGetTime() * speed + M_PI)));
       
        //sea 
        glBindVertexArray(VAO[2]);
        glUniform1i(elementMode, 2);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glUniform2f(seaPosLoc, 0.2 * sin(glfwGetTime() * speed), 0.1 * sin(glfwGetTime() * speed));

        //island1 
        glBindVertexArray(VAO[4]);
        glUniform1i(elementMode, 4);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 32);

        //fishes
        glBindVertexArray(VAO[8]);
        glUniform1i(elementMode, 8);
        updateFishPosition(fishVertices, sizeof(fishVertices), speed);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glUniform2f(seaPosLoc, 0.6 * cos(glfwGetTime() * speed), 0.1 * sin(glfwGetTime() * speed));
        glDrawArrays(GL_TRIANGLES, 3, 3);
        glUniform2f(seaPosLoc, 0.2 * sin(glfwGetTime() * speed), 0.1 * sin(glfwGetTime() * speed));
        glDrawArrays(GL_TRIANGLES, 6, 3);
        glUniform2f(seaPosLoc, 0.4 * sin(glfwGetTime() * speed), 0.1 * sin(glfwGetTime() * speed));
        glDrawArrays(GL_TRIANGLES, 9, 3);

        //island2
        glBindVertexArray(VAO[4]);
        glUniform1i(elementMode, 4);
        glDrawArrays(GL_TRIANGLE_FAN, 32, 32);

        // clouds
        glBindVertexArray(VAO[7]);
        glUniform1i(elementMode, 7);
        cloudOffsetX += 0.0009 * speed;
        if (cloudOffsetX > 1.8f) {
            cloudOffsetX = -1.8f;
        }
        for (int i = 0; i < 5 * 9; i++) {
            glUniform2f(cloudPosLoc, cloudOffsetX, 0);
            glDrawArrays(GL_TRIANGLE_FAN, 32*i, 32);
        }

        //pamltree
        glBindVertexArray(VAO[5]);
        glUniform1i(elementMode, 5);
        glUniform1i(isLeaf, GL_FALSE);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glUniform1i(isLeaf, GL_TRUE);
        for (int i = 0; i < 5; i++) {
            glDrawArrays(GL_TRIANGLE_FAN, 3+32*i, 32);
        }

        // fire
        glBindVertexArray(VAO[6]);
        glUniform1i(elementMode, 6);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 32);
        glDrawArrays(GL_TRIANGLE_FAN, 32, 32);
        glUniform1f(firePosLoc, 1.2 + 0.3 * sin(glfwGetTime() * speed));

        // index texture
        glBindVertexArray(VAO[3]);
        glUniform1i(elementMode, 3);
        glUniform1i(useTextureLoc, GL_TRUE);
        glBindTexture(GL_TEXTURE_2D, indexTexture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, indexTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glUniform1i(useTextureLoc, GL_FALSE);


        //swap
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //empty and end
    glDeleteTextures(1, &indexTexture);
    glDeleteBuffers(1, VBO);
    glDeleteVertexArrays(1, VAO);
    glDeleteProgram(unifiedShader);
    glfwTerminate();
    return 0;
}

unsigned int compileShader(GLenum type, const char* source)
{
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;
    if (file.is_open())
    {
        ss << file.rdbuf();
        file.close();
        std::cout << "Uspjesno procitao fajl sa putanje \"" << source << "\"!" << std::endl;
    }
    else {
        ss << "";
        std::cout << "Greska pri citanju fajla sa putanje \"" << source << "\"!" << std::endl;
    }
    std::string temp = ss.str();
    const char* sourceCode = temp.c_str();

    int shader = glCreateShader(type);

    int success;
    char infoLog[512];
    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        if (type == GL_VERTEX_SHADER)
            printf("VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            printf("FRAGMENT");
        printf(" sejder ima gresku! Greska: \n");
        printf(infoLog);
    }
    return shader;
}

unsigned int createShader(const char* vsSource, const char* fsSource){

    unsigned int program;
    unsigned int vertexShader;
    unsigned int fragmentShader;

    program = glCreateProgram();

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource);
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource);


    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);
    glValidateProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Objedinjeni sejder ima gresku! Greska: \n";
        std::cout << infoLog << std::endl;
    }

    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}

static unsigned loadImageToTexture(const char* filePath) {
    int TextureWidth;
    int TextureHeight;
    int TextureChannels;
    unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 0);
    if (ImageData != NULL)
    {
        //Slike se osnovno ucitavaju naopako pa se moraju ispraviti da budu uspravne
        stbi__vertical_flip(ImageData, TextureWidth, TextureHeight, TextureChannels);
        // Provjerava koji je format boja ucitane slike
        GLint InternalFormat = -1;
        switch (TextureChannels) {
        case 1: InternalFormat = GL_RED; break;
        case 3: InternalFormat = GL_RGB; break;
        case 4: InternalFormat = GL_RGBA; break;
        default: InternalFormat = GL_RGB; break;
        }

        unsigned int Texture;
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, TextureWidth, TextureHeight, 0, InternalFormat, GL_UNSIGNED_BYTE, ImageData);
        glBindTexture(GL_TEXTURE_2D, 0);
        // oslobadjanje memorije zauzete sa stbi_load posto vise nije potrebna
        stbi_image_free(ImageData);
        return Texture;
    }
    else
    {
        std::cout << "Textura nije ucitana! Putanja texture: " << filePath << std::endl;
        stbi_image_free(ImageData);
        return 0;
    }
}

void initialize(unsigned int VAO, unsigned int VBO, float *vertices, int verticesCount) {
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verticesCount, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void initializeTexture(unsigned int VAO, unsigned int VBO, float* vertices, int verticesCount, unsigned indexTexture) {
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verticesCount, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, indexTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void generatePalmTree(float palmTree[], float aspectRatio) {
    palmTree[0] = -0.18;
    palmTree[1] = -0.2;
    palmTree[2] = -0.12;
    palmTree[3] = -0.2;
    palmTree[4] = -0.15;
    palmTree[5] = 0.4;
    generateHalfCircle(palmTree, -0.15f, 0.4f, true, 70, 0.13f, aspectRatio, 6);
    generateHalfCircle(palmTree, -0.15f, 0.4f, true, 10, 0.12f, aspectRatio, (2 + CRES) * 2 * 1 + 6);
    generateHalfCircle(palmTree, -0.15f, 0.4f, false, -70, 0.13f, aspectRatio, (2 + CRES) * 2 * 2 + 6);
    generateHalfCircle(palmTree, -0.15f, 0.4f, false, -10, 0.12f, aspectRatio, (2 + CRES) * 2 * 3 + 6);
    generateHalfCircle(palmTree, -0.15f, 0.4f, true, -50, 0.11f, aspectRatio, (2 + CRES) * 2 * 4 + 6);
}

void generateClouds(float circleVerticesCloud[], float aspectRatio) {
    float cloudRadius = 0.1f;
    const int numClouds = 9;
    const int numCloudCirclesCloud = 5 * numClouds;  //5 kruznica zam svaki oblak
    float cloudCircleCenters[numCloudCirclesCloud][2] = {};

    for (int i = 0; i < numClouds; i++) {
        cloudCircleCenters[i * 5][0] = -2.52f + 0.6 * i;
        cloudCircleCenters[i * 5][1] = 0.85f - 0.3f * (i % 2);

        cloudCircleCenters[i * 5 + 1][0] = -2.6f + 0.6 * i;
        cloudCircleCenters[i * 5 + 1][1] = 0.8f - 0.3f * (i % 2);

        cloudCircleCenters[i * 5 + 2][0] = -2.43f + 0.6 * i;
        cloudCircleCenters[i * 5 + 2][1] = 0.8f - 0.3f * (i % 2);

        cloudCircleCenters[i * 5 + 3][0] = -2.55f + 0.6 * i;
        cloudCircleCenters[i * 5 + 3][1] = 0.74f - 0.3f * (i % 2);

        cloudCircleCenters[i * 5 + 4][0] = -2.48f + 0.6 * i;
        cloudCircleCenters[i * 5 + 4][1] = 0.74f - 0.3f * (i % 2);
    }

    for (int i = 0; i < numCloudCirclesCloud; ++i) {
        generateCircle(circleVerticesCloud, cloudCircleCenters[i][0], cloudCircleCenters[i][1], cloudRadius, aspectRatio, (2 + CRES) * 2 * i);
    }
}

void generateCircle(float circle[], float centerX, float centerY, float radius, float aspectRatio, int beg) {
    circle[beg] = centerX; // Center X
    circle[beg + 1] = centerY; // Center Y
    for (int i = 0; i <= CRES; i++) {
        float angle = (M_PI / 180) * (i * 360.0 / CRES);
        circle[beg + 2 + 2 * i] = centerX + radius * cos(angle) / aspectRatio; // Xi
        circle[beg + 2 + 2 * i + 1] = centerY + radius * sin(angle); // Yi
    }
}

void generateFire(float circle[], float centerX, float centerY, float radius, float aspectRatio, int beg) {
    circle[beg] = centerX; // Center X
    circle[beg + 1] = centerY + radius*2; // Center Y
    for (int i = 0; i <= CRES; i++) {
        float angle = (M_PI / 180) * (i * 360.0 / CRES);
        circle[beg + 2 + 2 * i] = centerX + radius * cos(angle) / aspectRatio; // Xi
        circle[beg + 2 + 2 * i + 1] = centerY + radius * sin(angle); // Yi
    }
}

void generateHalfCircle(float vec[], float x0, float y0, bool side, float angleOffset, float radius, float aspectRatio, int beg) {
    float angle = (angleOffset + (0 / float(CRES)) * 180.0) * M_PI / 180.0;
    float x_offset = radius * cos(angle) / aspectRatio;
    float y_offset = radius * sin(angle);

    if (side) {
        x0 -= x_offset;
        y0 -= y_offset;
    }
    else {
        x0 += x_offset;
        y0 += y_offset;
    }

    vec[beg] = x0;
    vec[beg + 1] = y0;
    for (int i = 0; i <= CRES; ++i) {
        float angle = (angleOffset + (i / float(CRES)) * 180.0) * M_PI / 180.0;

        vec[beg + 2 + i * 2] = x0 + radius * cos(angle) / aspectRatio;
        vec[beg + 2 + i * 2 + 1] = y0 + radius * sin(angle);
    }
}

void generateEllipse(float ellipse[], float centerX, float centerY, float radiusX,float radiusY, int beg) {
    ellipse[beg] = centerX; // Center X
    ellipse[beg + 1] = centerY; // Center Y
    for (int i = 0; i <= CRES; i++) {
        float angle = 2.0f * M_PI * i / CRES;   
        ellipse[beg + 2 + 2 * i] = centerX + radiusX * cos(angle); // Xi
        ellipse[beg + 2 + 2 * i + 1] = centerY + radiusY * sin(angle); // Yi
    }
}

void generateFish(float fishX, float fishY, float fishWidth, float fishHeight, float fishVertices[], int beg) {
    fishVertices[beg] = fishX - fishWidth;
    fishVertices[beg + 1] = fishY - fishHeight;
    fishVertices[beg + 2] = fishX + fishWidth;
    fishVertices[beg + 3] = fishY - fishHeight;
    fishVertices[beg + 4] = fishVertices[beg];
    fishVertices[beg + 5] = fishY;
}

void updateFishPosition(float fishVertices[], int verticesCount, float speed){
    glBufferData(GL_ARRAY_BUFFER, verticesCount, fishVertices, GL_STATIC_DRAW);
    if (cos(glfwGetTime() * speed) < 0) {
        fishVertices[4] = fishVertices[0];
        fishVertices[12 + 4] = fishVertices[12];
        fishVertices[18 + 4] = fishVertices[18];
    }else {
        fishVertices[4] = fishVertices[2];
        fishVertices[12 + 4] = fishVertices[12 + 2];
        fishVertices[18 + 4] = fishVertices[18 + 2];
    }

    if (sin(glfwGetTime() * speed) > 0) {
        fishVertices[6 + 4] = fishVertices[6];
    } else {
        fishVertices[6 + 4] = fishVertices[6 + 2];
    }
}
