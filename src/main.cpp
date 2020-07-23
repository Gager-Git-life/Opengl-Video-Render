#include "opengl_viewer.hpp"

int main( int argc, const char** argv )
{
    const int deviceid = 1;
    cv::VideoCapture capture(-1);

    if (!capture.isOpened())
        capture.open(deviceid);
    
    if (!capture.isOpened()) {
        std::cerr << "Failed to open the video device, video file or image sequence!\n" << std::endl;
        return 1;
    }

    OpenGLViewer viewer;
    cv::Mat frame;
    char key = -1;
    while (key != 27 && !viewer.ShouldClose()) {
        capture >> frame;
        if (frame.empty())
            break;
        else {
            viewer.RenderFrame(frame);
        }
        key = (char)cv::waitKey(11);
    }
    viewer.terminate();
    return 0;
}
