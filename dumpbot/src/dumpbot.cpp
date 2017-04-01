/**
 * @file /dumpbot/include/dumpbot/dumpbot.hpp
 *
 * @brief dump robot take out trash
 *
 * This is dump robot just take out trash based on navigation map.
 *
 * @author Carl
 *
 **/


/*****************************************************************************
** Includes
*****************************************************************************/

#include <string>
#include <ros/ros.h>
#include <pluginlib/class_list_macros.h>
#include <nodelet/nodelet.h>
#include <std_msgs/Empty.h>
#include <std_msgs/String.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/PoseWithCovarianceStamped.h>
#include <geometry_msgs/PoseStamped.h>
#include <nav_msgs/MapMetaData.h>
#include <nav_msgs/OccupancyGrid.h>
#include <iostream>
#include <stdio.h>
#include <ros/callback_queue.h>
#include <fstream>
#include <sstream>
#include <std_msgs/Header.h>
#include <stdlib.h>

namespace dumpbot
{

/**
 * @ brief Keeps msgs from navigation and send goal for taking out trash
 *
 * This Dumpbot will subscribe msgs from navigation and send goal where 
 * should taking out trash, also concert with command fo judge whether get object.
 *
 */
class Dumpbot : public nodelet::Nodelet
{
public:
  Dumpbot() : goal_x_(0.0),goal_y_(0.0),
              goal_z_(0.0){}

  ~Dumpbot(){
  }
private:
  double goal_x_; /**< The position x of goal where dumpbot dumping garbage. >*/
  double goal_y_; /**< The position y of goal where dumpbot dumping garbage. >*/
  double goal_z_; /**< The position z of goal where dumpbot dumping garbage. >*/
  double orien_z_; /**< The orientation z of goal where dumpbot dumping garbage. >*/


  /*!
   * @brief OnInit method from node handle.
   * OnInit method from node handle. Sets up the parameters
   * and topics.
   */
  virtual void onInit()
  {
    ros::NodeHandle& nh = getNodeHandle();
    ros::NodeHandle& private_nh = getPrivateNodeHandle();

    private_nh.getParam("goal_posistion_x", goal_x_);
    private_nh.getParam("goal_posistion_y", goal_y_);
    private_nh.getParam("goal_posistion_z", goal_z_);
    private_nh.getParam("goal_orientation_z", orien_z_);
    ROS_INFO_STREAM("Init goal_position_x: "<<goal_x_<<" goal_position_y: "<<goal_y_<<" goal_position_z: "<<goal_z_<<" goal_orientation_z: "<<orien_z_<<" .");
    goalpub_ = private_nh.advertise<geometry_msgs::PoseStamped>("/move_base_simple/goal",1);
    sub_ = nh.subscribe<std_msgs::String>("/dumpbot_serial_func/data", 1, &Dumpbot::changeDataCb, this);
  }
  
  int hexstring2char(const std::string& str, char* out){
    int n = 0;
    int temp;
    std::istringstream iss(str);
    iss >> std::hex;
    while (iss >> temp)
    {
        out[n++] = temp;
        iss.get();
    }
    return n;
  }
  /*!
   * @brief Callback for Dumpbot's MCU data.
   * Callback for Dumpbot's MCU data. It finds whether
   * go take out trash or not.
   * @param MCU data message.
   */
  void changeDataCb(const std_msgs::String::ConstPtr& data){
    std::string determineData= data->data.c_str();
    char buff[1024];
//	ROS_INFO("Yes! I heard:[%s]",determineData.c_str());
    std::cout << hexstring2char(determineData, buff);  //buff中输出结果
    ROS_INFO_STREAM("Yes! I heard:[" <<buff<<" ].");
	// load and parse data into int 
		
	// if data right then send goal
    geometry_msgs::PoseStamped next_goal;
    next_goal.header.frame_id = "/map";
    next_goal.pose.position.x = goal_x_;
    next_goal.pose.position.y = goal_y_;
    next_goal.pose.position.z = goal_z_;
    next_goal.pose.orientation.x = 0.0;
    next_goal.pose.orientation.y = 0.0;
    next_goal.pose.orientation.z = orien_z_;
    next_goal.pose.orientation.w = 1.0;
    goalpub_.publish(next_goal);
  }
    
  ros::Subscriber sub_;
  ros::Publisher goalpub_;
  

};

PLUGINLIB_DECLARE_CLASS(dumpbot, Dumpbot, dumpbot::Dumpbot, nodelet::Nodelet);
} // namespace dumbot

