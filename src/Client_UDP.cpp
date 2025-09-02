#include <opencv2/opencv.hpp>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstring> 
#include <random>
#include <chrono>

int send_tirger(int video_triger)
{ 
   std::random_device rd; // 乱数生成の種
   std::mt19937 gen(rd()); // メルセンヌ・ツイスタ
   std::uniform_int_distribution<> dist1(0, 2); // 0～2の一様分布
   std::uniform_int_distribution<> dist2(3, 5); // 3～5の一様分布
   int random_number;

   int port = 1910;
   const char* rasppi01 = "192.168.50.12"; 
   const char* rasppi02 = "192.168.50.56";
   const char* rasppi03 = "192.168.50.31"; 
   const char* rasppi04 = "192.168.50.32";

   int sockfd;
   sockaddr_in addr01;
   sockaddr_in addr02;
   sockaddr_in addr03;
   sockaddr_in addr04;

   sockfd = socket(AF_INET, SOCK_DGRAM, 0); // ★SOCK_DGRAMに修正
   if (sockfd < 0) 
   {
      std::cerr << "Cannot create socket" << std::endl;
      return -1;
   }

   // rasppi01のアドレスを設定
   memset(&addr01, 0, sizeof(addr01));
   addr01.sin_family = AF_INET;
   addr01.sin_port = htons(port);
   addr01.sin_addr.s_addr = inet_addr(rasppi01);
   
   // rasppi02のアドレスを設定
   memset(&addr02, 0, sizeof(addr02));
   addr02.sin_family = AF_INET;
   addr02.sin_port = htons(port);
   addr02.sin_addr.s_addr = inet_addr(rasppi02);

   // rasppi03のアドレスを設定
   memset(&addr03, 0, sizeof(addr03));
   addr03.sin_family = AF_INET;
   addr03.sin_port = htons(port);
   addr03.sin_addr.s_addr = inet_addr(rasppi03);
   
   // rasppi04のアドレスを設定
   memset(&addr04, 0, sizeof(addr04));
   addr04.sin_family = AF_INET;
   addr04.sin_port = htons(port);
   addr04.sin_addr.s_addr = inet_addr(rasppi04);
   
   char msg_send01[128];
   char msg_send02[128];
   char msg_send03[128];
   char msg_send04[128];


      
   // 左から人を検出した場合
   if(video_triger == 0)
   {
      random_number = dist1(gen);
   }
   // 右から人を検出した場合
   else if(video_triger == 1)
   {
      random_number = dist2(gen);
   }
   else if(video_triger == -1)
   {
      random_number = 6; // 待機動画
   }
   std::snprintf(msg_send01, sizeof(msg_send01), "%d", random_number); // 文字列として格納
   std::snprintf(msg_send02, sizeof(msg_send02), "%d", random_number);
   std::snprintf(msg_send03, sizeof(msg_send03), "%d", random_number);
   std::snprintf(msg_send04, sizeof(msg_send04), "%d", random_number);

   std::cout << "送信するメッセージ: " << random_number << std::endl;


   sendto(sockfd, msg_send01, std::size(msg_send01), 0,(sockaddr*)&addr01,sizeof(addr01)); 
   sendto(sockfd, msg_send02, std::size(msg_send02), 0,(sockaddr*)&addr02,sizeof(addr02));
   sendto(sockfd, msg_send03, std::size(msg_send03), 0,(sockaddr*)&addr03,sizeof(addr03));
   sendto(sockfd, msg_send04, std::size(msg_send04), 0,(sockaddr*)&addr04,sizeof(addr04));

   close(sockfd); 
   return 0;
}

int main() {
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Camera open failed!" << std::endl;
        return -1;
    }

    cv::Mat frame, gray, blur, prev, diff, thresh;
    cv::Mat accumulated;
    bool accumulating = false;
    std::string entrySide = "";

    bool enteredFromLeft = false;
    bool enteredFromRight = false;
    int triger = -1;

    // 検出無効タイマー
    bool detectionDisabled = false;
    auto disableStart = std::chrono::steady_clock::now();
    int disableDuration = 8000; // 8秒

    // 動きが最後にあった時間
    auto lastMotionTime = std::chrono::steady_clock::now();
    int motionTimeout = 10000; // ミリ秒（ここを調整可能にする） ← デフォルト10秒

    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(gray, blur, cv::Size(21, 21), 0);

        if (prev.empty()) {
            blur.copyTo(prev);
            continue;
        }

        cv::absdiff(prev, blur, diff);
        cv::threshold(diff, thresh, 30, 255, cv::THRESH_BINARY);
        blur.copyTo(prev);

        // --- 動き検出ありなら時刻を更新 ---
        if (cv::countNonZero(thresh) > 0) {
            lastMotionTime = std::chrono::steady_clock::now();
        }

        // 無効時間中はスキップ
        if (detectionDisabled) {
            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(now - disableStart).count() > disableDuration) {
                detectionDisabled = false;
                std::cout << "検出再開" << std::endl;
                send_tirger(-1); // 待機動画送信
            } else {
                //cv::imshow("frame", frame);
                //cv::imshow("motion", thresh);
                //if (accumulating) cv::imshow("accumulated", accumulated);
                //if (cv::waitKey(30) == 27) break;
                continue;
            }
        }

        bool left_edge = cv::countNonZero(thresh.colRange(0, 10)) > 0;
        bool right_edge = cv::countNonZero(thresh.colRange(frame.cols - 10, frame.cols)) > 0;

        if(!accumulating){
            if (left_edge && !enteredFromLeft) {
                enteredFromLeft = true;
                std::cout << "左から入場" << std::endl;
                triger = 0; // 左からの検出
                send_tirger(triger); // トリガー送信
            }
            else if (right_edge && !enteredFromRight) {
                enteredFromRight = true;
                std::cout << "右から入場" << std::endl;
                triger = 1; // 右からの検出
                send_tirger(triger); // トリガー送信
            }
        }

        if (!accumulating && (left_edge || right_edge)) {
            accumulating = true;
            accumulated = thresh.clone();
            entrySide = left_edge ? "left" : "right";
            std::cout << (entrySide == "left" ? "左から追跡開始" : "右から追跡開始") << std::endl;
        }

        if (accumulating) {
            cv::bitwise_or(accumulated, thresh, accumulated);

            std::vector<std::vector<cv::Point>> contours;
            cv::findContours(accumulated, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

            if (!contours.empty()) {
                cv::Rect bigBox;
                for (auto &c : contours) {
                    if (cv::contourArea(c) < 500) continue;
                    bigBox = bigBox | cv::boundingRect(c);
                }
                cv::rectangle(frame, bigBox, cv::Scalar(0,255,0), 2);

                if ((entrySide == "right" && bigBox.x < 10) ||
                    (entrySide == "left" && bigBox.x + bigBox.width > frame.cols - 10)) {

                    std::cout << "出場検出 → 累積クリア & 8秒間検出無効" << std::endl;
                    accumulating = false;
                    accumulated.release();
                    entrySide.clear();

		    enteredFromLeft = false;
		    enteredFromRight = false;
                    detectionDisabled = true;
                    disableStart = std::chrono::steady_clock::now();
                }
            }

            // --- motionTimeout ミリ秒間動きがなかったら累積終了 ---
            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastMotionTime).count() > motionTimeout) {
                accumulating = false;
                accumulated.release();
                entrySide.clear();
                std::cout << "累積終了（" << motionTimeout/1000 << "秒間動きなし）" << std::endl;
            }
        }

         //cv::imshow("frame", frame);
         //cv::imshow("motion", thresh);
         //if (accumulating) cv::imshow("accumulated", accumulated);

         //if (cv::waitKey(30) == 27) break;
    }
    return 0;
}
