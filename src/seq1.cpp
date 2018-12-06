#include <iostream>
#include "opencv2/core/core.hpp"
#include <opencv2/highgui.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/features2d.hpp"
#include <cv.h>
#include <math.h>
#include <cmath>


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
    }

}

Mat getMask(Mat img, Vec3b hsvPixel, int hThreshold, int sThreshold, int vThreshold){
    Mat hsvImg, mask;
    cvtColor(img,hsvImg, COLOR_BGR2HSV);
    //Vec3b hsvPixel(hsvImg.at<Vec3b>(x,y));

    Scalar minimumHSV = Scalar(hsvPixel.val[0] - hThreshold, hsvPixel.val[1] - sThreshold, hsvPixel.val[2] - vThreshold);
    Scalar maxHSV = Scalar(hsvPixel.val[0] + hThreshold, hsvPixel.val[1] + sThreshold, hsvPixel.val[2] + vThreshold);

    //create a mask using the thresholded range
    inRange(hsvImg, minimumHSV, maxHSV,mask);
    return mask;
}


int main(int argc, char** argv) {
    Mat img1 = cv::imread("/home/kasper/qtworkspace/markerImages/sequence_1/marker_color_01.png", CV_LOAD_IMAGE_COLOR);
    showImage("imageTest", img1);


    Mat hsvImg;
    Mat colorSeg;

    //convert to HSV
    cvtColor(img1,hsvImg, COLOR_BGR2HSV);

    Vec3b orangePixel(39,250,192); //HSV pixel values, from samples in the image
    Vec3b bluePixel(77,162,157);

    Mat orange = getMask(hsvImg, orangePixel, 10,10,10); //ORANGE
    Mat blue = getMask(hsvImg,bluePixel,25,40,20);   //BLUE

    bitwise_or(orange,blue,colorSeg);
    showImage("gt mask", blue);


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
    double perimiterLowThreshold = 200;
    double perimiterHighThreshold = 400;

    for(int i = 0; i < contours.size(); i++){
        //Define the number of elements in contour as the perimiter
        if(contours[i].size() > perimiterLowThreshold && perimiterHighThreshold > contours[i].size()){
            contoursThresh.push_back(contours[i]);
        }
        cout << "contour: " << i << "| Pixels in contour: " <<contours[i].size() <<  endl;
    }


    //Find center of the contours

    double averageX = 0;
    double averageY = 0;
    for(int i = 0; i < contoursThresh.size(); i++){
        circleMoment = moments((contoursThresh[i]));
        comX = circleMoment.m10/circleMoment.m00;
        comY = circleMoment.m01/circleMoment.m00;

        coordinates.push_back(Vec2i(comX,comY));

        //calculates the center of the marker
        averageX += comX/contoursThresh.size();
        averageY += comY/contoursThresh.size();

        //draws circles, center of circles
        drawContours(img1, contoursThresh,i, 255,2,8);
        circle(img1, Point(comX,comY), 2, 0, 2);
    }

    //Draws point in the middle of the marker
    circle(img1, Point(cvRound(averageX),cvRound(averageY)), 2, Scalar(0,0,255), 2);

    showImage("drawing", img1);


    while (cv::waitKey() != 27); // (d½o nothing)

    return 0;
}
