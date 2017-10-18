#include "opencv2/opencv.hpp"
// #include <cv.h>
#include <stdio.h>
// #include <highgui.h>

using namespace cv;
using namespace std;

int main(int argc,char **argv){
    // 2017-10-06T15:41:08.755369_original_image 
    // VideoCapture cap("c:/fullpath/Image_%03d.png");
    // VideoCapture cap(inputpath);
    vector<String> filenames;
    glob(argv[1], filenames);

    if(!filenames.size()){
        cout << "Empty folder / wrong path!" << endl;
        return -1;
      }

    Mat frame=imread(filenames[0]);
    string output_name=argv[1]+string(".avi");
    VideoWriter output_video(output_name,CV_FOURCC('M','J','P','G'), 5, Size(frame.cols,frame.rows));
    namedWindow("Display frame",CV_WINDOW_NORMAL);
    // resizeWindow("Display frame", 600, 600);    
    for(string filename:filenames){
        frame=imread(filename);
        output_video<<frame;
        if(!frame.data)
            cerr << "Error loading image!" << endl;
        
        imshow("Display frame", frame);

        // Esc key to stop 
        if(waitKey(30)==27)
            break;

    }

    frame.release();
    destroyAllWindows();
}
