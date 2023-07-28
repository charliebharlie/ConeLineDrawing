#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/videoio.hpp"
#include "opencv2/imgproc.hpp"

using namespace std;
using namespace cv;

static int MIN_CONTOUR_AREA = 55;
static int CIRCLE_SIZE = 20;


Mat applyColorFilter(Mat img) {
    Mat hsv;
    // saturate colors to make the brighter colors pop out, then save it into HSV
    cvtColor(img, hsv, COLOR_BGR2HSV);
    Mat thresh;

    // finds orange colored objects in the picture (HSV parameters), then save it into t
    inRange(hsv, Scalar(0, 165, 165), Scalar(255, 255, 255), thresh);
    return thresh;
}

bool hasBeenTested(int i, vector<int> passThrough) {
    for (int j = 0; j < passThrough.size(); j++) {
        if (i == passThrough[j]) {
            return true;
        }
    }
    return false;
}

void extendLine(Mat* img, Point p1, Point p2) {
    int height = (*img).rows; // nunmber of rows
    int width = (*img).cols; // number of columns
    double slope = (double)(p1.y - p2.y) / (double)(p1.x - p2.x); 
    double intercept = -p1.x * slope + p1.y;
    Point np1 = Point(0, intercept);
    Point np2 = Point(width, slope * width + intercept);
    line((*img), np1, np2, Scalar(0, 255, 0), 4);
}

int main()
{
    // image path
    string image_path = "C:/Users/Charl/OneDrive/Documents/dev/OpenCV/Photos/red.png";
    Mat img = imread(image_path, IMREAD_COLOR);

    // blur image
    Mat blur;
    medianBlur(img, blur, 5);

    // apply color threshold to get the cones
    Mat thresh = applyColorFilter(blur);
    // imshow("Image", thresh);

    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    findContours(thresh, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

    // contours that are greater or equal than the minimum contour area
    vector<vector<Point>> filtered_contours;
    for (int i = 0; i < contours.size(); i++) {
        // append the contours that are greater or equal than the minimum contour area and filter out the smaller contours
        if (contourArea(contours[i]) >= MIN_CONTOUR_AREA) {
            filtered_contours.push_back(contours[i]);
            // cout << contours[i];
        }
    }

    // print out the number of contours
    cout << "contour size: " << filtered_contours.size() << endl;

    // array that holds the moments of the contours (map out a grid and find the number of pixels in each x and y column) [TODO: Name of the array variable is mu]
    vector<Moments> mu(filtered_contours.size());

    // array that holds the centers through using the moments to find the center of the contours [TODO: Name of the array variable is centers]
    vector<Point> centers(filtered_contours.size());

    //generate centers of all of the contours
    for (int i = 0; i < filtered_contours.size(); i++) {
        mu[i] = moments(filtered_contours[i]);

        //cout << "m00: " << mu[i].m00 << endl;
        //cout << "m10: " << mu[i].m10 << endl;
        //cout << "m01: " << mu[i].m01 << endl;

        // found online
        centers[i] = Point(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
        // circle(thresh, centers[i], CIRCLE_SIZE, Scalar(255, 255, 255));

        putText(img, to_string(i), centers[i], FONT_ITALIC, 1.0, (255, 255, 255), 3);
    }

    pair<int, int> line1;
    int maxPassThrough;
    // used to find the best line of best fit
    vector<int> passThrough;
    // test every line of best fit
    for (int i = 0; i < filtered_contours.size(); i++) {
        Point c1 = centers[i];
        //cout << "Count is: " << c1 << " at index: " << i << endl;
        circle(img, c1, 1, (0, 255, 255), 2);
        for (int j = i + 1; j < filtered_contours.size(); j++) {
            Point c2 = centers[j];
            int y = c1.y - c2.y;
            int x = c2.x - c1.x;
            //cout << "Count is: " << y << " at y-coordinate " << endl;
            //cout << "Count is: " << x << " at x-coordinate " << endl;
            // this calculation is used for the y-intercept of the line of best fit
            int b = c1.x * c2.y - c2.x * c1.y;
            vector<int> passesThrough;

            // if the line intersects the cone, add that line
            for (int k = 0; k < filtered_contours.size(); k++) {
                // formula found online
                int dist = abs((x * centers[k].y) + (y * centers[k].x) + b) / sqrt((x * x) + (y * y));
                if (CIRCLE_SIZE >= dist) {
                    passesThrough.push_back(k);
                }
            }

            // test and filter for the best line of best fit
            if (passesThrough.size() > passThrough.size()) {
                line1.first = i;
                line1.second = j;
                passThrough = passesThrough;
                cout << "I:" << i << " " << "J:" << j << endl;
                cout << "size: " << passesThrough.size() << endl;
            }
        }
    }

    pair<int, int> line2;
    vector<int> passThrough2;
    // test every line of best fit that hasn't been tested
    for (int i = 0; i < filtered_contours.size(); i++) {
        bool skip = false;
        // cout << hasBeenTested(i, passThrough) << endl;
        if (hasBeenTested(i, passThrough2)) {
            skip = true;
        }
        if (skip) continue;

        Point c1 = centers[i];
        for (int j = i + 1; j < filtered_contours.size(); j++) {

            Point c2 = centers[j];
            int y = c1.y - c2.y;
            int x = c2.x - c1.x;
            int b = c1.x * c2.y - c2.x * c1.y;
            vector<int> passesThrough;
            for (int k = 0; k < filtered_contours.size(); k++) {
                if (hasBeenTested(i, passThrough)) {
                    skip = true;
                }
                if (skip) {
                    skip = false;
                    continue;
                }
                int dist = abs((x * centers[k].y) + (y * centers[k].x) + b) / sqrt((x * x) + (y * y));
                if (CIRCLE_SIZE >= dist) {
                    passesThrough.push_back(k);
                }
            }
            if (passesThrough.size() > passThrough2.size()) {
                line2.first = i;
                line2.second = j;
                passThrough2 = passesThrough;
                cout << "I:" << i << " " << "J:" << j << endl;
                cout << "size: " << passesThrough.size() << endl;
            }
        }
    }

    //drawing lines
    extendLine(&img, centers[line1.first], centers[line1.second]);

    extendLine(&img, centers[line2.first], centers[line2.second]);

    namedWindow("Display frame", WINDOW_NORMAL);
    resizeWindow("Display frame", 1920, 1080);
    imshow("Display frame", img);

    //imshow("thresh", thresh);

    //saving file as answer.png
    imwrite("answer.png", img);
    //imwrite("thresh.png", thresh);
    int k = waitKey(0); // Wait for a keystroke in the window
    return 0;
}
