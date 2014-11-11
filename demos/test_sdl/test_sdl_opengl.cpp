#include <iostream>
#include <fstream>
#include <string>

#include <gl/glew.h>

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <gl/glu.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>

const size_t SCREEN_WIDTH = 640;
const size_t SCREEN_HEIGHT = 480;

bool init();
bool initOpenGL();
void handleKeys(unsigned char key, int x, int y);
void update();
void render();
void close();

void printProgramLog(GLuint program);
void printShaderLog(GLuint shader);

SDL_Window* g_window = NULL;
SDL_GLContext g_context;

bool g_renderScene = true;

GLuint g_programId = 0;
GLuint g_vbo = 0;
GLuint g_vao = 0;

bool init() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "[ERROR] Could not initialize SDL: " 
      << SDL_GetError() << std::endl;
    return false;
  }

  // Have SDL use OpenGL 3.2.
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  g_window = SDL_CreateWindow( "Test SDL and modern OpenGL", 
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
    SCREEN_WIDTH, SCREEN_HEIGHT, 
    SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
  if (!g_window) {
    std::cerr << "[Error] Could not create SDL window: "
      << SDL_GetError() << std::endl;
      return false;
  }

  g_context = SDL_GL_CreateContext(g_window);
  if (!g_context) {
    std::cerr << "[Error] Could not set up OpenGL context: "
      << SDL_GetError() << std::endl;
    return false;
  }

  glewExperimental = GL_TRUE;
  GLenum glewStatus = glewInit();
  if (glewStatus != GLEW_OK) {
    std::cerr << "[Error] Could not initialize glew: "
      << glewGetErrorString(glewStatus) << std::endl;
      return false;
  }

  // Vsync
  if (SDL_GL_SetSwapInterval(1) < 0) {
    std::cerr << "[Warning] Unable to use vsync. Message: "
      << SDL_GetError() << std::endl;
  }

  if (!initOpenGL()) {
    std::cerr << "[Error] Init OpenGL failed. See previous messages."
      << std::endl;
    return false;
  }

  return true;
}

bool initOpenGL() {

  const GLubyte* renderer = glGetString(GL_RENDERER);
  const GLubyte* version = glGetString(GL_VERSION);
  std::cerr << "[LOG] Renderer: " << renderer << std::endl;
  std::cerr << "[LOG] OpenGL version supported: " << version << std::endl;

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  // Gee-I'm-A-Tree.
  float points[] = {
     0.25f,  0.25f, 0.0f,
     0.25f, -0.25f, 0.0f,
    -0.25f, -0.25f, 0.0f,

    -0.25f, -0.25f, 0.0f,
    -0.25f,  0.25f, 0.0f,
     0.25f,  0.25f, 0.0f
  };

  g_vbo = 0;
  glGenBuffers(1, &g_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
  glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), points, GL_STATIC_DRAW);

  g_vao = 0;
  glGenVertexArrays(1, &g_vao);
  glBindVertexArray(g_vao);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  // Load vertex shader source.
  std::ifstream vsFile;
  std::string vsLines;
  vsFile.open("vs.glsl");
  if (!vsFile.is_open()) {
    std::cerr << "[Error] Could not open vs.glsl";
  } else {
    std::string line;
    while(getline(vsFile, line)) {
      vsLines += line + "\n";
    }
    vsFile.close();
  }
  const char* vsSource = vsLines.c_str();

  // Load fragment shader source.
  std::ifstream fsFile;
  std::string fsLines;
  fsFile.open("fs.glsl");
  if (!fsFile.is_open()) {
    std::cerr << "[Error] Could not open fs.glsl";
  } else {
    std::string line;
    while(getline(fsFile, line)) {
      fsLines += line + "\n";
    }
    fsFile.close();
  }
  const char* fsSource = fsLines.c_str();  

  // Compile shaders.
  GLint shaderCompileStatus;

  // Vertex shader.
  shaderCompileStatus = GL_TRUE;
  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vsSource, NULL);
  glCompileShader(vertexShader);
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &shaderCompileStatus);
  if (shaderCompileStatus != GL_TRUE) {
    std::cerr << "[Error] Could not compile vertex shader. Shader source:" << std::endl;
    std::cerr << vsSource << std::endl;
    return false;
  }

  // Fragment shader.
  shaderCompileStatus = GL_TRUE;
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fsSource, NULL);
  glCompileShader(fragmentShader);  
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &shaderCompileStatus);
  if (shaderCompileStatus != GL_TRUE) {
    std::cerr << "[Error] Could not compile fragment shader. Shader source:" << std::endl;
    std::cerr << fsSource << std::endl;
    return false;
  }

  // Build program.
  g_programId = glCreateProgram();
  glAttachShader(g_programId, vertexShader);  
  glAttachShader(g_programId, fragmentShader);
  glLinkProgram(g_programId);

  // Program
  GLint programStatus = GL_TRUE;
  glGetProgramiv(g_programId, GL_LINK_STATUS, &programStatus);
  if (programStatus != GL_TRUE) {
    std::cerr << "[Error] Could not link shader programs. Details:" << std::endl;
    printProgramLog(g_programId);
    return false;
  }

  // Other GL state.
  glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
  return true;
}

void handleKeys(unsigned char key, int x, int y) {
  if (key == 'q') {
    g_renderScene = !g_renderScene;
    std::cerr << "[DEBUG] g_renderScene = "
      << (g_renderScene ? "True" : "False") << std::endl;
  }
}

void update() {
  // Doesn't need to do anything yet.
}

void render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

  if (g_renderScene) {
      glUseProgram(g_programId);
      glBindVertexArray(g_vao);
      glDrawArrays(GL_TRIANGLES, 0, 6);
  }
}

void close() {
  glDeleteProgram(g_programId);

  SDL_DestroyWindow(g_window);
  g_window = NULL;

  SDL_Quit();
}

void printProgramLog(GLuint program) {
  //Make sure name is shader
  if (glIsProgram(program)) {

    //Program log length
    int infoLogLength = 0;
    int maxLength = infoLogLength;
    
    //Get info string length
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
    
    //Allocate string
    char* infoLog = new char[maxLength];
    
    //Get info log
    glGetProgramInfoLog(program, maxLength, &infoLogLength, infoLog);
    if(infoLogLength > 0) {
      //Print Log
      std::cerr << "[LOG] " << infoLog << std::endl;
    } else {
      std::cerr << "[LOG] (no message)" << infoLog << std::endl;
    }
    
    //Deallocate string
    delete[] infoLog;
  }
  else {
    std::cerr << "[Error] Name " << program << " is not a program" << std::endl;
  }
}


void printShaderLog(GLuint shader) {
  //Make sure name is shader
  if(glIsShader(shader)) {
    
    //Shader log length
    int infoLogLength = 0;
    int maxLength = infoLogLength;
    
    //Get info string length
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
    
    //Allocate string
    char* infoLog = new char[maxLength];
    
    //Get info log
    glGetShaderInfoLog( shader, maxLength, &infoLogLength, infoLog );
    if( infoLogLength > 0 ) {
      std::cerr << "[LOG] " << infoLog << std::endl;
    }

    //Deallocate string
    delete[] infoLog;
  }
  else {
    std::cerr << "[Error] Name " << shader << " is not a shader" << std::endl;
  }
}

int main(int argc, char* argv[]) {
  if (!init()) {
    std::cerr << "[Error] init() failed." << std::endl;
    return -1;
  } else {
    std::cerr << "[LOG] init() complete. Start render loop." << std::endl;
    bool quit = false;
    SDL_Event e;
    SDL_StartTextInput();

    while (!quit) {
      while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
          quit = true;
        } else if (e.type == SDL_TEXTINPUT) {
          int x = 0, y = 0;
          SDL_GetMouseState(&x, &y);
          handleKeys(e.text.text[0], x, y);
        }
      }

      render();
      SDL_GL_SwapWindow(g_window);
    }

    SDL_StopTextInput();
  }

  close();
  return 0;
}
