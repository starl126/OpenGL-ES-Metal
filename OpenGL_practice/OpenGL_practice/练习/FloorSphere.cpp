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

/// 视角坐标
GLFrame fs_cameraFrame;
/// 物体坐标
GLFrame fs_objectFrame;

GLMatrixStack fs_modelViewMatrix;
GLMatrixStack fs_projectionMatrix;

GLFrustum fs_frustum;
GLGeometryTransform fs_transform;

GLBatch fs_floorBatch;
GLTriangleBatch fs_bigSphereBatch;
GLTriangleBatch fs_smallSphereBatch;

/// 添加随机小球
#define kNumberSpheres 50
GLFrame fs_sphereFrames[kNumberSpheres];

/// 正背面剔除
GLuint fs_isCull = 0;
GLuint fs_isDepthTest = 0;
GLuint fs_zFighting = 0;

void fs_setupRC() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    fs_shaderManager.InitializeStockShaders();
    
    /// 开启深度测试
    glEnable(GL_DEPTH_TEST);
    
    /// 设置地板坐标
    fs_floorBatch.Begin(GL_LINES, 400);
    for (GLfloat x = -50.0; x<= 50.0 ; x+= 1.0f) {
        
        /// 绘制横线
        fs_floorBatch.Vertex3f(-50.0f, -0.5f, x);
        fs_floorBatch.Vertex3f(50.0f, -0.5f, x);
        
        /// 绘制竖线
        fs_floorBatch.Vertex3f(x, -0.5f, 50.0f);
        fs_floorBatch.Vertex3f(x, -0.5f, -50.0f);
    }
    fs_floorBatch.End();
    
    gltMakeSphere(fs_bigSphereBatch, 0.4f, 40, 80);
}

void fs_changeSize(int w, int h) {
    glViewport(0, 0, w, h);
    
    fs_frustum.SetPerspective(35.0f, float(w)/float(h), 1.0f, 400.0f);
    fs_projectionMatrix.LoadMatrix(fs_frustum.GetProjectionMatrix());
    
    fs_modelViewMatrix.LoadIdentity();
    
    fs_transform.SetMatrixStacks(fs_modelViewMatrix, fs_projectionMatrix);
}

void fs_render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    /// 绘制地平线
    GLfloat vBlue[] = {0.0f, 0.0f, 1.0f, 1.0f};
    fs_shaderManager.UseStockShader(GLT_SHADER_FLAT, fs_transform.GetModelViewProjectionMatrix(), vBlue);
    fs_floorBatch.Draw();
    
    /// 开启定时器
    static CStopWatch timer;
    GLfloat yRot = timer.GetElapsedSeconds() * 60.0f;
    
    M3DVector4f vLightPos = {0.0f, 10.0f, 5.0f, 1.0f};
    GLfloat vRed[] = {1.0f, 0.0f, 0.0f, 1.0f};
    
    /// push一个矩阵其向-z方向移动3个单位
    fs_modelViewMatrix.PushMatrix();
    fs_modelViewMatrix.Translate(0.0f, 0.0f, -3.0f);
    
    /// 在-z方向上移动3个单位后在绕y轴旋转
    fs_modelViewMatrix.PushMatrix();
    fs_modelViewMatrix.Rotate(yRot, 0.0f, 1.0f, 0.0f);
    fs_shaderManager.UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF, fs_transform.GetModelViewMatrix(), fs_transform.GetProjectionMatrix(), vLightPos, vRed);
    fs_bigSphereBatch.Draw();
    
    /// polygon mode
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonOffset(-1.0, -1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(2.5F);
    
    GLfloat vBlack[] = {0.0f, 0.0f, 0.0f, 1.0f};
    fs_shaderManager.UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF, fs_transform.GetModelViewMatrix(), fs_transform.GetProjectionMatrix(), vLightPos, vBlack);
    fs_bigSphereBatch.Draw();
    
    fs_modelViewMatrix.PopMatrix();
    fs_modelViewMatrix.PopMatrix();
    
    /// reset state
    glLineWidth(1.0f);
    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_BLEND);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_POLYGON_OFFSET_LINE);
    
    glutSwapBuffers();
    glutPostRedisplay();
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
