//
//  Texture.cpp
//  OpenGL_practice
//
//  Created by 天边的星星 on 2020/9/29.
//  Copyright © 2020 starxin. All rights reserved.
//
#include "Texture.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

GLShaderManager t_shaderManager;

GLFrame t_objectFrame;
GLFrame t_cameraFrame;

GLFrustum t_frustum;

GLMatrixStack t_modelViewMatrix;
GLMatrixStack t_projectionMatrix;

GLuint t_textureId;
GLBatch t_pyramidBatch;

GLGeometryTransform t_transform;

bool t_loadTGATexture(const char *tgaFileName, GLint minFiler, GLint magFilter, GLenum wrapMode) {
    
    GLint width, height, components;
    GLenum format;
    
    /// 读取纹理字节
    GLbyte *pixels = gltReadTGABits(tgaFileName, &width, &height, &components, &format);
    if (pixels == NULL) {
        return false;
    }
    
    /// 设置纹理参数：过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFiler);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    
    /// 设置纹理参数之环绕参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
    
    /// 加载纹理
    glTexImage2D(GL_TEXTURE_2D, 0, components, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
    
    /// 销毁纹理内存数据
    free(pixels);
    
    ///只有当minFilter等于以下四种模式，才可以生成Mip贴图
    ///GL_NEAREST_MIPMAP_NEAREST具有非常好的性能，并且闪烁现象非常弱
    ///GL_LINEAR_MIPMAP_NEAREST常常用于对游戏进行加速，它使用了高质量的线性过滤器
    ///GL_NEAREST_MIPMAP_LINEAR和GL_LINEAR_MIPMAP_LINEAR 过滤器在Mip层之间执行了一些额外的插值，以消除它们之间的过滤痕迹
    ///GL_LINEAR_MIPMAP_LINEAR 三线性Mip贴图，纹理过滤的黄金准则，具有最高的精度
    if (minFiler == GL_NEAREST_MIPMAP_NEAREST ||
        minFiler == GL_NEAREST_MIPMAP_LINEAR  ||
        minFiler == GL_LINEAR_MIPMAP_NEAREST  ||
        minFiler == GL_LINEAR_MIPMAP_LINEAR) {
        
        /// 加载Mip map
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    
    return true;
}
void t_makePyramid(GLBatch &batch) {
    
    M3DVector3f vBackLeft = {-1.0f, -1.0f, -1.0f};
    M3DVector3f vBackRight = {1.0f, -1.0f, -1.0f};
    M3DVector3f vFrontLeft = {-1.0f, -1.0f, 1.0f};
    M3DVector3f vFrontRight = {1.0f, -1.0f, 1.0f};
    M3DVector3f vApex = {0.0f, 1.0f, 0.0f};
    
    batch.Begin(GL_TRIANGLES, 18, 1);
    
    /// 定义法线向量，以便光照时有阴影
    M3DVector3f normal;

    //1.找到三角形X 法线 如果出现一个面出现全黑，则求法线出问题了，则需要交换求法线的坐标顺序
     m3dFindNormal(normal, vBackLeft, vBackRight, vFrontRight);
    
     //vBackLeft
     batch.Normal3fv(normal);
     batch.MultiTexCoord2f(0, 0.0f, 0.0f);
     batch.Vertex3fv(vBackLeft);
     
     //vFrontRight
     batch.Normal3fv(normal);
     batch.MultiTexCoord2f(0, 1.0f, 1.0f);
     batch.Vertex3fv(vFrontRight);
     
     //vBackRight
     batch.Normal3fv(normal);
     batch.MultiTexCoord2f(0, 1.0f, 0.0f);
     batch.Vertex3fv(vBackRight);
    
     //三角形Y =(vFrontLeft,vBackLeft,vFrontRight)
    
     //1.找到三角形X 法线
     m3dFindNormal(normal, vFrontLeft, vBackLeft, vFrontRight);
     
     //vFrontLeft
     batch.Normal3fv(normal);
     batch.MultiTexCoord2f(0, 0.0f, 1.0f);
     batch.Vertex3fv(vFrontLeft);
     
     //vBackLeft
     batch.Normal3fv(normal);
     batch.MultiTexCoord2f(0, 0.0f, 0.0f);
     batch.Vertex3fv(vBackLeft);
     
     //vFrontRight
     batch.Normal3fv(normal);
     batch.MultiTexCoord2f(0, 1.0f, 1.0f);
     batch.Vertex3fv(vFrontRight);

    /// front
    m3dFindNormal(normal, vApex, vFrontLeft, vFrontRight);
    batch.Normal3fv(normal);
    batch.MultiTexCoord2f(0, 0.5f, 1.0f);
    batch.Vertex3fv(vApex);

    batch.Normal3fv(normal);
    batch.MultiTexCoord2f(0, 0.0f, 0.0f);
    batch.Vertex3fv(vFrontLeft);

    batch.Normal3fv(normal);
    batch.MultiTexCoord2f(0, 1.0f, 0.0f);
    batch.Vertex3fv(vFrontRight);

    /// right
    m3dFindNormal(normal, vApex, vFrontRight, vBackRight);
    batch.MultiTexCoord2f(0, 0.5f, 1.0f);
    batch.Normal3fv(normal);
    batch.Vertex3fv(vApex);

    batch.MultiTexCoord2f(0, 0.0f, 0.0f);
    batch.Normal3fv(normal);
    batch.Vertex3fv(vFrontRight);

    batch.MultiTexCoord2f(0, 1.0f, 0.0f);
    batch.Normal3fv(normal);
    batch.Vertex3fv(vBackRight);

    /// back
    m3dFindNormal(normal, vApex, vBackRight, vBackLeft);
    batch.Normal3fv(normal);
    batch.MultiTexCoord2f(0, 0.5f, 1.0f);
    batch.Vertex3fv(vApex);

    batch.Normal3fv(normal);
    batch.MultiTexCoord2f(0, 1.0f, 0.0f);
    batch.Vertex3fv(vBackLeft);

    batch.Normal3fv(normal);
    batch.MultiTexCoord2f(0, 0.0f, 0.0f);
    batch.Vertex3fv(vBackRight);

    /// left
    m3dFindNormal(normal, vApex, vBackLeft, vFrontLeft);
    batch.Normal3fv(normal);
    batch.MultiTexCoord2f(0, 0.5f, 1.0f);
    batch.Vertex3fv(vApex);

    batch.Normal3fv(normal);
    batch.MultiTexCoord2f(0, 0.0f, 0.0f);
    batch.Vertex3fv(vFrontLeft);

    batch.Normal3fv(normal);
    batch.MultiTexCoord2f(0, 1.0f, 0.0f);
    batch.Vertex3fv(vBackLeft);

    batch.End();
}

void t_setup() {
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    
    glEnable(GL_DEPTH_TEST);
    t_shaderManager.InitializeStockShaders();
    
    t_cameraFrame.MoveForward(-10.0f);
    
    /// 创建纹理对象
    glGenTextures(1, &t_textureId);
    /// 绑定纹理
    glBindTexture(GL_TEXTURE_2D, t_textureId);
    
    /// 载入纹理并设置纹理参数
    bool success = t_loadTGATexture("stone.tga", GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR, GL_CLAMP_TO_EDGE);
    
    if (!success) {
        printf("load tga texture failed\n");
        return;
    }
    
    /// 加载纹理坐标和金字塔顶点坐标
    t_makePyramid(t_pyramidBatch);
}

void t_render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    GLfloat vWhite[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat vLightPos[] = {1.0f, 1.0f, 0.0f};
    
    t_modelViewMatrix.PushMatrix();
    
    /// 视觉坐标
    M3DMatrix44f cameraM;
    t_cameraFrame.GetCameraMatrix(cameraM);
    t_modelViewMatrix.LoadMatrix(cameraM);
    
    /// 对象坐标*视觉坐标=》MV
    M3DMatrix44f objectM;
    t_objectFrame.GetCameraMatrix(objectM);
    t_modelViewMatrix.MultMatrix(objectM);
    
    /// 再次绑定纹理，单个纹理不需要，多个则需要重新绑定
    glBindTexture(GL_TEXTURE_2D, t_textureId);
    
    t_shaderManager.UseStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF, t_transform.GetModelViewMatrix(), t_transform.GetProjectionMatrix(), vLightPos, vWhite, 0);
    
    t_pyramidBatch.Draw();
    t_modelViewMatrix.PopMatrix();
    
    glutSwapBuffers();
}
void t_specialKey(int key, int x, int y) {
    
    GLfloat angle = 5.0f;
    if (key == GLUT_KEY_UP) {
        t_objectFrame.RotateWorld(m3dDegToRad(-angle), 1.0f, 0.0f, 0.0f);
    }
    else if (key == GLUT_KEY_DOWN) {
        t_objectFrame.RotateWorld(m3dDegToRad(angle), 1.0f, 0.0f, 0.0f);
    }
    else if (key == GLUT_KEY_LEFT) {
        t_objectFrame.RotateWorld(m3dDegToRad(-angle), 0.0f, 1.0f, 0.0f);
    }
    else if (key == GLUT_KEY_RIGHT) {
        t_objectFrame.RotateWorld(m3dDegToRad(angle), 0.0f, 1.0f, 0.0f);
    }
    
    glutPostRedisplay();
}
void t_shutdownRC(void) {
    glDeleteTextures(1, &t_textureId);
}
void t_reshape(int w, int h) {
    
    if (h == 0) {
        h = 1;
    }
    glViewport(0, 0, w, h);
    
    t_frustum.SetPerspective(35.0f, float(w)/float(h), 1.0f, 100.0f);
    t_projectionMatrix.LoadMatrix(t_frustum.GetProjectionMatrix());
    
    t_transform.SetMatrixStacks(t_modelViewMatrix, t_projectionMatrix);
}

int t_main(int argc, char *argv[]) {
    gltSetWorkingDirectory(argv[0]);
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE | GLUT_STENCIL);
    glutInitWindowSize(800, 800);
    glutCreateWindow("pyramid");
    
    glutDisplayFunc(t_render);
    glutSpecialFunc(t_specialKey);
    glutReshapeFunc(t_reshape);
    
    GLenum success = glewInit();
    if (success != GLEW_OK) {
        fprintf(stderr, "glew init error: %s\n", glewGetErrorString(success));
        return 1;
    }
    
    t_setup();
    glutMainLoop();
    t_shutdownRC();
    
    return 0;
}

#pragma clang diagnostic pop
