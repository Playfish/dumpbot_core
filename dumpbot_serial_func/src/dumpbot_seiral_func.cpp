/**
 * @file /dumpbot_serial_func/src/dumpbot_serial_func.cpp
 *
 * @brief Implementation for dirver with read data from dumpbot nodelet
 *
 * @author Carl
 *
 **/

/*****************************************************************************
 ** Includes
 *****************************************************************************/
#include <string>
#include <ros/ros.h>
#include <std_msgs/String.h>
#include <nodelet/nodelet.h>
#include <ecl/threads/thread.hpp>
#include <pluginlib/class_list_macros.h>
#include <dumpbot_serial_func/dumpbot_seiral_func.hpp>


namespace dumpbot_seiral_func{

class DumpbotSerialFunc : public nodelet::Nodelet{
#define BUFSIZE 15

public:
  DumpbotSerialFunc() : shutdown_requested_(false),serialNumber_("/dev/serial/by-id/usb-1a86_USB2.0-Serial-if00-port0"){}

  ~DumpbotSerialFunc(){
    NODELET_DEBUG_STREAM("Waiting for update thread to finish.");
    shutdown_requested_ = true;
    update_thread_.join();
  }

  virtual void onInit(){

    ros::NodeHandle nh = this->getPrivateNodeHandle();
    std::string name = nh.getUnresolvedNamespace();
    nh.getParam("serialNumber", serialNumber_);
    nh.getParam("baudRate", baudRate_);
    rcv_cnt = 0;
    success_flag = 0;

    fd = open(serialNumber_.c_str(), O_RDWR | O_NOCTTY | O_NDELAY );
    if(fd < 0){
      ROS_ERROR_STREAM("Open Serial: "<<serialNumber_.c_str()<<" Error!");
      exit(0);
    }

    int count=0;
    cmdpub_ = nh.advertise<std_msgs::String>("data",100);
    memset(buf, 0, sizeof(buf));
    memset(temp_buf, 0, sizeof(temp_buf));
    memset(result_buf, 0, sizeof(result_buf));
    dumpbotdriver_.UART0_Init(fd,baudRate_,0,8,1,'N');
    ROS_INFO_STREAM("Open serial: ["<< serialNumber_.c_str() <<" ] successful, with idex: "<<fd<<".");
    NODELET_INFO_STREAM("DumpbotSerialFunc initialised. Spinning up update thread ... [" << name << "]");
    update_thread_.start(&DumpbotSerialFunc::update, *this);
  }
private:
  void update(){
    rcv_cnt = 0;
    success_flag = 0;
    ros::Rate spin_rate(10);
    memset(buf, 0, sizeof(buf));
    memset(temp_buf, 0, sizeof(temp_buf));
    memset(result_buf, 0, sizeof(result_buf));
    ROS_INFO_STREAM("Begin receive data from "<<serialNumber_.c_str()<<", with idex:"<<fd<<".");
    fd = open(serialNumber_.c_str(), O_RDWR | O_NOCTTY | O_NDELAY );
    dumpbotdriver_.UART0_Init(fd,baudRate_,0,8,1,'N');
    while (! shutdown_requested_ && ros::ok())
    {
//    ROS_INFO_STREAM("count: "<<count<<".");
      std::stringstream ostream;
      std::stringstream ostream_test;
      std_msgs::String testmsg;

      len = dumpbotdriver_.UART0_Recv(fd, buf,40);  
      if(len>0){
        for(int i = 0; i < len; i++){
          if(rcv_cnt<=14){
            result_buf[rcv_cnt++] = buf[i];
            ROS_DEBUG_STREAM("rcv_cnt<=14");
            if(rcv_cnt == 15){
              success_flag = 1;
              ROS_DEBUG_STREAM("success_flag: "<<success_flag<<" .");
            }// end if(rcv_cnt == 15)
          }// end if(rcv_cnt <= 14)
          else{
            /****
             *  checkout received data
             */
            ROS_INFO_STREAM("Received data, with :[");
            for(int j=0;j<BUFSIZE;j++){
              printf("%x ",(unsigned char) result_buf[j]);
            }
            printf("] \n");
            /****
             *  send data to topic
             */
            if(0x22 == result_buf[0] && 1 == success_flag){
              for(int j=0;j<BUFSIZE;j++){
//                ostream <<"test: ["<<count<<"]";
                ostream <<hex << (unsigned char)result_buf[j]<<" "<<std::dec;
               }
              testmsg.data = ostream.str();
              ROS_INFO_STREAM("Received data: ["<<testmsg.data.c_str()<<"]");
              cmdpub_.publish(testmsg);
             }
            success_flag = 0;
            /****
             *  data writing end
             */
            if(0x22 == buf[i])
            {
              rcv_cnt = 0;
              result_buf[rcv_cnt++] = buf[i];
              ROS_INFO_STREAM("buf["<<i<<"] == 0x22");
            }           
          }// end else 
        }// end if(len>0)
      }// end while
      spin_rate.sleep();
//      count++;
    }

    ROS_INFO_STREAM("Shotdown and close serial: "<<serialNumber_.c_str()<<".");
    dumpbotdriver_.UART0_Close(fd);
  }
private:
  int fd, len, rcv_cnt;
  int success_flag;
  char buf[40], temp_buf[BUFSIZE],result_buf[BUFSIZE];
  
  DumpbotDriver dumpbotdriver_;
  ecl::Thread update_thread_;
  bool shutdown_requested_;
  ros::Publisher cmdpub_;
  // ROS Parameters
  std::string serialNumber_;
  int baudRate_;
  
    
    
};

} //namespace dumpbot_serial_func
PLUGINLIB_EXPORT_CLASS(dumpbot_seiral_func::DumpbotSerialFunc,
                        nodelet::Nodelet);
