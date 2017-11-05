#include "opencv2/opencv.hpp"
#include <iostream>
// #include <highgui.h>

using namespace cv;
using namespace std;

int main(int argc,char **argv){
    if(argc!=3)
    {
        cout<<"Input format incorrect!"<<endl;
	return -1;
    }
    vector<String> filenames;
    glob(argv[1]+string("/*.jpg"), filenames);

    if(!filenames.size()){
        cout << "Empty folder / wrong path!" << endl;
        return -1;
    }

    Mat frame=imread(filenames[0]);
    VideoWriter output_video(argv[2],
	CV_FOURCC('M','J','P','G'), 
	5, 
	Size(frame.cols/10,frame.rows/10)
    );
    // namedWindow("Display frame",CV_WINDOW_NORMAL);
    // resizeWindow("Display frame", 600, 600);    
    int cnt=0;
    for(string filename:filenames){
        frame=imread(filename);
	Mat frame_mini;
	resize(frame, frame_mini, Size(frame.cols/10,frame.rows/10));
	putText(frame_mini, to_string(cnt), Point(30, 30), FONT_HERSHEY_COMPLEX, 1, Scalar(255, 0, 0));   
	output_video<<frame_mini;
	cnt++;
        if(!frame.data)
            cerr << "Error loading image!" << endl;
        
        // imshow("Display frame", frame);

        // Esc key to stop 
        // if(waitKey(30)==27)
        //    break;

    }

    frame.release();
    destroyAllWindows();
}
