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
    rcv_cnt = 0;
    success_flag = 0;
    error_cnt = 0;
    msg = "We have open serial!";

    fd = open(serialNumber_.c_str(), O_RDWR | O_NOCTTY | O_NDELAY );
    if(fd < 0){
      ROS_ERROR_STREAM("Open Error!");
      exit(0);
    }

    cmdpub_ = nh.advertise<std_msgs::String>("data",100);
    memset(buf, 0, sizeof(buf));
    memset(temp_buf, 0, sizeof(temp_buf));
    memset(result_buf, 0, sizeof(result_buf));
    dumpbotdriver_.UART0_Init(fd,2400,0,8,1,'N');
    ROS_INFO_STREAM("msg: ["<< msg <<" ]");
    NODELET_INFO_STREAM("DumpbotSerialFunc initialised. Spinning up update thread ... [" << name << "]");
    update_thread_.start(&DumpbotSerialFunc::update, *this);
  }
private:
  void update(){

    ros::Rate spin_rate(10);
//    int count=0;
    while (! shutdown_requested_ && ros::ok())
    {
      std::stringstream ostream;
      std_msgs::String testmsg;

      len = dumpbotdriver_.UART0_Recv(fd, buf,40);  
	  if(len>0){
		for(int i = 0; i < len; i++){
		  if(rcv_cnt<=14){
			result_buf[rcv_cnt++] = buf[i];
			if(rcv_cnt == 15){
			  success_flag = 1;
			}else{
			  //============ send data====================
			  ostream << "{ " ;
			  ostream << std::setfill('0') << std::uppercase;
              if(0x22 == result_buf[0] && 1 == success_flag){
				for(int j=0;j<BUFSIZE;j++){
//				  ostream <<"test: ["<<count<<"]";
				  ostream <<hex << (unsigned char)result_buf[j]<<" "<<std::dec;
			 	}
			  	  ostream << "}";
		    	  testmsg.data = ostream.str();
    		//	  ROS_INFO("%s", testmsg.data.c_str());
				  cmdpub_.publish(testmsg);
				  success_flag = 0;
			  }
			}
		  }
		}
	  }
	  dumpbotdriver_.UART0_Close(fd);
      spin_rate.sleep();
//	  count++;
    }
  }
private:
    int fd, len, rcv_cnt;
    int success_flag;
    int error_cnt;
    char buf[40], temp_buf[BUFSIZE],result_buf[BUFSIZE];
    
    const char *msg;
    fd_set rds;
    DumpbotDriver dumpbotdriver_;
    ecl::Thread update_thread_;
    bool shutdown_requested_;
	ros::Publisher cmdpub_;
	std::string serialNumber_;
    
};

} //namespace dumpbot_serial_func
PLUGINLIB_EXPORT_CLASS(dumpbot_seiral_func::DumpbotSerialFunc,
                        nodelet::Nodelet);
