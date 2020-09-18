//
//  FloorSphere.cpp
//  OpenGL_practice
//
//  Created by 天边的星星 on 2020/9/17.
//  Copyright © 2020 starxin. All rights reserved.
//

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

#include "FloorSphere.hpp"

GLShaderManager fs_shaderManager;

GLFrame fs_cameraFrame;

GLMatrixStack fs_viewMatrix;
GLMatrixStack fs_projectionMatrix;

GLFrustum fs_frustum;
GLGeometryTransform fs_transform;

GLBatch fs_floorBatch;
GLTriangleBatch fs_bigSphereBatch;

void fs_setupRC() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    fs_shaderManager.InitializeStockShaders();
    
    /// 开启深度测试
    glEnable(GL_DEPTH_TEST);
    
    /// 设置地板坐标
    fs_floorBatch.Begin(GL_LINES, 400);
    for (GLfloat x = -50.0; x<= 50.0 ; x+= 1.0f) {
        
        /// 绘制横线
        fs_floorBatch.Vertex3f(-50.0f, -1.5f, x);
        fs_floorBatch.Vertex3f(50.0f, -1.5f, x);
        
        /// 绘制竖线
        fs_floorBatch.Vertex3f(x, -1.5f, 50.0f);
        fs_floorBatch.Vertex3f(x, -1.5f, -50.0f);
    }
    fs_floorBatch.End();
    
    gltMakeSphere(fs_bigSphereBatch, 0.5f, 60, 20);
}

void fs_changeSize(int w, int h) {
    glViewport(0, 0, w, h);
    
    fs_frustum.SetPerspective(35.0f, float(w)/float(h), 1.0f, 400.0f);
    fs_projectionMatrix.LoadMatrix(fs_frustum.GetProjectionMatrix());
    
    fs_transform.SetMatrixStacks(fs_viewMatrix, fs_projectionMatrix);
}

void fs_render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    /// 设置模型视图
    fs_viewMatrix.PushMatrix();
    
    M3DMatrix44f viewM;
    fs_cameraFrame.GetCameraMatrix(viewM);
    fs_viewMatrix.LoadMatrix(viewM);
    
    GLfloat vBlue[4] = {0.0f, 0.0f, 1.0f, 1.0f};
    fs_shaderManager.UseStockShader(GLT_SHADER_FLAT, fs_transform.GetModelViewProjectionMatrix(), vBlue);
    fs_floorBatch.Draw();
    
    ///
    fs_viewMatrix.Translate(0.0f, 0.0f, -5.0f);
    
    GLfloat vRed[] = {1.0f, 0.0f, 0.0f, 1.0f};
    GLfloat vLight[] = {0.0f, 10.0f, 0.0f};
    fs_shaderManager.UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF, fs_transform.GetModelViewMatrix(), fs_transform.GetProjectionMatrix(),vLight, vRed);
    fs_bigSphereBatch.Draw();
    
    
    fs_viewMatrix.PopMatrix();
    
    glutSwapBuffers();
}

int fs_main(int argc, char *argv[]) {
    gltSetWorkingDirectory(argv[0]);
    
    glutInitWindowSize(800, 800);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInit(&argc, argv);
    
    glutCreateWindow("floor-sphere");
    
    glutDisplayFunc(fs_render);
    glutReshapeFunc(fs_changeSize);
    
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "glew init error: %s\n", glewGetErrorString(err));
        return 1;
    }
    
    fs_setupRC();
    glutMainLoop();
    
    return 0;
}

#pragma clang diagnostic pop
