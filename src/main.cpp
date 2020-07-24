#include "opengl_viewer.hpp"
#include "UltraFace.hpp"
#include "Pfld.hpp"

std::string ultraface_mnn_path = "./models/slim-320.mnn";
std::string pfld_mnn_path = "./models/pfld-lite.mnn";
UltraFace ultraface(ultraface_mnn_path, 320, 240, 4, 0.65); // config model input
Pfld pfld(pfld_mnn_path, 8);

cv::Mat landmark_frmae(cv::Mat frame){
    vector<FaceInfo> face_info;
    ultraface.detect(frame, face_info);

    for (auto face : face_info) {
        cv::Point pt1(face.x1, face.y1);
        cv::Point pt2(face.x2, face.y2);
        //cv::rectangle(frame, pt1, pt2, cv::Scalar(0, 255, 0), 2);

        // pfld
        cv::Mat resize_img, resize_img_copy;
        cv::Point s_point;
        cv::Size croped_wh;
        vector<LandmarkInfo> landmarks;

        resize_img = pfld.Get_Resize_Croped_Img(frame, resize_img_copy, pt1, pt2, s_point, croped_wh);
        if(!resize_img.data){
            continue;
        }
        pfld.Get_Landmark_Points(resize_img, resize_img_copy, croped_wh, s_point, landmarks);
        //string croped_landmark_name = "croped_landmark.jpg";
        //cv::imwrite(croped_landmark_name, resize_img_copy);
        pfld.Pic_Landmark(frame, landmarks);
        
    }
    return frame;
}

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
            frame = landmark_frmae(frame);
            viewer.RenderFrame(frame);
        }
        key = (char)cv::waitKey(11);
    }
    viewer.terminate();
    return 0;
}
