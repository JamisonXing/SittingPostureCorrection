//#include "PoseDetector6.cpp"-----
#include <openpose/headers.hpp>
#include<windows.h>
#include<cstdio>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <iostream>
#include <openpose/flags.hpp>
#include <iostream>
#include <fstream>
using namespace std;

using namespace std;
using namespace cv;

class PoseDetector {
public:
	PoseDetector();
	~PoseDetector() {
		delete opWrapper;
	}

	op::Array<float> detectPose();

private:
	op::Wrapper* opWrapper;
	void configureWrapper(op::Wrapper* opWrapper);
	void printKeypoints(const std::shared_ptr<std::vector<std::shared_ptr<op::Datum>>>& datumsPtr);
	bool display(const std::shared_ptr<std::vector<std::shared_ptr<op::Datum>>>& datumsPtr);
};
////5
// #include <opencv2/opencv.hpp> #include <openpose/flags.hpp>

// Custom OpenPose flags
// Producer
//DEFINE_string(image_dir, "D:/program/deeplearning/deeplearning/openpose_source/openpose/build/examples/SitePoseMonitor/testsimgs",
//    "Process a directory of images. Read all standard formats (jpg, png, bmp, etc.).");
// Display
DEFINE_bool(no_display, false,
	"Enable to disable the visual display.");
PoseDetector::PoseDetector() {
	op::opLog("Starting PoseDetector...", op::Priority::High);
	const auto opTimer = op::getTimerInit();

	// Configuring OpenPose
	op::opLog("Configuring OpenPose...", op::Priority::High);
	this->opWrapper = new op::Wrapper(op::ThreadManagerMode::Asynchronous);
	this->configureWrapper(opWrapper);
	// Starting OpenPose
	op::opLog("Starting thread(s)...", op::Priority::High);
	opWrapper->start();
}
op::Array<float> PoseDetector::detectPose() {
	const cv::Mat cvImageToProcess = cv::imread("D:/ProgramFile/openpose-1.7.0/test.jpg");//xxj
	//const cv::Mat cvImageToProcess = cv::imread("D:/program/deeplearning/deeplearning/openpose_source/openpose/test.jpg");
	const op::Matrix imageToProcess = OP_CV2OPCONSTMAT(cvImageToProcess);
	auto datumProcessed = opWrapper->emplaceAndPop(imageToProcess);
	if (datumProcessed != nullptr)
	{
		printKeypoints(datumProcessed);
		if (!FLAGS_no_display)
		{
			const auto userwantstoexit = display(datumProcessed);
			if (userwantstoexit)
			{
				op::opLog("user pressed esc to exit demo.", op::Priority::High);
			}
		}
	}
	else
		op::opLog("image could not be processed.", op::Priority::High);
	if (datumProcessed != nullptr && !datumProcessed->empty()) {
		return datumProcessed->at(0)->poseKeypoints;
	}
	else {
		op::Array<float> array;
		return array;
	}
}

void PoseDetector::configureWrapper(op::Wrapper* opWrapper)
{
	try
	{
		// Configuring OpenPose

		// logging_level
		op::checkBool(
			0 <= FLAGS_logging_level && FLAGS_logging_level <= 255, "Wrong logging_level value.",
			__LINE__, __FUNCTION__, __FILE__);
		op::ConfigureLog::setPriorityThreshold((op::Priority)FLAGS_logging_level);
		op::Profiler::setDefaultX(FLAGS_profile_speed);

		// Applying user defined configuration - GFlags to program variables outputSize
		const auto outputSize = op::flagsToPoint(op::String(FLAGS_output_resolution), "-1x-1");
		// netInputSize
		const auto netInputSize = op::flagsToPoint(op::String(FLAGS_net_resolution), "-1x368");
		// faceNetInputSize
		const auto faceNetInputSize = op::flagsToPoint(op::String(FLAGS_face_net_resolution), "368x368 (multiples of 16)");
		// handNetInputSize
		const auto handNetInputSize = op::flagsToPoint(op::String(FLAGS_hand_net_resolution), "368x368 (multiples of 16)");
		// poseMode
		const auto poseMode = op::flagsToPoseMode(FLAGS_body);
		// poseModel
		const auto poseModel = op::flagsToPoseModel(op::String(FLAGS_model_pose));
		// JSON saving
		if (!FLAGS_write_keypoint.empty())
			op::opLog(
				"Flag `write_keypoint` is deprecated and will eventually be removed. Please, use `write_json`"
				" instead.", op::Priority::Max);
		// keypointScaleMode
		const auto keypointScaleMode = op::flagsToScaleMode(FLAGS_keypoint_scale);
		// heatmaps to add
		const auto heatMapTypes = op::flagsToHeatMaps(FLAGS_heatmaps_add_parts, FLAGS_heatmaps_add_bkg,
			FLAGS_heatmaps_add_PAFs);
		const auto heatMapScaleMode = op::flagsToHeatMapScaleMode(FLAGS_heatmaps_scale);
		// >1 camera view?
		const auto multipleView = (FLAGS_3d || FLAGS_3d_views > 1);
		// Face and hand detectors
		const auto faceDetector = op::flagsToDetector(FLAGS_face_detector);
		const auto handDetector = op::flagsToDetector(FLAGS_hand_detector);
		// Enabling Google Logging
		const bool enableGoogleLogging = true;

		// Pose configuration (use WrapperStructPose{} for default and recommended configuration)---FLAGS_net_resolution_dynamic,
		const op::WrapperStructPose wrapperStructPose{
			poseMode, netInputSize,  outputSize, keypointScaleMode, FLAGS_num_gpu,
			FLAGS_num_gpu_start, FLAGS_scale_number, (float)FLAGS_scale_gap,
			op::flagsToRenderMode(FLAGS_render_pose, multipleView), poseModel, !FLAGS_disable_blending,
			(float)FLAGS_alpha_pose, (float)FLAGS_alpha_heatmap, FLAGS_part_to_show, op::String(FLAGS_model_folder),
			heatMapTypes, heatMapScaleMode, FLAGS_part_candidates, (float)FLAGS_render_threshold,
			FLAGS_number_people_max, FLAGS_maximize_positives, FLAGS_fps_max, op::String(FLAGS_prototxt_path),
			op::String(FLAGS_caffemodel_path), (float)FLAGS_upsampling_ratio, enableGoogleLogging };
		opWrapper->configure(wrapperStructPose);
		// Face configuration (use op::WrapperStructFace{} to disable it)
		const op::WrapperStructFace wrapperStructFace{
			FLAGS_face, faceDetector, faceNetInputSize,
			op::flagsToRenderMode(FLAGS_face_render, multipleView, FLAGS_render_pose),
			(float)FLAGS_face_alpha_pose, (float)FLAGS_face_alpha_heatmap, (float)FLAGS_face_render_threshold };
		opWrapper->configure(wrapperStructFace);
		// Hand configuration (use op::WrapperStructHand{} to disable it)
		const op::WrapperStructHand wrapperStructHand{
			FLAGS_hand, handDetector, handNetInputSize, FLAGS_hand_scale_number, (float)FLAGS_hand_scale_range,
			op::flagsToRenderMode(FLAGS_hand_render, multipleView, FLAGS_render_pose), (float)FLAGS_hand_alpha_pose,
			(float)FLAGS_hand_alpha_heatmap, (float)FLAGS_hand_render_threshold };
		opWrapper->configure(wrapperStructHand);
		// Extra functionality configuration (use op::WrapperStructExtra{} to disable it)
		const op::WrapperStructExtra wrapperStructExtra{
			FLAGS_3d, FLAGS_3d_min_views, FLAGS_identification, FLAGS_tracking, FLAGS_ik_threads };
		opWrapper->configure(wrapperStructExtra);
		// Output (comment or use default argument to disable any output)
		const op::WrapperStructOutput wrapperStructOutput{
			FLAGS_cli_verbose, op::String(FLAGS_write_keypoint), op::stringToDataFormat(FLAGS_write_keypoint_format),
			op::String(FLAGS_write_json), op::String(FLAGS_write_coco_json), FLAGS_write_coco_json_variants,
			FLAGS_write_coco_json_variant, op::String(FLAGS_write_images), op::String(FLAGS_write_images_format),
			op::String(FLAGS_write_video), FLAGS_write_video_fps, FLAGS_write_video_with_audio,
			op::String(FLAGS_write_heatmaps), op::String(FLAGS_write_heatmaps_format), op::String(FLAGS_write_video_3d),
			op::String(FLAGS_write_video_adam), op::String(FLAGS_write_bvh), op::String(FLAGS_udp_host),
			op::String(FLAGS_udp_port) };
		opWrapper->configure(wrapperStructOutput);
		// No GUI. Equivalent to: opWrapper.configure(op::WrapperStructGui{}); Set to single-thread
		// (for sequential processing and/or debugging and/or reducing latency)
		if (FLAGS_disable_multi_thread)
			opWrapper->disableMultiThreading();
	}
	catch (const std::exception& e)
	{
		op::error(e.what(), __LINE__, __FUNCTION__, __FILE__);
	}
}

void PoseDetector::printKeypoints(const std::shared_ptr<std::vector<std::shared_ptr<op::Datum>>>& datumsPtr)
{
	try
	{
		// Example: How to use the pose keypoints
		if (datumsPtr != nullptr && !datumsPtr->empty())
		{
			op::opLog("Body keypoints: " + datumsPtr->at(0)->poseKeypoints.toString(), op::Priority::High);
			op::opLog("Face keypoints: " + datumsPtr->at(0)->faceKeypoints.toString(), op::Priority::High);
			op::opLog("Left hand keypoints: " + datumsPtr->at(0)->handKeypoints[0].toString(), op::Priority::High);
			op::opLog("Right hand keypoints: " + datumsPtr->at(0)->handKeypoints[1].toString(), op::Priority::High);
		}
		else
			op::opLog("Nullptr or empty datumsPtr found.", op::Priority::High);
	}
	catch (const std::exception& e)
	{
		op::error(e.what(), __LINE__, __FUNCTION__, __FILE__);
	}
}

bool PoseDetector::display(const std::shared_ptr<std::vector<std::shared_ptr<op::Datum>>>& datumsPtr)
{
	try
	{
		// User's displaying/saving/other processing here datum.cvOutputData: rendered frame with
		// pose or heatmaps datum.poseKeypoints: Array<float> with the estimated pose
		if (datumsPtr != nullptr && !datumsPtr->empty())
		{
			// Display image and sleeps at least 1 ms (it usually sleeps ~5-10 msec to display the image)
			const cv::Mat cvMat = OP_OP2CVCONSTMAT(datumsPtr->at(0)->cvOutputData);
			if (!cvMat.empty()) {
				cv::imshow(OPEN_POSE_NAME_AND_VERSION + " - Tutorial C++ API", cvMat);
				cv::imwrite("pose.jpg", cvMat);
			}
			else
				op::opLog("Empty cv::Mat as output.", op::Priority::High, __LINE__, __FUNCTION__, __FILE__);
		}
		else
			op::opLog("Nullptr or empty datumsPtr found.", op::Priority::High);
		const auto key = (char)cv::waitKey(1);
		return (key == 27);
	}
	catch (const std::exception& e)
	{
		op::error(e.what(), __LINE__, __FUNCTION__, __FILE__);
		return true;
	}
}
//------------------------------------------------------------------------------策略
//#include "PoseAnalyzer4.cpp"-----
enum Result
{
	NO_PERSON = -1,
	OKAY = 0,
	NECK_WRONG = 1,
	BACK_WRONG = 2,
	LEG_WRONG = 3,
	SAMPLEING = 4,
	SHOULDER_WRONG = 5
};
class PoseAnalyzer
{
public:
	int analyse(op::Array<float> poseKeypoints);

private:
	double getAngle(double x1, double y1, double x2, double y2, double x3, double y3);
	static int wrongCounter;
	static int wrongCounter1;//用来计数
	static int sampleCounter;
	static int goodCounter;
	static int badNeck;
	static int badShoulder;
	static int goodNeck;
	static int goodShoulder;
};
// //4 #include <iostream>

int PoseAnalyzer::analyse(op::Array<float> poseKeypoints) {
	Result result = Result::OKAY;//0

	if (poseKeypoints.empty()) {
		std::cout << "no person found" << std::endl;
		return Result::NO_PERSON;//-1
	}

	//----------Angle program is abandoned（角度方案被舍弃 2022.2.12）--------
	//0,1,2,5,15,16,17,18
	//float pt0_x = poseKeypoints[3 * 0];
	//float pt0_y = poseKeypoints[3 * 0 + 1];


	//float pt1_x = poseKeypoints[3 * 1];
	//float pt1_y = poseKeypoints[3 * 1 + 1];

	//float pt8_x = poseKeypoints[3 * 8];
	//float pt8_y = poseKeypoints[3 * 8 + 1];

	//float pt0_x = poseKeypoints[3 * 0];
	//float pt0_y = poseKeypoints[3 * 0 + 1];

	//float pt1_x = poseKeypoints[3 * 1];
	//float pt1_y = poseKeypoints[3 * 1 + 1];

	//float pt2_x = poseKeypoints[3 * 2];
	//float pt2_y = poseKeypoints[3 * 2 + 1];

	//float pt5_x = poseKeypoints[3 * 5];					//					15      16
	//float pt5_y = poseKeypoints[3 * 5 + 1];				//               17     0      18
															//	                    |
//	float pt15_x = poseKeypoints[3 * 15];					//             2--------1--------5
//	float pt15_y = poseKeypoints[3 * 15 + 1];				//            /			|		  \				
															//			 /          |          \
//	float pt16_x = poseKeypoints[3 * 16];					//          3			|	      	6
//	float pt16_y = poseKeypoints[3 * 16 + 1];				//		   /            |            \
															//		  4				|			  7
//	float pt17_x = poseKeypoints[3 * 17];					//						8
//	float pt17_y = poseKeypoints[3 * 17 + 1];				//
															//
//	float pt18_x = poseKeypoints[3 * 18];					//
//	float pt18_y = poseKeypoints[3 * 18 + 1];				//


	//double neckAngle1 = getAngle(pt0_x, pt0_y, pt1_x, pt1_y, pt5_x, pt5_y);
	//double neckAngle2 = getAngle(pt0_x, pt0_y, pt1_x, pt1_y, pt2_x, pt2_y);
	//double backAngle1 = getAngle(pt0_x, pt0_y, pt1_x, pt1_y, pt8_x, pt8_y);
	////double angle12 = getAngle(pt1_x, pt1_y, pt8_x + 1, pt8_y, pt8_x, pt8_y);
	////double angle12 = getAngle(pt1_x, pt1_y, pt8_x, pt8_y, pt8_x, pt8_y);


	//op::opLog("neckAngle1: " + std::to_string(neckAngle1), op::Priority::High);
	//op::opLog("neckAngle2: " + std::to_string(neckAngle2), op::Priority::High);
	//op::opLog("subtraction: " + std::to_string(abs(neckAngle1 - neckAngle2)), op::Priority::High);
	//op::opLog("backAngle1: " + std::to_string(backAngle1), op::Priority::High);

	//if(abs(neckAngle1 - neckAngle2) > 20){
	////if (neckAngle1  > 70 ) {
	//	op::opLog("Incorrect neck posture", op::Priority::High);
	//	result = Result::NECK_WRONG;
	//	wrongCounter++;
	//}
	//if (backAngle1 > 60) {
	//	op::opLog("Incorrect back posture ", op::Priority::High);
	//	result = Result::BACK_WRONG;
	//	wrongCounter++;
	//}

	//Proportional plan（采用比例方案 2022.2.12）
	float noseX = poseKeypoints[3 * 0];
	float noseY = poseKeypoints[3 * 0 + 1];

	float midX = poseKeypoints[3 * 1];
	float midY = poseKeypoints[3 * 1 + 1];

	float leftX = poseKeypoints[3 * 2];
	float leftY = poseKeypoints[3 * 2 + 1];

	float rightX = poseKeypoints[3 * 5];					
	float rightY = poseKeypoints[3 * 5 + 1];

	//NECK
	float headX = noseX - midX;
	float headY = noseY - midY;
	float rate1 = abs(headX / headY);
	if (headX != 0) {
		if (rate1 > 0.13) {
			op::opLog("BAD NECK", op::Priority::High);
			result = Result::NECK_WRONG;
			badNeck++;
			wrongCounter++;
			wrongCounter1++;

		}
		else {
			op::opLog("GOOD NECK ", op::Priority::High);
			goodCounter++;
			goodNeck++;
		}
	}
	//当0 和 1重合时
	else {
		op::opLog("BAD NECK", op::Priority::High);
		result = Result::NECK_WRONG;
		wrongCounter++;
		badNeck++;
	}
	//SHOULDER
	float leftSX = midX - leftX ;
	float leftSY = midY - leftY ;
	float rate2 = leftSY / leftSX;
	float rightSX = rightX - midX;
	float rightSY = midY - rightY;
	float rate3 = rightSY / rightSX;
	if (!((abs(rate2) < 0.25 && abs(rate3) < 0.25 && rate2 * rate3 > 0)||
		(abs(rate2) < 0.1 && abs(rate3) < 0.1))){
		op::opLog("BAD SHOULDER", op::Priority::High);
		result = Result::SHOULDER_WRONG;
		badShoulder++;
		wrongCounter++;
		wrongCounter1++;

	}else {
		op::opLog("GOOD SHOULDER ", op::Priority::High);
		goodCounter++;
		goodShoulder++;
	}




	//Sampling three times in a row
	sampleCounter++;
	if (sampleCounter >= 3) {
		op::opLog("end of sampling", op::Priority::High);
		if (wrongCounter < 2) {//If the wrong poses are not accumulated to 2 in 3 samplings
			result = Result::OKAY;
		}
		wrongCounter = 0;
		sampleCounter = 0;
	}
	else {
		op::opLog("Number of samples" + std::to_string(sampleCounter), op::Priority::High);
		result = Result::SAMPLEING;
	}


	//统计数据,txt版本
	//ofstream outfile;
	//outfile.open("data.txt",ios::out | ios::app);
	//cout << "Writing to the file" << endl;
	//outfile << "wrongCount:" << wrongCounter1 << endl;
	//outfile << "goodCount:" << goodCounter << endl;
	//outfile << "total:" << goodCounter + wrongCounter1 << endl;
	//outfile.close();
	
	//统计数据，csv版本
	ofstream outfile;
	outfile.open("data.csv", ios::out | ios::app);
	cout << "Writing to the data" << endl;
	//outfile << "wrongcount" << ',' << "goodcount" << ',' << "total" << endl;
	outfile << wrongCounter1 << ',' << goodCounter << ',' << wrongCounter1 + goodCounter << endl;
	outfile.close();

	ofstream outfile1;
	outfile1.open("neck.csv", ios::out | ios::app);
	cout << "Writing to the neck" << endl;
	//outfile << "wrongcount" << ',' << "goodcount" << ',' << "total" << endl;
	outfile1 << goodNeck << ',' << badNeck << ',' << goodNeck + badNeck << endl;
	outfile1.close();




	return result;
}

// //3 #include <cmath>
//#define M_PI       3.14159265358979323846   // pi
//double PoseAnalyzer::getAngle(double x1, double y1, double x2, double y2, double x3, double y3)
//{
//	double theta = atan2(x1 - x3, y1 - y3) - atan2(x2 - x3, y2 - y3);
//	if (theta > M_PI)
//		theta -= 2 * M_PI;
//	if (theta < -M_PI)
//		theta += 2 * M_PI;
//
//	theta = abs(theta * 180.0 / M_PI);
//	return theta;
//}

int PoseAnalyzer::wrongCounter = 0;
int PoseAnalyzer::wrongCounter1 = 0;
int PoseAnalyzer::sampleCounter = 0;
int PoseAnalyzer::goodCounter = 0;
int PoseAnalyzer::goodNeck = 0;
int PoseAnalyzer::goodShoulder = 0;
int PoseAnalyzer::badNeck = 0;
int PoseAnalyzer::badShoulder = 0;


//----------------------------------------------------------------------
//#include "Camera2.cpp"----
class Camera
{
public:
	void takePicture();
};
// //2 #include <opencv2/opencv.hpp> using namespace std; using namespace cv;
void Camera::takePicture()
{
	VideoCapture cap(0);			 //���ڴ�����ͷ
	char pic_Name[128] = {};
	//��Ƭ����

	if (!cap.isOpened())
	{
		cout << "The camera open failed!" << endl;
	}
	//源代码
	Mat frame;
	cap >> frame;
	imshow("Camera", frame);		//չʾ��ǰ����
	waitKey(10);
	imwrite("test.jpg", frame);	//��Mat����д���ļ�
   //
	//
	// 
	//xxj--cuo
	//while (true) {
	//    Mat frame;
	//    cap >> frame;
	//    //imshow("Camera", frame);		
	//   // waitKey(30);
	//    imwrite("test.jpg", frame);	
}

// //1 #include<windows.h> #include<cstdio>

long lastRestTime = 0;
long getTimestamp() {
	time_t t;
	time(&t);
	return t;
}
int main(int argc, char* argv[])
{
	PoseDetector poseDetector;
	PoseAnalyzer poseAnalyzer;
	Camera camera;
	lastRestTime = getTimestamp();

	while (true) {
		camera.takePicture();
		op::Array<float> poseKeypoints = poseDetector.detectPose();
		int result = poseAnalyzer.analyse(poseKeypoints);
		if (result == Result::NECK_WRONG) {
			MessageBox(GetForegroundWindow(), TEXT("Pay attention to neck posture"), TEXT("SitePoseMonitor"), 1);
		}
		if (result == Result::SHOULDER_WRONG) {
			MessageBox(GetForegroundWindow(), TEXT("Pay attention to your shoulder posture"), TEXT("SitePoseMonitor"), 1);
		}
		if (result == Result::NO_PERSON) {
			lastRestTime = getTimestamp();
		}
		op::opLog("have been worked:" + std::to_string(getTimestamp() - lastRestTime), op::Priority::High);

		if (getTimestamp() - lastRestTime >= 60 * 60) {//1小时休息一次
			MessageBox(GetForegroundWindow(), TEXT("Take a break, let's do some activity~"), TEXT("SitePoseMonitor"), 1);
		}
		//Sleep(1000*10); //改变速度
		Sleep(1000);
	}

	return 1;
}
