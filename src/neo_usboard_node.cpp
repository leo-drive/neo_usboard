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

#include <../include/neo_usboard_node.h>

//#######################
//#### main programm ####
int main(int argc, char** argv)
{
    // initialize ROS
    ros::init(argc, argv, "neo_usboard_node");
    neo_usboard_node node;

    if(node.init() != 0) return 1;

    //wait 5 sec after init for all topics to be ready!
    sleep(5);

    ros::Duration rdTimeOutDuration(1.0); //[s]
    ros::Time rtNextTimeReqData;
    ros::Time rtTimeOut; // = ros::Time::now();
    ros::Rate r(30.0); //Frequency Thread Cycling [Hz]

    int iCurrentState = 0;  // 0 = Read ParameterSet
                            // 1 = Write ParameterSet
                            // 2 = Publish Data (Request Mode)
                            // 3 = Publish Data (Automatic Mode)

    bool bRequestedParameterSet = false;
    bool bRequestedSensorData = false;
    int iWriteParamSetPartX = 0;
    bool bWaitForConfirmParamSetPartX = false;

    while(node.n.ok())
    {
        if(iCurrentState == 0)
        {
            if(!bRequestedParameterSet)
            {
                //request current parameter set
                ROS_INFO("USBoard: requesting parameter set");
                bool ret = false;
                ret = node.requestParameterSet();
                bRequestedParameterSet = true;
            }
            else
            {
                //Wait until complete ParameterSet was received
		//if done go to state 1
                if(node.receivedParameterSet())
                {
                    iCurrentState = 1;
                    ROS_INFO("USBoard: parameter set received");
                }
                else
                {
                    //Wait
                    ROS_INFO("USBoard: waiting for parameter Set");
                }
            }
        }
	else if(iCurrentState == 1)
	{

            if(!bWaitForConfirmParamSetPartX)
            {
                ROS_INFO("USBoard: writing parameter set part %d/9", iWriteParamSetPartX+1);
                node.writeParameterSetPartX(iWriteParamSetPartX);
                bWaitForConfirmParamSetPartX = true;
            }
            else
            {
                //wait for confirmation
                if(node.confirmedParameterSetPartX(iWriteParamSetPartX))
                {
                    ROS_INFO("USBoard: confirmed part %d/9", iWriteParamSetPartX+1);
                    bWaitForConfirmParamSetPartX = false;

                    if(iWriteParamSetPartX == 8)
                    {
                        iWriteParamSetPartX = 0;

                        //write param set to can or serial
                        if(node.getMode() == 1)
                        {
                            ROS_INFO("USBoard: Automatic Mode");
                            iCurrentState = 3;
                        }
                        else
                        {
                            ROS_INFO("USBoard: Request Mode");
                            iCurrentState = 2;
                        }
                    }
                    else
                    {
                        iWriteParamSetPartX++;
                    }
                }
                else
                {
                     //ROS_INFO("USBoard: waiting for Paramset confirmation");
                }
            }
	}
        else if(iCurrentState == 2)
        {
            if(node.getDataRequestService())
            {
                if(!bRequestedSensorData)
                {
                    //request sensor data
                    bool ret = false;
                    ret = node.requestSensorData();
                    bRequestedSensorData = true;
                    rtTimeOut = ros::Time::now() + ros::Duration(1.0);
                }
            }
            if(bRequestedSensorData)
            {
                //Wait until complete sensor data was received

                if(node.receivedSensorData())
                {
                    //publish data to topic
                    //ROS_INFO("Data published");
                    node.publishUSBoardData();
                    bRequestedSensorData = false;
                }
                else
                {
                    //Wait and check for timeout
                    if(ros::Time::now() > rtTimeOut)
                    {
                        ROS_ERROR("USBoard: Timeout: no answer reveived!");
                        bRequestedSensorData = false;
                    }
                }
            }
        }//END State == 2
        else if(iCurrentState == 3)
        {
            //Wait until complete sensor data was received
            if(node.receivedSensorData())
            {
                //publish data to topic
                node.publishUSBoardData();
            }
        }//END State == 3

        //wait to complete cycle time
        ros::spinOnce();
        r.sleep();

    }

	return 0;

}
