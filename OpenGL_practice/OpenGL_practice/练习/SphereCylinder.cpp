//
//  SphereCylinder.cpp
//  OpenGL_practice
//
//  Created by 天边的星星 on 2020/9/23.
//  Copyright © 2020 starxin. All rights reserved.
//

#include "SphereCylinder.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

GLFrame sc_objectFrame;
GLFrame sc_cameraFrame;

GLFrustum sc_frustum;

GLMatrixStack sc_modelViewMatrix;
GLMatrixStack sc_projectionMatrix;

GLGeometryTransform sc_transform;

GLTriangleBatch sc_sphereBatch;
GLTriangleBatch sc_torusBatch;
GLTriangleBatch sc_cylinderBatch;
GLTriangleBatch sc_coneBatch;
GLTriangleBatch sc_diskBatch;

GLShaderManager sc_shaderManager;

GLuint sc_step = 0;

GLfloat sc_vGreen[] = {0.0f, 1.0f, 0.0f, 1.0f};
GLfloat sc_vBlack[] = {0.0f, 0.0f, 0.0f, 1.0f};


void sc_setup() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    
    sc_shaderManager.InitializeStockShaders();
    
    /// 物体坐标向里也就是-z平移10个单位
//    sc_objectFrame.MoveForward(10);
    /// 相机坐标中介向z平移10个单位
    sc_cameraFrame.MoveForward(-10);
    
    gltMakeSphere(sc_sphereBatch, 2.0f, 20, 40);
    gltMakeTorus(sc_torusBatch, 1.0f, 0.4f, 20, 10);
    gltMakeCylinder(sc_cylinderBatch, 1.0f, 1.0f, 2.0f, 20, 5);
    gltMakeCylinder(sc_coneBatch, 2.0f, 0.0f, 3.0f, 20, 5);
    gltMakeDisk(sc_diskBatch, 0.6f, 1.5f, 20, 5);
}
void sc_draw(GLTriangleBatch &batch) {
    
    sc_shaderManager.UseStockShader(GLT_SHADER_FLAT, sc_transform.GetModelViewProjectionMatrix(), sc_vGreen);
    batch.Draw();
    
    /// 开启多边形偏移
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(-1.0f, -1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(2.0F);
    
    /// 开启混合功能，实现一定的抗锯齿能力
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    
    sc_shaderManager.UseStockShader(GLT_SHADER_FLAT, sc_transform.GetModelViewProjectionMatrix(), sc_vBlack);
    batch.Draw();
    
    /// 复原状态机
    glLineWidth(1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_POLYGON_OFFSET_LINE);
    
    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_BLEND);
}
void sc_render() {
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    /// 使用相机坐标转换
    M3DMatrix44f cameraM;
    sc_cameraFrame.GetCameraMatrix(cameraM);
    sc_modelViewMatrix.PushMatrix(cameraM);
    
    /// 使用物体坐标中介转换
    M3DMatrix44f objectM;
    sc_objectFrame.GetMatrix(objectM);
    sc_modelViewMatrix.MultMatrix(objectM);
    
    switch (sc_step) {
        case 0:
            sc_draw(sc_sphereBatch);
            break;
        case 1:
            sc_draw(sc_torusBatch);
            break;
        case 2:
            sc_draw(sc_cylinderBatch);
            break;
        case 3:
            sc_draw(sc_coneBatch);
            break;
        case 4:
            sc_draw(sc_diskBatch);
            break;
        default:
            break;
    }
    sc_modelViewMatrix.PopMatrix();
    glutSwapBuffers();
}
void sc_specialKey(int key, int x, int y) {
#warning “如果使用相机坐标旋转则只能看到平移，故旋转不适合相机坐标中介旋转”
    if (key == GLUT_KEY_UP) {
        sc_objectFrame.RotateWorld(m3dDegToRad(-5.0f), 1.0f, 0.0f, 0.0f);
    }
    else if (key == GLUT_KEY_DOWN) {
        sc_objectFrame.RotateWorld(m3dDegToRad(5.0f), 1.0f, 0.0f, 0.0f);
    }
    else if (key == GLUT_KEY_LEFT) {
        sc_objectFrame.RotateWorld(m3dDegToRad(-5.0f), 0.0f, 1.0f, 0.0f);
    }
    else if (key == GLUT_KEY_RIGHT) {
        sc_objectFrame.RotateWorld(m3dDegToRad(5.0f), 0.0f, 1.0f, 0.0f);
    }
    glutPostRedisplay();
}
void sc_keyPress(unsigned char key, int x, int y) {
    if (key == 32) {
        ++sc_step;
        if (sc_step > 4) {
            sc_step = 0;
        }
    }
    
    switch (sc_step) {
        case 0:
            glutSetWindowTitle("sphere");
            break;
        case 1:
            glutSetWindowTitle("torus");
            break;
        case 2:
            glutSetWindowTitle("cylinder");
            break;
        case 3:
            glutSetWindowTitle("cone");
            break;
        case 4:
            glutSetWindowTitle("disk");
            break;
        default:
            break;
    }
    
    glutPostRedisplay();
}
void sc_reshape(int w, int h) {
    if (h == 0) {
        h = 1;
    }
    glViewport(0, 0, w, h);
    sc_frustum.SetPerspective(35.0f, GLfloat(w)/GLfloat(h), 1.0f, 200.0f);
    sc_projectionMatrix.LoadMatrix(sc_frustum.GetProjectionMatrix());
    
    sc_modelViewMatrix.LoadIdentity();
    
    sc_transform.SetMatrixStacks(sc_modelViewMatrix, sc_projectionMatrix);
}

int sc_main(int argc, char *argv[]) {
    gltSetWorkingDirectory(argv[0]);
    
    glutInit(&argc, argv);
    glutInitWindowSize(800, 800);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutCreateWindow("sphere");
    
    glutDisplayFunc(sc_render);
    glutSpecialFunc(sc_specialKey);
    glutReshapeFunc(sc_reshape);
    glutKeyboardFunc(sc_keyPress);
    
    GLenum success = glewInit();
    if (success != GLEW_OK) {
        fprintf(stderr, "glew init error: %s\n", glewGetErrorString(success));
        return 1;
    }
    sc_setup();
    
    glutMainLoop();
    
    return 0;
}
 
#pragma clang diagnostic pop
