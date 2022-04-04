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
//语音功能
#include<string>
#include<sapi.h>
#pragma comment(lib,"sapi.lib")

//图形界面
#include <easyx.h>
#include <graphics.h>
#include<conio.h>
#include<tchar.h>

using namespace std;
using namespace cv;

//开关量来画散点图
static int b = 0;
//控制速度
static int TIME = 100;

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
				//cv::imshow(OPEN_POSE_NAME_AND_VERSION + " - Tutorial C++ API", cvMat);
				cv::imwrite("pose.jpg", cvMat);
				////更改图片大小，并保存
				//Size srcSize = Size(640,356);  //填入任意指定尺寸		
				//Mat cvMat1;
				//cv::resize(cvMat, cvMat1, srcSize, INTER_LINEAR);
				//cv::imwrite("pose.jpg", cvMat1);

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
	SHOULDER_WRONG = 5,
};
class PoseAnalyzer
{
public:
	int analyse(op::Array<float> poseKeypoints);

private:
	//double getAngle(double x1, double y1, double x2, double y2, double x3, double y3);
	static int wrongCounter;
	static int wrongCounter1;//用来计数
	static int sampleCounter; 
	static int goodCounter;
	static int badNeck;
	static int badShoulder;
	static int goodNeck;
	static int goodShoulder;
	//interval间隔
	static int interval;
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
			//显示到背景上
			outtextxy(1190, 780, "BAD NECK");
			result = Result::NECK_WRONG;
			badNeck++;
			wrongCounter++;
			wrongCounter1++;
			b++;

		}
		else {
			op::opLog("GOOD NECK ", op::Priority::High);
			outtextxy(1190, 780, "GOOD NECK");
			goodCounter++;
			goodNeck++;
		}
	}
	//当0 和 1重合时
	else {
		op::opLog("BAD NECK", op::Priority::High);
		outtextxy(1190, 780, "BAD NECK");
		result = Result::NECK_WRONG;
		wrongCounter++;
		badNeck++;
		b++;
	}
	//SHOULDER
	float leftSX = midX - leftX ;
	float leftSY = midY - leftY ;
	float rate2 = leftSY / leftSX;
	float rightSX = rightX - midX;
	float rightSY = midY - rightY;
	float rate3 = rightSY / rightSX;
	if (!((abs(rate2) < 0.25 && abs(rate3) < 0.25 && rate2 * rate3 > 0)|| //45度
		(abs(rate2) < 0.1 && abs(rate3) < 0.1))){
		op::opLog("BAD SHOULDER", op::Priority::High);
		//显示到背景上 位置1190 735
		outtextxy(1190, 735, "BAD SHOULDER");
		result = Result::SHOULDER_WRONG;
		badShoulder++;
		wrongCounter++;
		wrongCounter1++;
		b++;

	}else {
		op::opLog("GOOD SHOULDER ", op::Priority::High);
		outtextxy(1190, 735, "GOOD SHOULDER");
		goodCounter++;
		goodShoulder++;
	}



	//Sampling three times in a row
	sampleCounter++;
	if (sampleCounter >= 3) {
		op::opLog("end of sampling", op::Priority::High);
		//outtextxy(1490, 780, "end of sampling");
		if (wrongCounter < 2) {//If the wrong poses are not accumulated to 2 in 3 samplings
			result = Result::OKAY;
		}
		wrongCounter = 0;
		sampleCounter = 0;
	}
	else {
		op::opLog("Number of samples" + std::to_string(sampleCounter), op::Priority::High);
		//std::string s = "Number of samples : " + std::to_string(sampleCounter);
		outtextxy(1490, 735, "Number of samples");
		TCHAR s[5];
		_stprintf(s, _T("%d"), sampleCounter);		
		outtextxy(1620, 735, s);

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
	outfile1 << badNeck << ',' << goodNeck << ',' << goodNeck + badNeck << endl;
	outfile1.close();

	ofstream outfile2;
	outfile2.open("shoulder.csv", ios::out | ios::app);
	cout << "Writing to the shoulder" << endl;
	//outfile << "wrongcount" << ',' << "goodcount" << ',' << "total" << endl;
	outfile2 << badShoulder << ',' << goodShoulder << ',' << goodShoulder + badShoulder << endl;
	outfile2.close();

	ofstream outfile3;
	outfile3.open("total.csv", ios::out | ios::app);
	cout << "Writing to the total" << endl;
	//outfile << "wrongcount" << ',' << "goodcount" << ',' << "total" << endl;
	if (b >= 1 ) {
		outfile3 << 0 << ',' << 1 << ',' << interval++ <<endl;
		outfile3.close();
	}
	else {
		outfile3 << 1 << ',' << 0 << ',' << interval++ << endl;
		outfile3.close();
	}


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
int PoseAnalyzer::interval = TIME / 1000;
//----------------------------------------------------------------------
//#include "Camera2.cpp"----
class Camera
{
public:
	void takePicture();
	//尝试视频处理
	void takeVideo();
};
// //2 #include <opencv2/opencv.hpp> using namespace std; using namespace cv;
void Camera::takePicture()
{
	VideoCapture cap(0);			 
	char pic_Name[128] = {};

	if (!cap.isOpened())
	{
		cout << "The camera open failed!" << endl;
	}
	//源代码
	Mat frame;
	cap >> frame;
	//imshow("Camera", frame);	输出图像	
	waitKey(1);
	imwrite("test.jpg", frame);
	////更改图片大小，并保存

	//Size srcSize = Size(640, 356);  //填入任意指定尺寸		
	//Mat frame1;
	//cv::resize(frame, frame1, srcSize, INTER_LINEAR);
	//cv::imwrite("pose.jpg", frame1);
}

void Camera::takeVideo() {
	VideoCapture capture(0);//如果是笔记本，0是自带的摄像头，1是外接的摄像头
	double rate = 25.0;//视频的帧率
	Size videoSize(1280, 960);
	VideoWriter writer("VideoTest.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), rate, videoSize);
	Mat frame;
	while (capture.isOpened())
	{
		capture >> frame;
		writer << frame;
		imshow("video", frame);
		if (waitKey(20) == 27)//27是键盘摁下esc时，计算机接收到的ascii码值
		{
			break;
		}
	}

}
// //1 #include<windows.h> #include<cstdio>

long lastRestTime = 0;
long getTimestamp() {
	time_t t;
	time(&t);
	return t;
}

////--------------------语音播报模块----------------------------
////配置
void MSSSpeak(LPCWSTR speakContent) {//LPCTSTR
	//初始化COM接口
	ISpVoice* pVoice = NULL;
	if (FAILED(::CoInitialize(NULL))) {
		MessageBox(NULL,(LPCSTR)L"COM接口初始化失败！",(LPCSTR)L"提示",
		MB_ICONWARNING | MB_CANCELTRYCONTINUE | MB_DEFBUTTON2);
	}

	//获取SpVoice接口
	HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL,
		CLSCTX_ALL, IID_ISpVoice, (void**)&pVoice);

	if (SUCCEEDED(hr)) {
		pVoice->SetVolume((USHORT)100); //设置音量，范围是 0 -100
		pVoice->SetRate(0); //设置速度，范围是 -10 - 10
		hr = pVoice->Speak(speakContent, 0, NULL);
		pVoice->Release();
		pVoice = NULL;
	}
	//释放com资源
	::CoUninitialize();
}

//string转换成wstring  
std::wstring  StringToWString(const std::string& s) {
	std::wstring wszStr;
	int nLength = MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, NULL, NULL);
	wszStr.resize(nLength);
	LPWSTR lpwszStr = new wchar_t[nLength];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, lpwszStr, nLength);
	wszStr = lpwszStr;
	delete[] lpwszStr;
	return wszStr;
}


//朗读
void read(string temp) {
	wstring a = StringToWString(temp);
	LPCWSTR str = a.c_str();
	/*不知道为什么Cstr不行*/
	MSSSpeak(str);
	cout << "朗读结束\n";
}

//int main() {
//	read("你好，世界");
//	return 0;
//}

//------------------------------------------------------------



int main(int argc, char* argv[])
{
	////朗读测试
	//read("reading test");
	////read("朗读测试");

	// 绘图窗口初始化
	initgraph(1920,1077);
	IMAGE BG;
	loadimage(&BG, _T("BG.jpg"));
	//loadimage(&loading, _T("loading.jpg"));
	putimage(0, 0, &BG);
	//putimage(325, 162, &loading);
	//putimage(862, 162, &loading);
	outtextxy(1190, 695, "RESULT:");

	PoseDetector poseDetector;
	PoseAnalyzer poseAnalyzer;
	Camera camera;
	lastRestTime = getTimestamp();

	//清除之前的数据Clear the perivious data
	ofstream file("data.csv");
	file.trunc;
	file.close();
	ofstream file1("neck.csv");
	file1.trunc;
	file1.close();
	ofstream file2("shoulder.csv");
	file2.trunc;
	file2.close();
	ofstream file3("total.csv");
	file3.trunc;
	file3.close();

	while (true) {
		camera.takePicture();
		//camera.takeVideo();
		op::Array<float> poseKeypoints = poseDetector.detectPose();
		//640 * 480实际图像640 * 356
		IMAGE img1, img2;
		loadimage(&img1, _T("test.jpg"));
		loadimage(&img2, _T("pose.jpg"));
		putimage(325, 162, &img2);
		putimage(862, 162, &img1);

		
		int result = poseAnalyzer.analyse(poseKeypoints);

		//循环结束就将开关量置为0
		b = 0;
		if (result == Result::NECK_WRONG) {
			//MessageBox(GetForegroundWindow(), TEXT("Pay attention to neck posture"), TEXT("SitePoseMonitor"), 1);
			read("attention to your neck posture");
		}
		if (result == Result::SHOULDER_WRONG) {
			//MessageBox(GetForegroundWindow(), TEXT("Pay attention to your shoulder posture"), TEXT("SitePoseMonitor"), 1);
			read("attention to your shoulder posture");
		}
		if (result == Result::NO_PERSON) {
			lastRestTime = getTimestamp();
		}
		op::opLog("have been worked:" + std::to_string(getTimestamp() - lastRestTime), op::Priority::High);
		//打印学习时间


		TCHAR d[5];
		_stprintf(d, _T("%d"), (getTimestamp() - lastRestTime));		// 高版本 VC 推荐使用 _stprintf_s 函数
		outtextxy(1190, 825, d);


		if (getTimestamp() - lastRestTime >= 60 * 60) {//1小时休息一次
			MessageBox(GetForegroundWindow(), TEXT("Take a break, let's do some activity~"), TEXT("SitePoseMonitor"), 1);
		}
		//Sleep(1000*10); //改变速度
		Sleep(TIME);
	}

	return 1;
}
