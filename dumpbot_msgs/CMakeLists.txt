cmake_minimum_required(VERSION 2.8.3)
project(dumpbot_msgs)

find_package(catkin REQUIRED COMPONENTS std_msgs sensor_msgs message_generation)

add_message_files(DIRECTORY msgs
                      FILES SensorData.msg
                 )

generate_messages(DEPENDENCIES std_msgs sensor_msgs)

catkin_package(CATKIN_DEPENDS std_msgs sensor_msgs message_runtime)
