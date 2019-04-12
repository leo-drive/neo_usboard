/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2017, Neobotix GmbH
 *  All rights reserved.
 *
 * Author: Jan-Niklas Nieland
 *
 * Date of creation: July 2017
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the Neobotix nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/

// ROS includes
#include <ros/ros.h>
#include <iostream>
#include <SerUSBoard.h>
#include <CANUSBoard.h>

// ROS message includes
#include <neo_msgs/USBoard.h>
#include <sensor_msgs/Range.h>
#include <can_msgs/Frame.h>
#include <std_srvs/Empty.h>

// ROS service includes
//--

// external includes
//--

//####################
//#### node class ####
class neo_usboard_node
{

    public:

        // Constructor
        neo_usboard_node()
        {
            usboard_available = false;
            usboard_online = false;
            usboard_timeout_ = 0.5;
            m_bDataRequestedviaService = false;

            m_iMode = 1;
        }

        ~neo_usboard_node()
        {
            delete m_SerUSBoard;
        }

        // create a handle for this node, initialize node
        ros::NodeHandle n;

        //basic topics
        ros::Publisher topicPub_usBoard;
        ros::Publisher topicPub_USRangeSensor1;
        ros::Publisher topicPub_USRangeSensor2;
        ros::Publisher topicPub_USRangeSensor3;
        ros::Publisher topicPub_USRangeSensor4;
        ros::Publisher topicPub_USRangeSensor5;
        ros::Publisher topicPub_USRangeSensor6;
        ros::Publisher topicPub_USRangeSensor7;
        ros::Publisher topicPub_USRangeSensor8;
        ros::Publisher topicPub_USRangeSensor9;
        ros::Publisher topicPub_USRangeSensor10;
        ros::Publisher topicPub_USRangeSensor11;
        ros::Publisher topicPub_USRangeSensor12;
        ros::Publisher topicPub_USRangeSensor13;
        ros::Publisher topicPub_USRangeSensor14;
        ros::Publisher topicPub_USRangeSensor15;
        ros::Publisher topicPub_USRangeSensor16;

        //Service
        ros::ServiceServer srv_request_data;

        //ROS-CAN-Interface
        ros::Subscriber topicSub_CANRecMsgs;
        ros::Publisher topicPub_CANSendMsgs;


        void PublishUSBoardData();
        void publishUSBoardData();
        int readUSBoardData();

        //new wraper functions
        bool requestParameterSet();
        bool receivedParameterSet();
        bool writeParameterSetPartX(int iPart);
        bool confirmedParameterSetPartX(int iPart);

        int init();
        int requestSensorData();
        bool receivedSensorData();
        int requestBoardStatus();
        int requestActivateChannels();
        int requestSensorReadings1TO8();
        int requestSensorReadings9TO16();
        int requestAnalogreadings();

        void readParameter();
        int getMode();
        double getRequestRate();
        double getTimeOut();
        bool getDataRequestService();

        bool ros_callback_req_data(std_srvs::Empty::Request& request, std_srvs::Empty::Response& response);


    private:

        //Interface
        bool m_bUseCANInterface;
        CANUSBoard * m_CANUSBoard;
        std::string m_sComPort;
        SerUSBoard * m_SerUSBoard;


        //Parametrization
        bool m_bWriteParameter;
        bool m_bWriteParameterToEEPROM;

        struct ParameterSet
        {
            int iCANBaudRate;
            int8_t iBytesCANBaseAddress[4];
            bool bCANExtendedID;
            int iSendMode;
            int iSendTimeInterval;
            bool bSensorActive[16];
            int iWarningDistances[16];
            int iAlarmDistances[16];
            int iSerialNumber;
        };

        double requestRate;
        double usboard_timeout_;

        ros::Time time_last_message_received_;
        bool usboard_online; //the usboard is sending messages at regular time
        bool usboard_available; //the usboard has sent at least one message -> publish topic
        int m_iMode;
        int m_iInterval;
        double m_dTimeOut;
        bool m_bSensorActive[16];
        std::string m_sSensorFrame[16];
        int m_iSensorWarnDist[16];  //[cm]
        int m_iSensorAlarmDist[16]; //[cm]
        double m_dSensorMinRange;
        double m_dSensorMaxRange;

        bool m_bDataRequestedviaService;

        //log
        bool log;  //enables or disables the log for neo_usboard
};




