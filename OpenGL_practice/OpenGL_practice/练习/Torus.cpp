//
//  Torus.cpp
//  OpenGL_practice
//
//  Created by 天边的星星 on 2020/9/10.
//  Copyright © 2020 starxin. All rights reserved.
//

#include "Torus.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

GLShaderManager smTorus;
GLFrame vFrameTorus;
GLFrustum frustumTorus;
GLTriangleBatch batchTorus;
GLMatrixStack mvMatrixTorus;
GLMatrixStack proMatrixTorus;
GLGeometryTransform transformTorus;

/// 初始化数据
void setupTorus(void) {
    glClearColor(0.3f, 0.3f, 0.3f, 1.0);
    
    smTorus.InitializeStockShaders();
    transformTorus.SetMatrixStacks(mvMatrixTorus, proMatrixTorus);
    
    gltMakeTorus(batchTorus, 1.0f, 0.2f, 60, 20);
    
    /// 移动origin坐标 x=6 y=6 z=5
    vFrameTorus.MoveForward(6.0f);
    
    glPointSize(4.0f);
}

/// 窗口大小变化
void windowSizeChangedTorus(int w, int h) {
    glViewport(0, 0, w, h);
    if (h == 0) {
        h = 1;
    }
    frustumTorus.SetPerspective(35.0f, float(w)/float(h), 2.0f, -500.0f);
    proMatrixTorus.LoadMatrix(frustumTorus.GetProjectionMatrix());
}

/// 特殊键位
void specialKeyTorus(int key, int x, int y) {
    if (key == GLUT_KEY_UP) {
        vFrameTorus.RotateWorld(m3dDegToRad(-5.0f), 1.0f, 0.0f, 0.0f);
    }
    else if (key == GLUT_KEY_DOWN) {
        vFrameTorus.RotateWorld(m3dDegToRad(5.0f), 1.0f, 0.0f, 0.0f);
    }
    else if (key == GLUT_KEY_LEFT) {
        vFrameTorus.RotateWorld(m3dDegToRad(-5.0f), 0.0f, 1.0f, 0.0f);
    }
    else if (key == GLUT_KEY_RIGHT) {
        vFrameTorus.RotateWorld(m3dDegToRad(5.0f), 0.0f, 1.0f, 0.0f);
    }
    else {}
    
    glutPostRedisplay();
}

/// 渲染
void renderTorus(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    
    mvMatrixTorus.PushMatrix(vFrameTorus);
    GLfloat vRed[] = {1.0f, 0.0f, 0.0f, 1.0f};
    smTorus.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, transformTorus.GetModelViewMatrix(), transformTorus.GetProjectionMatrix(), vRed);
    batchTorus.Draw();
    mvMatrixTorus.PopMatrix();
    
    glutSwapBuffers();
}

/// main函数调用入口
int testForTorus(int argc, char *argv[]) {
    gltSetWorkingDirectory(argv[0]);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Torus");
    
    glutDisplayFunc(renderTorus);
    glutReshapeFunc(windowSizeChangedTorus);
    glutSpecialFunc(specialKeyTorus);
    
    GLenum success = glewInit();
    if (success != GLEW_OK) {
        fprintf(stderr, "glew init error: %s\n", glewGetErrorString(success));
        return 1;
    }
    setupTorus();
    glutMainLoop();
    return 0;
}

#pragma clang diagnostic pop
