#include <iostream>
#include "opencv2/core/core.hpp"
#include <opencv2/highgui.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/features2d.hpp"
#include <cv.h>
#include <math.h>
#include <cmath>
#include <string>
#include <iostream>

using namespace cv;
using namespace std;



void showImage(string name,Mat img, bool print=0, string path=""){
    cv::namedWindow(name, WINDOW_NORMAL);
    cv::imshow(name, img);
    cv::resizeWindow(name, 800,800);
    if(print == 1){
        path.append(name);
        path.append(".png");
        imwrite(path, img);
        //cout << "writing to path: " << path << endl;
    }
}

Mat getMask(Mat img,Scalar minimumHSV, Scalar maxHSV){
    Mat mask;

    //create a mask using the thresholded range
    inRange(img, minimumHSV, maxHSV,mask);
    return mask;
}

void seq1Algo(Mat img1, string pathToWrite = ""){
    bool print = 1;
    if(pathToWrite.empty()){
        print = 0;
    }
    //Mat img1 = cv::imread("/home/kasper/qtworkspace/markerImages/sequence_1/marker_color_01.png", CV_LOAD_IMAGE_COLOR);
    showImage("imageTest", img1);

    Mat hsvImg;
    Mat colorSeg;

    //convert to HSV
    cvtColor(img1,hsvImg, COLOR_BGR2HSV);

    //Pixelvalues
    Vec3b orangePixel(39,250,192); //HSV pixel values, from samples in the image
    Vec3b bluePixel(77,162,157);

    //Thresholds used for Easy marker
    /*
    Vec3b hsvOrangeMaxThresh(10,10,10);
    Vec3b hsvOrangeMinThresh(10,10,10);

    Vec3b hsvBlueMaxThresh(60,35,20);
    Vec3b hsvBlueMinThresh(20,50,80);
    */



    //Thresholds used for Hard marker
    Vec3b hsvOrangeMaxThresh(10,20,20);
    Vec3b hsvOrangeMinThresh(10,10,10);

    Vec3b hsvBlueMaxThresh(70,35,20);
    Vec3b hsvBlueMinThresh(30,50,80);


    Mat orange = getMask(hsvImg,cv::Scalar(0, 90, 90), cv::Scalar(14, 255, 255)); //ORANGE
    Mat blue = getMask(hsvImg,  cv::Scalar(100, 70, 30), cv::Scalar(130, 255, 200));   //BLUE

    bitwise_or(orange,blue,colorSeg);
    showImage("gt mask", colorSeg);


    //morphoogical operations to enhance quality of GT
    Mat morph;
    Mat openKernel(Mat::ones(7,7,CV_32F));
    Mat closeKernel(Mat::ones(7,7,CV_32F));
    morphologyEx(colorSeg,morph, MORPH_CLOSE,closeKernel);
    morphologyEx(morph,morph, MORPH_OPEN,openKernel);


    showImage("Morphological transform", morph);



    //Blob detection
    //Extract connected components
    vector<vector<Point>> contours;
    findContours(morph,contours,CV_RETR_LIST,CV_CHAIN_APPROX_NONE);
    //cout << contours.size() << endl; //

    //Vector<moments> outputMoment;
    int comX = -1;
    int comY = -1;

    Moments circleMoment;
    vector<Vec2i> coordinates;
    vector<vector<Point>> contoursThresh;
    //filter the contours based on compactness:
    double perimiterLowThreshold = 150;
    double perimiterHighThreshold = 400;

    for(int i = 0; i < contours.size(); i++){
        //Define the number of elements in contour as the perimiter
        if(contours[i].size() > perimiterLowThreshold && perimiterHighThreshold > contours[i].size()){
            contoursThresh.push_back(contours[i]);
        }
        //cout << "contour: " << i << "| Pixels in contour: " <<contours[i].size() <<  endl;
    }


    //Calc compactness
    RotatedRect rRect;
    double radius= 0;
    double compactness = 0;
    double area = 0;

    //Find center of the contours
    double averageX = 0;
    double averageY = 0;
    Vec2i centerVec;
    Point2f  center;
    for(int i = 0; i < contoursThresh.size(); i++){
        //calc compactness
        area = contourArea(contoursThresh[i]);
        compactness = ((4*CV_PI)*area)/pow(arcLength(contoursThresh[i],true),2);
        //cout << compactness << endl;
        if(compactness > 0.8){
            circleMoment = moments((contoursThresh[i]));
            comX = circleMoment.m10/circleMoment.m00;
            comY = circleMoment.m01/circleMoment.m00;
            coordinates.push_back(centerVec);

            //draws circles, center of circles
            drawContours(img1, contoursThresh,i, 255,2,8);
            circle(img1, Point(comX,comY), 2, 0, 2);
        }
    }

    showImage("drawing", img1,print, pathToWrite);
}


int main(int argc, char** argv) {
    //From: https://stackoverflow.com/questions/31346132/how-to-get-all-images-in-folder-using-c
    vector<cv::String> fn;
    glob("/home/kasper/qtworkspace/markerImages/sequence_1_h/*.png", fn, false);
    vector<Mat> images;
    size_t count = fn.size(); //number of png files in images folder
    string pathToWrite;
    for (size_t i=0; i<count; i++){
        pathToWrite = "/home/kasper/RWworkspace/markerImages/Results/sequence_1_h/";
        pathToWrite.append(to_string(i));
        //if(i == 23)
            seq1Algo(imread(fn[i]),pathToWrite);
    }

    while (cv::waitKey() != 27); // (do nothing)


    return 0;
}
