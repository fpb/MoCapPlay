//
//  main.cpp
//  3DViewing
//
//  Created by Fernando Birra on 23/04/14.
//  Copyright (c) 2014 FCT/UNL. All rights reserved.
//

#include <stdlib.h>
#include <iostream>

#include <GL/glew.h> // include GLEW and new version of GL on Windows

#include <GLFW/glfw3.h>
#include <GLUT/glut.h>

#include "Phigs3DView.h"
#include "quaternion.h"
#include "bvh.h"
#include "BVHLoader.h"



static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

void drawFloor(float size)
{
    float delta = size/25.0f;
    glColor3f(1.0f, 1.0f, 0.0f);
    
    glBegin(GL_LINES);
    for(int i=-size; i<=size; i+= delta) {
        glVertex3f(-size, 0, i);
        glVertex3f(size, 0, i);
    }
    for(int i=-size; i<=size; i+= delta) {
        glVertex3f(i, 0, -size);
        glVertex3f(i, 0, size);
    }
    glEnd();
}

void drawHierarchy(BVH root, int level)
{
    if(level==0) return;
    glPushMatrix();
    
    // Translation transform
    glTranslatef(root.offset.x, root.offset.y, root.offset.z);
    
    // Rotation transform
    Quaternion qx(glm::vec3(1,0,0), root.rx);
    Quaternion qy(glm::vec3(0,1,0), root.ry);
    Quaternion qz(glm::vec3(0,0,1), root.rz);
    
//    glm::mat4 trf = (qz * qx * qy).matrix4();
    glm::mat4 trf = root.getLocalTransform().matrix4();
    
    glMultMatrixf(glm::value_ptr(trf));
    
//  glm::mat4 transf = root.rotz * root.rotx * root.roty;
//  glMultMatrixf(glm::value_ptr(transf));
    glutWireSphere(2, 10, 10);
    for(int i=0; i<root.children.size(); i++) {
        glBegin(GL_LINES);
        glVertex3f(0,0,0);
        glVertex3f(root.children[i].offset.x, root.children[i].offset.y, root.children[i].offset.z);
        glEnd();
        drawHierarchy(root.children[i], level-1);
    }
    glPopMatrix();
}

BVHLoader *bvhloader;

void load_bvh(std::string filename)
{
    bvhloader = new BVHLoader(filename);
    bvhloader->loadHierarchy();
    //if(bvhloader->hasMoreMotionData())
    //    bvhloader->parseMotionData();
    bvhloader->root.dumpTree();
}

Phigs3DView camera;

void setupCamera()
{
    glm::vec3 vrp(10,100,16);
    glm::vec3 vup(0,1,0);
    glm::vec3 vpn(1,0,1);
    glm::vec3 prp(0,0,320);
    
    float umax=320, vmax=320;
    float f=290, b=-320;
    
    camera.setVRC(vrp, vup, vpn);
    camera.setVolume(prp, -umax, umax, -vmax, vmax, f, b);
    
}

void displayAxis() {
    float size = 10.0;
    
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(size, 0.0, 0.0);
    glEnd();
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_LINES);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, size, 0.0);
    glEnd();
    glBegin(GL_LINES);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 0.0, size);
    glEnd();
    
    glColor3f(0.0, 0.0, 1.0);
}

void displaySkeleton(GLFWwindow *window)
{

    float ratio;
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float) height;
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(camera.getProjectionMatrix()));
    
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(camera.getViewMatrix()));

    glColor3f(1.0f, 1.0f, 1.0f);
    drawHierarchy(bvhloader->root,100);
    // Output state
    Quaternion q(glm::vec3(1,0,0),0);
    
}

bool running = false;


static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(action != GLFW_PRESS) return;
    
    switch(key) {
        case GLFW_KEY_A:
            camera.zoomOut();
            std::cout << "ZOOM_OUT" << std::endl;
            break;
        case GLFW_KEY_Z:
            std::cout << "ZOOM_IN" << std::endl;
            camera.zoomIn();
            break;
        case GLFW_KEY_R:
            bvhloader->rewind();
            if(bvhloader->hasMoreMotionData())
                bvhloader->parseMotionData();
            displaySkeleton(window);
            running = false;
            break;
        case GLFW_KEY_S:
            if(bvhloader->hasMoreMotionData())
                bvhloader->parseMotionData();
            break;
        case GLFW_KEY_SPACE:
            running = !running;
            std::cout << "SPACE" << std::endl;
            break;
        case GLFW_KEY_UP:
            camera.rotateUp();
            displaySkeleton(window);
            break;
        case GLFW_KEY_DOWN:
            camera.rotateDown();
            displaySkeleton(window);
            break;
        case GLFW_KEY_LEFT:
            camera.rotateLeft();
            displaySkeleton(window);
            break;
        case GLFW_KEY_RIGHT:
            camera.rotateRight();
            displaySkeleton(window);
            break;
        case GLFW_KEY_D:
            std::cout << "-------- STATE ----------" << std::endl;
            bvhloader->root.dumpState(Quaternion(glm::vec3(1,0,0), 0), std::cout);
            break;
        case GLFW_KEY_ESCAPE:
            if(action==GLFW_PRESS) glfwSetWindowShouldClose(window, GL_TRUE);

    }
}


//int main_quaternion()
//{
//    quaternion_test_vector_rotation();
//    return 0;
//}

double oldTime;

void idleFunc()
{
    double time = glfwGetTime();
    
    if(time-oldTime > 0.033333) {
        oldTime = time;
        if(running & bvhloader->hasMoreMotionData())
            bvhloader->parseMotionData();
    }
}

int main(int argc, char *argv[])
{
    GLFWwindow* window;
    
    
    glfwSetErrorCallback(error_callback);
    
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing

    window = glfwCreateWindow(480, 480, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);

    GLenum err=glewInit();
    if(err!=GLEW_OK)
    {
        //Problem: glewInit failed, something is seriously wrong.
        std::cout<<"glewInit failed, aborting."<<std::endl;
    }
    
    // get version info
    const GLubyte* renderer = glGetString (GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString (GL_VERSION); // version as a string
    printf ("Renderer: %s\n", renderer);
    printf ("OpenGL version supported %s\n", version);

    oldTime = glfwGetTime();
    
    load_bvh(argv[1]);
    
    
    setupCamera();
    
    glClearColor(0.4, 0.4, 0.4, 1.0);
    glfwSetKeyCallback(window, key_callback);
    while (!glfwWindowShouldClose(window))
    {
        idleFunc();
        displayAxis();
        displaySkeleton(window);
        drawFloor(1000);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
















/*

#define WINDOW_WIDTH    640
#define WINDOW_HEIGHT   480

#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library

#include <stdio.h>

#include "Phigs3DView.h"


void test()
{
    Phigs3DView view;
    
    view.setVRC(glm::vec3(0,0,0), glm::vec3(0,1,0), glm::vec3(0,0,1));
    view.setVolume(glm::vec3(0,0,1), -1, 1, -1, 1, 0.1, 0);
}


static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

GLubyte pixeldata[WINDOW_WIDTH*WINDOW_HEIGHT*3];

void captureWindow(GLFWwindow *window)
{
    glReadPixels(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, pixeldata);
}

void keyboard(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    switch(key) {
        case GLFW_KEY_S:
            captureWindow(window);
            break;
    }
}




int main () {
    
    glfwSetErrorCallback(error_callback);

    // start GL context and O/S window using the GLFW helper library
    if (!glfwInit ()) {
        fprintf (stderr, "ERROR: could not start GLFW3\n");
        return 1;
    }
    
#ifdef __APPLE__
	// uncomment these lines if on Apple OS X
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
    
    GLFWwindow* window = glfwCreateWindow (WINDOW_WIDTH, WINDOW_HEIGHT, "Hello Triangle", NULL, NULL);
    if (!window) {
        fprintf (stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent (window);
    glfwSetKeyCallback(window, keyboard);
    
    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit();
    
    // get version info
    const GLubyte* renderer = glGetString (GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString (GL_VERSION); // version as a string
    printf ("Renderer: %s\n", renderer);
    printf ("OpenGL version supported %s\n", version);
    
    // tell GL to only draw onto a pixel if the shape is closer to the viewer
    glEnable (GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"
    glClearColor(1.0, 1.0, 1.0, 0.0);
    
    // OTHER STUFF GOES HERE NEXT
    float points[] = {
        0.0f,  0.5f,  0.0f,
        0.5f, -0.5f,  0.0f,
        -0.5f, -0.5f,  0.0f
    };
    
    GLuint vbo = 0;
    glGenBuffers (1, &vbo);
    glBindBuffer (GL_ARRAY_BUFFER, vbo);
    glBufferData (GL_ARRAY_BUFFER, 9 * sizeof (float), points, GL_STATIC_DRAW);
    
    GLuint vao = 0;
    glGenVertexArrays (1, &vao);
    glBindVertexArray (vao);
    glEnableVertexAttribArray (0);
    glBindBuffer (GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    
    const char* vertex_shader =
    "#version 400\n"
    "in vec3 vp;"
    "void main () {"
    "  gl_Position = vec4 (vp, 1.0);"
    "}";
   
    const char *vShader = {
        "#version 330 core\n"
        "uniform Uniforms {"
        "   vec3    translation;"
        "   float   scale;"
        "   vec4    rotation;"
        "   bool    enabled;"
        "};"
        "in vec2 vPos;"
        "in vec3 vColor;"
        "out vec4 fColor;"
        
    };
    
    const char* fragment_shader =
    "#version 400\n"
    "out vec4 frag_colour;"
    "void main () {"
    "  frag_colour = vec4 (0.5, 0.0, 0.5, 1.0);"
    "}";
    
    GLuint vs = glCreateShader (GL_VERTEX_SHADER);
    glShaderSource (vs, 1, &vertex_shader, NULL);
    glCompileShader (vs);
    GLuint fs = glCreateShader (GL_FRAGMENT_SHADER);
    glShaderSource (fs, 1, &fragment_shader, NULL);
    glCompileShader (fs);
    
    GLuint shader_programme = glCreateProgram ();
    glAttachShader (shader_programme, fs);
    glAttachShader (shader_programme, vs);
    glLinkProgram (shader_programme);
    
    
    test();
    
    
    while (!glfwWindowShouldClose (window)) {
        // wipe the drawing surface clear
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram (shader_programme);
        glBindVertexArray (vao);
        // draw points 0-3 from the currently bound VAO with current in-use shader
        glDrawArrays (GL_TRIANGLES, 0, 3);
        // update other events like input handling
        glfwPollEvents ();
        // put the stuff we've been drawing onto the display
        glfwSwapBuffers (window);
    }
    
    // close GL context and any other GLFW resources
    glfwTerminate();
    
    return 0;
}
 */
