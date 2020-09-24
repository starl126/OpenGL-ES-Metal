//
//  RotateSphere.cpp
//  OpenGL_practice
//
//  Created by 天边的星星 on 2020/9/24.
//  Copyright © 2020 starxin. All rights reserved.
//

#include "RotateSphere.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

GLFrame rs_objectFrame;

GLMatrixStack rs_modelViewMatrix;
GLMatrixStack rs_projectionMatrix;

GLFrustum rs_frustum;

GLShaderManager rs_shaderManager;
GLTriangleBatch rs_sphereBatch;

GLGeometryTransform rs_transform;

GLfloat rs_vBlack[] = {0.0f, 0.0f, 0.0f, 1.0f};

void rs_setup() {
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    
    rs_objectFrame.MoveForward(5.0f);
    rs_shaderManager.InitializeStockShaders();
    
    gltMakeSphere(rs_sphereBatch, 0.6f, 10, 20);
}
void rs_reshape(int w, int h) {
    if (h == 0) {
        h = 1;
    }
    glViewport(0, 0, w, h);
    rs_frustum.SetPerspective(35.0f, GLfloat(w)/GLfloat(h), 1.0f, 100.0f);
    rs_projectionMatrix.LoadMatrix(rs_frustum.GetProjectionMatrix());
    
    rs_transform.SetMatrixStacks(rs_modelViewMatrix, rs_projectionMatrix);
}
/// 对物体坐标进行操作
void rs_objectMatrixRender() {

    /// 第一种方式
//    rs_objectFrame.RotateWorld(m3dDegToRad(-0.5f), 0.0f, 1.0f, 0.0f);
//    rs_modelViewMatrix.PushMatrix(rs_objectFrame);
    
    /// 第二种方式
    static CStopWatch timer;
    GLfloat yRot = timer.GetElapsedSeconds() * 60.0f;
    rs_modelViewMatrix.PushMatrix(rs_objectFrame);
    rs_modelViewMatrix.Rotate(yRot, 0.0f, 1.0f, 0.0f);
    
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(2.0F);
    
    rs_shaderManager.UseStockShader(GLT_SHADER_FLAT, rs_transform.GetModelViewProjectionMatrix(), rs_vBlack);
    rs_sphereBatch.Draw();
    
    rs_modelViewMatrix.PopMatrix();
}
void rs_finalMatrixRender() {
    static CStopWatch timer;
    GLfloat yRot = timer.GetElapsedSeconds() * 60.0f;
    
    /// 这里是通过最终的坐标进行变换
    M3DMatrix44f translateM, rotateM, modelViewM, modelViewProjectionM;
    m3dTranslationMatrix44(translateM, 0.0f, 0.0f, -5.0f);
    m3dRotationMatrix44(rotateM, m3dDegToRad(yRot), 0.0f, 1.0f, 0.0f);
    m3dMatrixMultiply44(modelViewM, translateM, rotateM);
    m3dMatrixMultiply44(modelViewProjectionM, rs_frustum.GetProjectionMatrix(), modelViewM);
    
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(2.0F);
    
    
    rs_shaderManager.UseStockShader(GLT_SHADER_FLAT, modelViewProjectionM, rs_vBlack);
    rs_sphereBatch.Draw();
}
void rs_render() {
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
//    rs_finalMatrixRender();
    rs_objectMatrixRender();
    
    glutSwapBuffers();
    glutPostRedisplay();
}

int rs_main(int argc, char *argv[]) {
    gltSetWorkingDirectory(argv[0]);
    
    glutInit(&argc, argv);
    glutInitWindowSize(800, 800);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutCreateWindow("sphere");
    
    glutDisplayFunc(rs_render);
    glutReshapeFunc(rs_reshape);
    
    GLenum success = glewInit();
    if (success != GLEW_OK) {
        fprintf(stderr, "glew init error: %s\n", glewGetErrorString(success));
        return 1;
    }
    rs_setup();
    
    glutMainLoop();
    
    return 0;
}

#pragma clang diagnostic pop
 
