//#include <GL/glew.h>
//#include <GLFW/glfw3.h>
//#include <iostream>
//
//void CheckFramebufferStatus()
//{
//    GLuint fbo;
//    glGenFramebuffers(1, &fbo);
//    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
//
//    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
//    if (status == GL_FRAMEBUFFER_COMPLETE)
//    {
//        std::cout << "✅ Framebuffer 생성 성공!" << std::endl;
//    }
//    else
//    {
//        std::cout << "🚨 Framebuffer 오류 발생! 상태 코드: " << status << std::endl;
//    }
//
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//    glDeleteFramebuffers(1, &fbo);
//}
//
//int main()
//{
//    // GLFW 초기화
//    if (!glfwInit())
//    {
//        std::cerr << "🚨 GLFW 초기화 실패!" << std::endl;
//        return -1;
//    }
//
//    // OpenGL 컨텍스트 생성을 위한 윈도우 생성
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);  // OpenGL 4.x 사용
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL GLEW 테스트", nullptr, nullptr);
//    if (!window)
//    {
//        std::cerr << "🚨 GLFW 윈도우 생성 실패!" << std::endl;
//        glfwTerminate();
//        return -1;
//    }
//
//    // OpenGL 컨텍스트를 현재 윈도우로 설정
//    glfwMakeContextCurrent(window);
//
//    // GLEW 초기화
//    glewExperimental = GL_TRUE;
//    if (glewInit() != GLEW_OK)
//    {
//        std::cerr << "🚨 GLEW 초기화 실패!" << std::endl;
//        glfwDestroyWindow(window);
//        glfwTerminate();
//        return -1;
//    }
//
//    // OpenGL 버전 출력
//    const char* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
//    std::cout << "✅ OpenGL Version: " << version << std::endl;
//
//    // 프레임버퍼 테스트 실행
//    CheckFramebufferStatus();
//
//    // GLFW 정리
//    glfwDestroyWindow(window);
//    glfwTerminate();
//
//    return 0;
//}
