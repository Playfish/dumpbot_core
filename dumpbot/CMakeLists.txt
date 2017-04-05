cmake_minimum_required(VERSION 2.8.3)
project(dumpbot)

find_package(catkin REQUIRED COMPONENTS geometry_msgs nodelet pluginlib nav_msgs std_msgs roscpp dumpbot_msgs)
find_package(Boost REQUIRED)

catkin_package(
  INCLUDE_DIRS
  LIBRARIES ${PROJECT_NAME}
  CATKIN_DEPENDS geometry_msgs nodelet roscpp nav_msgs std_msgs pluginlib dumpbot_msgs
  DEPENDS Boost 
)

###########
## Build ##
###########

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
)

## Declare a cpp library
add_library(${PROJECT_NAME} src/dumpbot.cpp)
add_dependencies(${PROJECT_NAME} dumpbot_msgs_gencpp)
add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_gencfg)

## Specify libraries to link a library or executable target against
target_link_libraries(${PROJECT_NAME}
  ${catkin_LIBRARIES}
)


#############
## Install ##
#############

## Mark executables and/or libraries for installation
install(TARGETS ${PROJECT_NAME} 
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

## Mark all other required files for installation

install(DIRECTORY launch
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)

install(DIRECTORY include/${PROJECT_NAME}/
        DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
)

install(DIRECTORY plugins
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)

