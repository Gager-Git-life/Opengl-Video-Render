#include "opengl_viewer.hpp"


OpenGLViewer::OpenGLViewer():cam_render_texture_(-1),
                                sub_render_texture_(-1),
                                 window_width_(800),
                                 window_height_(600),
                                 first_mouse_(true),
                                 last_x_(window_width_/2.0),
                                 last_y_(window_height_/2.0f),
                                 delta_time_(0),
                                 show_stereo_camera_(true)
{
    CreateWindow();
    CreateGeometries();
    CreateShader("/home/gager/opengl_workspace/realtime_render/shaders/camera_render.vs",
                "/home/gager/opengl_workspace/realtime_render/shaders/camera_render.fs");
}


OpenGLViewer::~OpenGLViewer()
{
}


int OpenGLViewer::CreateWindow() {
    glfwInit();
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
    glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );
    window_ = glfwCreateWindow( window_width_, window_height_, "stereo visualization", nullptr, nullptr );
    if ( nullptr == window_ )
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate( );
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent( window_ );
    glewExperimental = GL_TRUE;
    if ( GLEW_OK != glewInit( ) )
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }

    int screen_width, screen_height;
    glfwGetFramebufferSize( window_, &screen_width, &screen_height );
    glViewport( 0, 0, screen_width, screen_height );
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    glfwSetWindowUserPointer(window_, this);
    glfwSetCursorPosCallback(window_,
                             +[]( GLFWwindow* window, double xpos, double ypos )
                             {
                                 static_cast<OpenGLViewer*>(glfwGetWindowUserPointer(window))->on_mouse(window,xpos,ypos);
                             }
                             );
    
    
    glfwSetFramebufferSizeCallback(window_,
                                   +[]( GLFWwindow* window, int width, int height )
                                   {
                                       static_cast<OpenGLViewer*>(glfwGetWindowUserPointer(window))->on_window_resize(window, width, height);
                                   }
                                   );
    
    glfwSetScrollCallback(window_,
                          +[]( GLFWwindow* window, double xoffset, double yoffset )
                          {
                              static_cast<OpenGLViewer*>(glfwGetWindowUserPointer(window))->on_scroll(window,xoffset,yoffset);
                          }
                          );
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    return 0;
}

int OpenGLViewer::CreateGeometries() {

    camera_ = std::unique_ptr<OpenGLCamera>(new OpenGLCamera(glm::vec3(-0.444255,0.29527,-0.218251)));

    // create opencv render rectangle
    float quad_vertices[] = {
        // positions          // colors           // texture coords
         1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,   // top right
         1.0f, -1.0f, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,   // bottom right
        -1.0f, -1.0f, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,   // bottom left
        -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f    // top left
    };
    unsigned int quad_indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    
    // std::cout << "----------" << std::endl;
    glGenVertexArrays(1, &camera_quad_vao_);
    // std::cout << "++++++++++" << std::endl;
    glGenBuffers(1, &camera_quad_vbo_);
    glGenBuffers(1, &camera_quad_ebo_);
    glBindVertexArray(camera_quad_vao_);
    glBindBuffer(GL_ARRAY_BUFFER, camera_quad_vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, camera_quad_ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices), quad_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    return 0;
}

void OpenGLViewer::CreateShader(std::string vertexPath, std::string fragmentPath){

    int result = cam_render_shader_.init(vertexPath, fragmentPath);
    if(result){
        load_shader_over = true;
    }
    else{
        load_shader_over = false;
    } 
}

void OpenGLViewer::Load_Texture(texture_config &texture){

    if (!texture.image.empty()) {
        unsigned char *data = texture.image.ptr();
        int nrChannels = texture.image.channels();
        int width = texture.image.cols;
        int height = texture.image.rows;
        if (texture.texture_num==-1)
            glGenTextures(1, &texture.texture_num);
        glBindTexture(GL_TEXTURE_2D, texture.texture_num);

        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);       
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if(texture.Types == "RGB"){
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
        }
        else{
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
        cam_render_shader_.use();
        cam_render_shader_.setInt(texture.texture_name, texture.nums);
    }
}

void OpenGLViewer::Control_Texture(){

    float params[4];
    float tone=0.8, beauty=0.7, bright=0.9;

    glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
    glBindTexture(GL_TEXTURE_2D, cam_render_texture_);
    cam_render_shader_.use();
    cam_render_shader_.setInt("width", 800);
    cam_render_shader_.setInt("height", 600);
    cam_render_shader_.setVec4f("params", 1.0 - 0.6 * beauty, 1.0 - 0.3 * beauty, 0.1 + 0.3 * tone, 0.1 + 0.3 * tone);
    cam_render_shader_.setFloat("brightness", 0.6 * (-0.5 + bright));
    glBindVertexArray( camera_quad_vao_ );
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

int OpenGLViewer::RenderFrame(cv::Mat camera_image) {
    
    if (glfwGetKey(window_, GLFW_KEY_ESCAPE ) != GLFW_PRESS)
    {
        float current_frame_time = glfwGetTime();
        delta_time_ = current_frame_time - last_frame_time_;
        last_frame_time_ = current_frame_time;
        ProcessInput();

        camera_texture.image = camera_image;
        Load_Texture(camera_texture);

        if(decorate_texture != "null"){
            cv::Mat image = cv::imread(decorate_texture, -1);
            sub_texture.image = image;
            Load_Texture(sub_texture);
        }

        // render scene
        glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT );

        // params input
        Control_Texture();

        glfwSwapBuffers( window_ );
        glfwPollEvents( );
    }
    else {
        glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        return -1;
    }
    return 0;
}

void OpenGLViewer::ProcessInput()
{
    if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window_, true);
    if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS)
        camera_->ProcessKeyboard(Camera_Movement::FORWARD, delta_time_);
    if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS)
        camera_->ProcessKeyboard(Camera_Movement::BACKWARD, delta_time_);
    if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS)
        camera_->ProcessKeyboard(Camera_Movement::LEFT, delta_time_);
    if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS)
        camera_->ProcessKeyboard(Camera_Movement::RIGHT, delta_time_);
    if (glfwGetKey(window_, GLFW_KEY_O) == GLFW_PRESS)
        camera_->ProcessKeyboard(Camera_Movement::UP, delta_time_);
    if (glfwGetKey(window_, GLFW_KEY_P) == GLFW_PRESS)
        camera_->ProcessKeyboard(Camera_Movement::DOWN, delta_time_);
}

void OpenGLViewer::terminate() {
    glDeleteVertexArrays( 1, &camera_quad_vao_ );
    glDeleteBuffers( 1, &camera_quad_vbo_ );
    glDeleteVertexArrays( 1, &point_cloud_vao_ );
    glDeleteBuffers( 1, &point_cloud_vbo_ );
    glfwTerminate( );
}

void OpenGLViewer::on_window_resize(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void OpenGLViewer::on_mouse(GLFWwindow* window, double xpos, double ypos)
{
    if (first_mouse_)
    {
        last_x_ = xpos;
        last_y_ = ypos;
        first_mouse_ = false;
    }
    float xoffset = xpos - last_x_;
    float yoffset = last_y_ - ypos;
    last_x_ = xpos;
    last_y_ = ypos;
    camera_->ProcessMouseMovement(xoffset, yoffset);
}

void OpenGLViewer::on_scroll(GLFWwindow* window, double xoffset, double yoffset)
{
    camera_->ProcessMouseScroll(yoffset);
}
