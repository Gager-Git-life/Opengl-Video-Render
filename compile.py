import os
import sys



if __name__ == "__main__":
    compile_file = sys.argv[1]
    cmd = "g++ -g -o out {} src/opengl_viewer.cpp src/opengl_shader.cpp src/opengl_camera.cpp $(pkg-config --cflags --libs opencv4)".format(compile_file) + \
          " -lglfw3 -lGL -lGLEW -lm -lXrandr -lXi -lX11 -lXxf86vm -ldl -lpthread -lXinerama -lXcursor"
    print('[INFO]>>> {}'.format(cmd))
    os.system(cmd)
