//
//  Practice.cpp
//  OpenGL_practice
//
//  Created by 天边的星星 on 2020/9/9.
//  Copyright © 2020 starxin. All rights reserved.
//

#include "Practice.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

GLShaderManager shaderManager1;
GLMatrixStack modelViewMatrix1;
GLMatrixStack projectionMatrix1;

GLFrame cameraFrame1;
GLFrame objectFrame1;

GLFrustum viewFrustum1;

GLBatch pointBatch1;
GLBatch lineBatch1;
GLBatch lineStripBatch1;
GLBatch lineFanBatch1;
GLBatch triangleBatch1;
GLBatch triangleStripBatch1;
GLBatch triangleFanBatch1;

GLGeometryTransform geometryTransform1;

GLfloat vGreen1[] = {0.0f, 1.0f, 0.0f, 1.0f};
GLfloat vBack1[] = {0.0f, 0.0f, 0.0f, 1.0f};

/// 记录空格跟踪次数
int step1 = 0;

/// 设置初始化操作
void setup(void) {
    glClearColor(0.5, 0.5, 0.5, 1.0);
    shaderManager1.InitializeStockShaders();
    glEnable(GL_DEPTH_TEST);
    
    geometryTransform1.SetMatrixStacks(modelViewMatrix1, projectionMatrix1);
    
    /// 数值越小，则视角距离越远，物体越小；越大，则视角距离越近，物理越大
    cameraFrame1.MoveForward(-15);
    
    /// 定义三点 线 三角形
    GLfloat vCoast[9] = {
        3,3,0,
        0,3,0,
        3,0,0
    };
    pointBatch1.Begin(GL_POINTS, 3);
    pointBatch1.CopyVertexData3f(vCoast);
    pointBatch1.End();
    
    lineBatch1.Begin(GL_LINES, 3);
    lineBatch1.CopyVertexData3f(vCoast);
    lineBatch1.End();
    
    lineStripBatch1.Begin(GL_LINE_STRIP, 3);
    lineStripBatch1.CopyVertexData3f(vCoast);
    lineStripBatch1.End();
    
    lineFanBatch1.Begin(GL_LINE_LOOP, 3);
    lineFanBatch1.CopyVertexData3f(vCoast);
    lineFanBatch1.End();
    
    /// 通过三角形创建金字塔顶点模型数据
    GLfloat vPyramid[12][3] = {
        2.0f, 0.0f, -2.0f,
        2.0f, 0.0f, 2.0f,
        0.0f, 4.0f, 0.0f,
        
        0.0f, 4.0f, 0.0f,
        2.0f, 0.0f, 2.0f,
        -2.0f, 0.0f, 2.0f,
        
        -2.0f, 0.0f, 2.0f,
        -2.0f, 0.0f, -2.0f,
        0.0f, 4.0f, 0.0f,
        
        -2.0f, 0.0f, -2.0f,
        0.0f, 4.0f, 0.0f,
        2.0f, 0.0f, -2.0f,
    };
    triangleBatch1.Begin(GL_TRIANGLES, 12);
    triangleBatch1.CopyVertexData3f(vPyramid);
    triangleBatch1.End();
    
    /// 通过三角形带绘制小环或者圆环
    GLfloat radius = 3.0f;
    int pointCount = 0;
    GLfloat vPoint[100][3];
    
    for (GLfloat i=0; i<=M3D_2PI; i += 0.2f) {
        vPoint[pointCount][0] = radius*cos(i);
        vPoint[pointCount][1] = radius*sin(i);
        vPoint[pointCount][2] = 0.5f;
        pointCount++;
        
        vPoint[pointCount][0] = radius*cos(i);
        vPoint[pointCount][1] = radius*sin(i);
        vPoint[pointCount][2] = -0.5f;
        pointCount++;
    }
    
    /// 最后一个点
    vPoint[pointCount][0] = radius;
    vPoint[pointCount][1] = 0;
    vPoint[pointCount][2] = 0.5f;
    pointCount++;
    
    vPoint[pointCount][0] = radius;
    vPoint[pointCount][1] = 0;
    vPoint[pointCount][2] = -0.5f;
    pointCount++;
    
    triangleStripBatch1.Begin(GL_TRIANGLE_STRIP, pointCount);
    triangleStripBatch1.CopyVertexData3f(vPoint);
    triangleStripBatch1.End();
    
    radius = 3.0f;
    pointCount = 0;
    
    /// 原点坐标
    vPoint[pointCount][0] = 0.0f;
    vPoint[pointCount][1] = 0.0f;
    vPoint[pointCount][2] = -0.5f;
    pointCount++;
    
    /// 通过三角形扇创建扇形
    for (GLfloat angle = 0; angle<M3D_2PI; angle += M3D_2PI/6.0f) {
        vPoint[pointCount][0] = radius*cos(angle);
        vPoint[pointCount][1] = radius*sin(angle);
        vPoint[pointCount][2] = -0.5f;
        pointCount++;
    }
    
    /// 最后一个点坐标
    vPoint[pointCount][0] = radius;
    vPoint[pointCount][1] = .0f;
    vPoint[pointCount][2] = -0.5f;
    pointCount++;
    
    triangleFanBatch1.Begin(GL_TRIANGLE_FAN, pointCount);
    triangleFanBatch1.CopyVertexData3f(vPoint);
    triangleFanBatch1.End();
}

/// 视图窗口大小变化
void viewportSizeChange(int w, int h) {
    glViewport(0, 0, w, h);
    
    /// 窗口大小变化后其投影矩阵(正交投影/透视投影)也会变化
    GLfloat aspect = GLfloat(w) / GLfloat(h);
    viewFrustum1.SetPerspective(50.f, aspect, 2.0f, -500.0f);
    projectionMatrix1.LoadMatrix(viewFrustum1.GetProjectionMatrix());
    
    /// 顶部元素压入单元矩阵，可以不用，因为系统初始化源码就压入了单元矩阵
    modelViewMatrix1.LoadIdentity();
    
    glutPostRedisplay();
}

/// 针对复杂三角形渲染排除z-fighting
void drawWireBatch(GLBatch *batch) {
    shaderManager1.UseStockShader(GLT_SHADER_FLAT, geometryTransform1.GetModelViewProjectionMatrix(), vGreen1);
    batch->Draw();
    
    /// z-fighting处理
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(-1.0f, -1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(2.5f);
    
    /// 反锯齿
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    shaderManager1.UseStockShader(GLT_SHADER_FLAT, geometryTransform1.GetModelViewProjectionMatrix(), vBack1);
    batch->Draw();
    
    /// 复原状态机
    glLineWidth(1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_POLYGON_OFFSET_LINE);
    glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);
}

/// 渲染
void render(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    modelViewMatrix1.PushMatrix();
    M3DMatrix44f cameraM;
    cameraFrame1.GetCameraMatrix(cameraM);
    modelViewMatrix1.LoadMatrix(cameraM);
    
    M3DMatrix44f objectM;
    objectFrame1.GetMatrix(objectM);
    modelViewMatrix1.MultMatrix(objectM);
    
    shaderManager1.UseStockShader(GLT_SHADER_FLAT, geometryTransform1.GetModelViewProjectionMatrix(), vBack1);
    
    /// 根据step进行不同类型渲染
    switch (step1) {
        case 0://点渲染
            glPointSize(4.0f);
            pointBatch1.Draw();
            glPointSize(1.0f);
            break;
        case 1://线渲染
            glLineWidth(3.0f);
            lineBatch1.Draw();
            glLineWidth(1.0f);
            break;
        case 2://线带渲染
            glLineWidth(3.0f);
            lineStripBatch1.Draw();
            glLineWidth(1.0f);
            break;
        case 3://线环渲染
            glLineWidth(3.0f);
            lineFanBatch1.Draw();
            glLineWidth(1.0f);
            break;
        case 4://三角形渲染
            drawWireBatch(&triangleBatch1);
            break;
        case 5://三角形带渲染
            drawWireBatch(&triangleStripBatch1);
            break;
        case 6://三角形扇渲染
            drawWireBatch(&triangleFanBatch1);
            break;
        default:
            break;
    }
    modelViewMatrix1.PopMatrix();
    
    glutSwapBuffers();
}

/// 特殊键位控制处理 这里是键盘上、下、左、右
void specialKey(int key, int x, int y) {
    if (key == GLUT_KEY_UP) {
        objectFrame1.RotateWorld(m3dDegToRad(-5.0f), 1.0f, 0.0f, 0.0f);
    }
    else if (key == GLUT_KEY_DOWN) {
        objectFrame1.RotateWorld(m3dDegToRad(5.0f), 1.0f, 0.0f, 0.0f);
    }
    else if (key == GLUT_KEY_LEFT) {
        objectFrame1.RotateWorld(m3dDegToRad(-5.0f), 0.0f, 1.0f, 0.0f);
    }
    else if (key == GLUT_KEY_RIGHT) {
        objectFrame1.RotateWorld(m3dDegToRad(5.0f), 0.0f, 1.0f, 0.0f);
    }
    else {}
    
    glutPostRedisplay();
}

/// 键盘按键处理 这里只处理空格键，其ascii码对应是32
void keyPressFunc(unsigned char key, int x, int y) {
    
    if (key == 32) {
        step1++;
        if (step1 > 6) {
            step1 = 0;
        }
    }
    
    switch (step1) {
        case 0:
            glutSetWindowTitle("GL_POINTS");
            break;
        case 1:
            glutSetWindowTitle("GL_LINES");
            break;
        case 2:
            glutSetWindowTitle("GL_LINE_STRIP");
            break;
        case 3:
            glutSetWindowTitle("GL_LINE_LOOP");
            break;
        case 4:
            glutSetWindowTitle("GL_TRIANGLES");
            break;
        case 5:
            glutSetWindowTitle("GL_TRIANGLE_STRIP");
            break;
        case 6:
            glutSetWindowTitle("GL_TRIANGLE_FAN");
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

/// main函数入口
int testForComplicatedShapes(int argc, char *argv[]) {
    gltSetWorkingDirectory(argv[0]);
    glutInit(&argc, argv);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100, 100);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutCreateWindow("GL_POINTS");
    
    glutReshapeFunc(viewportSizeChange);
    glutSpecialFunc(specialKey);
    glutKeyboardFunc(keyPressFunc);
    glutDisplayFunc(render);
    
    GLenum success = glewInit();
    if (success != GLEW_OK) {
        fprintf(stderr, "glew error: %s\n", glewGetErrorString(success));
        return 1;
    }
    
    ///绘制
    setup();

    glutMainLoop();
    return 0;
}

#pragma clang diagnostic pop
