#define _CRT_SECURE_NO_WARNINGS
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <iostream>
#include <fstream>
using namespace cv;
using namespace std;

int IOU_max1(char filename[], string trackerTypes[], int type);
int choose1(char filename[], string trackerTypes[]);
int play(string filename, string trackerTypes[], int type);

int IOU_max2(char filename[], string trackerTypes[], int type);
int choose2(char filename[], string trackerTypes[]);
int main()
{
    string trackerTypes[8] = { "BOOSTING", "MIL", "KCF", "TLD","MEDIANFLOW", "GOTURN", "MOSSE", "CSRT" };
    char filename1[] = "1.avi";
    char filename2[] = "2.avi";
    int type;
    type = choose1(filename1, trackerTypes);
    //type = 4;
    IOU_max1(filename1, trackerTypes, type);
    string name = trackerTypes[type] + "_out" + filename1;
    play(name, trackerTypes, type);
    
    type = choose2(filename2, trackerTypes);
    //type = 7;
    IOU_max2(filename2, trackerTypes, type);
    string name = trackerTypes[type] + "_out" + filename2;
    play(name, trackerTypes, type);


    return 1;
}

int choose1(char filename[], string trackerTypes[]) {
    float averTime[8]{};
    float accuracy[8]{};
    for (int type = 0; type < 8; type++) {
        if (type == 5)continue;
        // Create a tracker
        //form 0 to 7
        string trackerType = trackerTypes[type];
        Ptr<Tracker> tracker;

        if (trackerType == "BOOSTING")   tracker = TrackerBoosting::create();
        if (trackerType == "MIL")        tracker = TrackerMIL::create();
        if (trackerType == "KCF")        tracker = TrackerKCF::create();
        if (trackerType == "TLD")        tracker = TrackerTLD::create();
        if (trackerType == "MEDIANFLOW") tracker = TrackerMedianFlow::create();
        if (trackerType == "GOTURN")     tracker = TrackerGOTURN::create();//error
        if (trackerType == "MOSSE")      tracker = TrackerMOSSE::create();//Tracking failure detected
        if (trackerType == "CSRT")       tracker = TrackerCSRT::create();

        string writefile = trackerType + "_out";
        // Read video
        VideoCapture video(filename);

        // Exit if video is not opened
        if (!video.isOpened())
        {
            cout << "Could not read video file" << endl;
            return 1;
        }
        // Read first frame
        Mat frame;
        bool ok = video.read(frame);

        Rect2d track[913];
        int iter = 0;
        // Define bounding box
        Rect2d bbox(422, 131, 60, 64);
        track[iter] = bbox;
        iter++;
        rectangle(frame, bbox, Scalar(255, 0, 0), 2, 1);
        //imshow("Tracking", frame);

        tracker->init(frame, bbox);

        VideoWriter writer;
        writer.open(writefile + filename, VideoWriter::fourcc('M', 'J', 'P', 'G'), 30.0, Size(frame.cols, frame.rows), true);
        writer.write(frame);
        float totalTime = 0;
        while (video.read(frame))
        {
            double timer = (double)getTickCount();
            // Update the tracking result
            bool ok = tracker->update(frame, bbox);
            totalTime += ((double)getTickCount() - timer) / (getTickFrequency());
            track[iter] = bbox;
            iter++;
            if (ok)
                rectangle(frame, bbox, Scalar(255, 0, 0), 2, 1);
            else
                putText(frame, "Tracking failure detected", Point(100, 80), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 255), 2);

            // Display tracker type on frame
            putText(frame, trackerType + " Tracker", Point(100, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50, 170, 50), 2);

            // Display frame.
            //imshow("Tracking", frame);
            writer.write(frame);

            // Exit if ESC pressed.
            int k = waitKey(1);
            if (k == 27)
            {
                break;
            }
        }

        writer.release();
        averTime[type] = totalTime / iter;
        //cout << trackerType << "  Average computational time per frame : " << averTime[type] << "  s" << endl;

        Rect2d ori[913];
        ifstream fin("GroundTruth_1.txt");
        int count = 0;
        string title;
        fin >> title;
        fin >> title;
        fin >> title;
        fin >> title;
        while (!fin.eof()) {
            fin >> ori[count].x;
            fin >> ori[count].y;
            fin >> ori[count].width;
            fin >> ori[count].height;
            count++;
        }
        ofstream fout(trackerTypes[type] + "_GT_1.txt");
        fout << "x        y       w       h" << endl << endl;
        for (int r = 0; r < 913; r++) {
            int zz = 0;
            zz = track[r].x;
            fout << zz;
            fout << "     ";
            zz = track[r].y;
            fout << zz;
            fout << "     ";
            zz = track[r].width;
            fout << zz;
            fout << "      ";
            zz = track[r].height;
            fout << zz;
            fout << endl;
        }
        //ori track
        float IOU[913];
        double temp = 0;
        count = 0;
        for (int i = 0; i < 913; i++) {
            Rect Intersection = ori[i] | track[i];
            Rect Union = ori[i] & track[i];
            temp = Union.area() * 1.0 / Intersection.area();
            //cout << i << "  " << IOU<<endl;
            IOU[i] = temp;
            if (temp > 0.5)
                count++;
        }
        accuracy[type] = count * 1.0 / iter;
    }
    int max;
    max = accuracy[1];
    int num;
    for (int i = 0; i < 8; i++) {
        if (i == 5)continue;
        cout << trackerTypes[i] << "  Average computational time per frame : " << averTime[i] << "  s" << endl;
        cout << trackerTypes[i] << "  Accuracy rate : " << accuracy[i] << endl;
        if (max < accuracy[i]) {
            max = accuracy[i];
            num = i;
        }
    }
    return num;
}
int IOU_max1(char filename[], string trackerTypes[], int type) {
    string trackerType = trackerTypes[type];
    Ptr<Tracker> tracker;

    if (trackerType == "BOOSTING")   tracker = TrackerBoosting::create();
    if (trackerType == "MIL")        tracker = TrackerMIL::create();
    if (trackerType == "KCF")        tracker = TrackerKCF::create();
    if (trackerType == "TLD")        tracker = TrackerTLD::create();
    if (trackerType == "MEDIANFLOW") tracker = TrackerMedianFlow::create();
    if (trackerType == "GOTURN")     tracker = TrackerGOTURN::create();//error
    if (trackerType == "MOSSE")      tracker = TrackerMOSSE::create();//Tracking failure detected
    if (trackerType == "CSRT")       tracker = TrackerCSRT::create();

    string writefile = trackerType + "_out";
    // Read video
    VideoCapture video(filename);

    // Exit if video is not opened
    if (!video.isOpened())
    {
        cout << "Could not read video file" << endl;
    }
    // Read first frame
    Mat frame;
    bool ok = video.read(frame);

    Rect2d track[913];
    int iter = 0;
    // Define bounding box
    Rect2d bbox(422, 131, 60, 64);
    track[iter] = bbox;
    iter++;
    rectangle(frame, bbox, Scalar(255, 0, 0), 2, 1);

    tracker->init(frame, bbox);

    float totalTime = 0;
    while (video.read(frame))
    {
        // Update the tracking result
        bool ok = tracker->update(frame, bbox);
        track[iter] = bbox;
        iter++;
        if (ok)
            rectangle(frame, bbox, Scalar(255, 0, 0), 2, 1);
        else
            putText(frame, "Tracking failure detected", Point(100, 80), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 255), 2);

        // Display tracker type on frame
        putText(frame, trackerType + " Tracker", Point(100, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50, 170, 50), 2);

        // Display frame.
        //imshow("Tracking", frame);
        //writer.write(frame);

        // Exit if ESC pressed.
        int k = waitKey(1);
        if (k == 27)
        {
            break;
        }
    }

    //writer.release();
    ifstream fin("GroundTruth_1.txt");
    Rect2d ori[913];
    int count = 0;
    string title;
    fin >> title;
    fin >> title;
    fin >> title;
    fin >> title;
    while (!fin.eof()) {
        fin >> ori[count].x;
        fin >> ori[count].y;
        fin >> ori[count].width;
        fin >> ori[count].height;
        count++;
    }
    //ori track
    float IOU[913];
    double temp = 0;
    count = 0;
    for (int i = 0; i < 913; i++) {
        Rect Intersection = ori[i] | track[i];
        Rect Union = ori[i] & track[i];
        temp = Union.area() * 1.0 / Intersection.area();
        IOU[i] = temp;
    }
    float stat[10]{};
    int temp1;
    for (int i = 0; i < 913; i++) {
        temp1 = (int)(IOU[i] * 10 + 0.5);
        if (temp1 >= 10)
            stat[9]++;
        else
            stat[temp1]++;
    }
    for (int i = 0; i < 10; i++) {
        stat[i] = stat[i] / 913;
    }
    for (int i = 0; i < 10; i++)
        cout << stat[i] << endl;
    return 0;
}

int choose2(char filename[], string trackerTypes[]) {
    float averTime[8]{};
    float accuracy[8]{};
    for (int type = 0; type < 8; type++) {
        if (type == 5)continue;
        // Create a tracker
        //form 0 to 7
        string trackerType = trackerTypes[type];
        Ptr<Tracker> tracker;

        if (trackerType == "BOOSTING")   tracker = TrackerBoosting::create();
        if (trackerType == "MIL")        tracker = TrackerMIL::create();
        if (trackerType == "KCF")        tracker = TrackerKCF::create();
        if (trackerType == "TLD")        tracker = TrackerTLD::create();
        if (trackerType == "MEDIANFLOW") tracker = TrackerMedianFlow::create();
        if (trackerType == "GOTURN")     tracker = TrackerGOTURN::create();//error
        if (trackerType == "MOSSE")      tracker = TrackerMOSSE::create();//Tracking failure detected
        if (trackerType == "CSRT")       tracker = TrackerCSRT::create();

        string writefile = trackerType + "_out";
        // Read video
        VideoCapture video(filename);

        // Exit if video is not opened
        if (!video.isOpened())
        {
            cout << "Could not read video file" << endl;
            return 1;
        }
        // Read first frame
        Mat frame;
        bool ok = video.read(frame);

        Rect2d track[942];
        int iter = 0;
        // Define bounding box
        Rect2d bbox(512, 120, 80, 102);
        track[iter] = bbox;
        iter++;
        rectangle(frame, bbox, Scalar(255, 0, 0), 2, 1);

        tracker->init(frame, bbox);

        VideoWriter writer;
        writer.open(writefile + filename, VideoWriter::fourcc('M', 'J', 'P', 'G'), 30.0, Size(frame.cols, frame.rows), true);
        writer.write(frame);
        float totalTime = 0;
        int xyz;
        while (video.read(frame))
        {
            double timer = (double)getTickCount();
            // Update the tracking result
            bool ok = tracker->update(frame, bbox);
            totalTime += ((double)getTickCount() - timer) / (getTickFrequency());
            track[iter] = bbox;
            iter++;
            if (ok)
                rectangle(frame, bbox, Scalar(255, 0, 0), 2, 1);
            else
                putText(frame, "Tracking failure detected", Point(100, 80), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 255), 2);

            // Display tracker type on frame
            putText(frame, trackerType + " Tracker", Point(100, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50, 170, 50), 2);

            // Display frame.
            //imshow("Tracking", frame);
            writer.write(frame);

            // Exit if ESC pressed.
            int k = waitKey(1);
            if (k == 27)
            {
                break;
            }
        }
        iter = 942;
        writer.release();
        averTime[type] = totalTime / iter;
        //cout << trackerType << "  Average computational time per frame : " << averTime[type] << "  s" << endl;

        Rect2d ori[942];
        ifstream fin("GroundTruth_2.txt");
        int count = 0;
        string title;
        fin >> title;
        fin >> title;
        fin >> title;
        fin >> title;
        while (!fin.eof()) {
            fin >> ori[count].x;
            fin >> ori[count].y;
            fin >> ori[count].width;
            fin >> ori[count].height;
            count++;
        }
        ofstream fout(trackerTypes[type] + "_GT_2.txt");
        fout << "x        y       w       h" << endl << endl;
        for (int r = 0; r < 942; r++) {
            int zz = 0;
            zz = track[r].x;
            fout << zz;
            fout << "     ";
            zz = track[r].y;
            fout << zz;
            fout << "     ";
            zz = track[r].width;
            fout << zz;
            fout << "      ";
            zz = track[r].height;
            fout << zz;
            fout << endl;
        }
        //ori track
        float IOU[942];
        double temp = 0;
        count = 0;
        for (int i = 0; i < 942; i++) {
            Rect Intersection = ori[i] | track[i];
            Rect Union = ori[i] & track[i];
            temp = Union.area() * 1.0 / Intersection.area();
            //cout << i << "  " << IOU<<endl;
            IOU[i] = temp;
            if (temp > 0.5)
                count++;
        }
        accuracy[type] = count * 1.0 / iter;
    }
    
    int max;
    max = accuracy[1];
    int num;
    for (int i = 0; i < 8; i++) {
        if (i == 5)continue;
        cout << trackerTypes[i] << "  Average computational time per frame : " << averTime[i] << "  s" << endl;
        cout << trackerTypes[i] << "  Accuracy rate : " << accuracy[i] << endl;
        if (max < accuracy[i]) {
            max = accuracy[i];
            num = i;
        }
    }
    return num;
}
int IOU_max2(char filename[], string trackerTypes[], int type) {
    string trackerType = trackerTypes[type];
    Ptr<Tracker> tracker;

    if (trackerType == "BOOSTING")   tracker = TrackerBoosting::create();
    if (trackerType == "MIL")        tracker = TrackerMIL::create();
    if (trackerType == "KCF")        tracker = TrackerKCF::create();
    if (trackerType == "TLD")        tracker = TrackerTLD::create();
    if (trackerType == "MEDIANFLOW") tracker = TrackerMedianFlow::create();
    if (trackerType == "GOTURN")     tracker = TrackerGOTURN::create();//error
    if (trackerType == "MOSSE")      tracker = TrackerMOSSE::create();//Tracking failure detected
    if (trackerType == "CSRT")       tracker = TrackerCSRT::create();

    string writefile = trackerType + "_out";
    // Read video
    VideoCapture video(filename);

    // Exit if video is not opened
    if (!video.isOpened())
    {
        cout << "Could not read video file" << endl;
    }
    // Read first frame
    Mat frame;
    bool ok = video.read(frame);

    Rect2d track[942];
    int iter = 0;
    // Define bounding box
    Rect2d bbox(512, 120, 80, 102);
    track[iter] = bbox;
    iter++;
    rectangle(frame, bbox, Scalar(255, 0, 0), 2, 1);

    tracker->init(frame, bbox);
    int xyz;
    while (video.read(frame))
    {
        // Update the tracking result
        bool ok = tracker->update(frame, bbox);
        track[iter] = bbox;
        iter++;
        if (ok)
            rectangle(frame, bbox, Scalar(255, 0, 0), 2, 1);
        else
            putText(frame, "Tracking failure detected", Point(100, 80), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 255), 2);

        // Display tracker type on frame
        putText(frame, trackerType + " Tracker", Point(100, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50, 170, 50), 2);

        // Display frame.
        //imshow("Tracking", frame);
        //writer.write(frame);

        // Exit if ESC pressed.
        int k = waitKey(1);
        if (k == 27)
        {
            break;
        }
        if (iter == 941)
            xyz = 0;
    }

    //writer.release();
    ifstream fin("GroundTruth_2.txt");
    Rect2d ori[942];
    int count = 0;
    string title;
    fin >> title;
    fin >> title;
    fin >> title;
    fin >> title;
    while (!fin.eof()) {
        fin >> ori[count].x;
        fin >> ori[count].y;
        fin >> ori[count].width;
        fin >> ori[count].height;
        count++;
    }
    //ori track
    float IOU[942];
    double temp = 0;
    count = 0;
    for (int i = 0; i < 942; i++) {
        Rect Intersection = ori[i] | track[i];
        Rect Union = ori[i] & track[i];
        temp = Union.area() * 1.0 / Intersection.area();
        IOU[i] = temp;
    }
    float stat[10]{};
    int temp1;
    for (int i = 0; i < 942; i++) {
        temp1 = (int)(IOU[i] * 10 + 0.5);
        if (temp1 >= 10)
            stat[9]++;
        else
            stat[temp1]++;
    }
    for (int i = 0; i < 10; i++) {
        stat[i] = stat[i] / 942;
    }
    for (int i = 1; i < 10; i++) {
        stat[i] = stat[i] + stat[i - 1];
    }
    for (int i = 0; i < 10; i++)
        cout << stat[i] << endl;
    return 0;
}
int play(string filename, string trackerTypes[], int type) {

    VideoCapture video(filename);
    if (!video.isOpened())
    {
        cout << "Could not read video file" << endl;
        return -1;
    }

    Mat frame;
    bool ok = video.read(frame);

    while (video.read(frame))
    {
        // Display frame.
        imshow(trackerTypes[type], frame);

        // Exit if ESC pressed.
        int k = waitKey(1);
        if (k == 27)
        {
            break;
        }
    }
    destroyAllWindows();
    return 0;
}