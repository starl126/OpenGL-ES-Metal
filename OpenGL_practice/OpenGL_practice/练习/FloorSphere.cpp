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
/// 地平线对象坐标
GLFrame fs_floorObjectFrame;
/// 大圆对象坐标
GLFrame fs_bigSphereObjectFrame;

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
        fs_floorBatch.Vertex3f(-50.0f, -1.5f, x);
        fs_floorBatch.Vertex3f(50.0f, -1.5f, x);
        
        /// 绘制竖线
        fs_floorBatch.Vertex3f(x, -1.5f, 50.0f);
        fs_floorBatch.Vertex3f(x, -1.5f, -50.0f);
    }
    fs_floorBatch.End();
    
    gltMakeSphere(fs_bigSphereBatch, 0.5f, 60, 20);
    
    gltMakeSphere(fs_smallSphereBatch, 0.5f, 40, 20);
    for (int i=0; i<kNumberSpheres; i++) {
        GLfloat x = float(rand()%100-50);
        GLfloat z = float(rand()%40+10);
        fs_sphereFrames[i].SetOrigin(x, 0.0f, -z);
    }
    
    /// 添加菜单
    void fs_createMenu(int value);
    glutCreateMenu(fs_createMenu);
    glutAddMenuEntry("正背面剔除", 0);
    glutAddMenuEntry("深度测试", 1);
    glutAddMenuEntry("线填充", 2);
    glutAddMenuEntry("点填充", 3);
    glutAddMenuEntry("fill填充", 4);
    glutAddMenuEntry("z-fighting", 5);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void fs_createMenu(int value) {
    switch (value) {
        case 0:
            fs_isCull = !fs_isCull;
            glEnable(GL_CULL_FACE);
            if (fs_isCull) {
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);
                glFrontFace(GL_CCW);
            } else {
                glDisable(GL_CULL_FACE);
            }
            break;
        case 1:
            fs_isDepthTest = !fs_isDepthTest;
            if (fs_isDepthTest) {
                glEnable(GL_DEPTH_TEST);
            } else {
                glDisable(GL_DEPTH_TEST);
            }
            break;
        case 2:
        {
            glPolygonMode(GL_FRONT, GL_LINE);
        }
            break;
        case 3:
        {
            glPolygonMode(GL_FRONT, GL_POINT);
        }
            break;
        case 4:
        {
            glPolygonMode(GL_FRONT, GL_FILL);
        }
            break;
        case 5:
        {
            fs_zFighting = !fs_zFighting;
            if (fs_zFighting) {
                glEnable(GL_POLYGON_OFFSET_FILL);
                glPolygonOffset(-1, -1);
            } else {
                glDisable(GL_POLYGON_OFFSET_FILL);
            }
        }
            break;
        default:
            break;
    }
    
    glutPostRedisplay();
}

void fs_changeSize(int w, int h) {
    glViewport(0, 0, w, h);
    
    fs_frustum.SetPerspective(35.0f, float(w)/float(h), 1.0f, 400.0f);
    fs_projectionMatrix.LoadMatrix(fs_frustum.GetProjectionMatrix());
    
    fs_transform.SetMatrixStacks(fs_modelViewMatrix, fs_projectionMatrix);
}

void fs_render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    /// floor
    fs_modelViewMatrix.PushMatrix(fs_floorObjectFrame);
    GLfloat vBlue[4] = {0.0f, 0.0f, 1.0f, 1.0f};
    fs_shaderManager.UseStockShader(GLT_SHADER_FLAT, fs_transform.GetModelViewProjectionMatrix(), vBlue);
    fs_floorBatch.Draw();
    fs_modelViewMatrix.PopMatrix();
    
    /// big sphere
    fs_modelViewMatrix.PushMatrix(fs_bigSphereObjectFrame);
    GLfloat vRed[] = {1.0f, 0.0f, 0.0f, 1.0f};
    GLfloat vLight[] = {0.0f, 10.0f, 0.0f};
    fs_modelViewMatrix.Translate(0, 0, 3);
    fs_shaderManager.UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF, fs_transform.GetModelViewMatrix(), fs_transform.GetProjectionMatrix(),vLight, vRed);
    fs_bigSphereBatch.Draw();
    fs_modelViewMatrix.PopMatrix();

    /// several small sphere
    for (int i=0; i<kNumberSpheres; i++) {
        GLFrame frame = fs_sphereFrames[i];
        fs_modelViewMatrix.PushMatrix(frame);
        fs_shaderManager.UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF, fs_transform.GetModelViewMatrix(), fs_transform.GetProjectionMatrix(), vLight, vRed);
        fs_smallSphereBatch.Draw();
        fs_modelViewMatrix.PopMatrix();
    }

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
