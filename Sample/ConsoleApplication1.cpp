#include "stdafx.h"
#include <windows.h>
#include "PSMoveService/PSMoveClient_CAPI.h"

PSMControllerList controllerList;
//PSMTrackerList trackerList;
PSMHmdList hmdList;

int main()
{
	SetConsoleTitle(_T("PSMoveService - get position tracking data"));

	bool success = true;

	if (PSM_Initialize(PSMOVESERVICE_DEFAULT_ADDRESS, PSMOVESERVICE_DEFAULT_PORT, PSM_DEFAULT_TIMEOUT) != PSMResult_Success)
	{
		printf("Failed to initialize the client network manager");
		success = false;
	}
	

	if (success)
	{
		memset(&controllerList, 0, sizeof(PSMControllerList));
		PSM_GetControllerList(&controllerList, PSM_DEFAULT_TIMEOUT);

		//memset(&trackerList, 0, sizeof(PSMTrackerList));
		//PSM_GetTrackerList(&trackerList, PSM_DEFAULT_TIMEOUT);

		memset(&hmdList, 0, sizeof(PSMHmdList));
		PSM_GetHmdList(&hmdList, PSM_DEFAULT_TIMEOUT);

		// Register as listener and start streams
		unsigned int data_stream_flags =
			PSMControllerDataStreamFlags::PSMStreamFlags_includePositionData |
			PSMControllerDataStreamFlags::PSMStreamFlags_includePhysicsData |
			PSMControllerDataStreamFlags::PSMStreamFlags_includeCalibratedSensorData |
			PSMControllerDataStreamFlags::PSMStreamFlags_includeRawTrackerData;

		//HMD
		if (hmdList.count > 0) {
			if (PSM_AllocateHmdListener(hmdList.hmd_id[0]) != PSMResult_Success) 
				success = false;
			
			if (PSM_StartHmdDataStream(hmdList.hmd_id[0], data_stream_flags, PSM_DEFAULT_TIMEOUT) != PSMResult_Success) 
				success = false;
		}

		//Controller1
		if (controllerList.count > 0) {
			if (PSM_AllocateControllerListener(controllerList.controller_id[0]) != PSMResult_Success) 
				success = false;
			
			if (PSM_StartControllerDataStream(controllerList.controller_id[0], data_stream_flags, PSM_DEFAULT_TIMEOUT) != PSMResult_Success) 
				success = false;
		}

		//Controller2
		if (controllerList.count > 1) {
			if (PSM_AllocateControllerListener(controllerList.controller_id[1]) != PSMResult_Success)
				success = false;

			if (PSM_StartControllerDataStream(controllerList.controller_id[1], data_stream_flags, PSM_DEFAULT_TIMEOUT) != PSMResult_Success)
				success = false;
		}
	}

	PSMVector3f hmdPose;
	PSMVector3f controller1Pose, controller2Pose;

	while (success)
	{
		if ((GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0) break;

		PSM_Update();

		system("cls");

		if (hmdList.count > 0) {
			PSM_GetHmdPosition(hmdList.hmd_id[0], &hmdPose);
			printf("hmd = %5.2f, Y=%5.2f, Z=%5.2f\r\n", hmdPose.x, hmdPose.y, hmdPose.z);
		}

		if (controllerList.count > 0) {
			PSM_GetControllerPosition(controllerList.controller_id[0], &controller1Pose);
			printf("controller1 = %5.2f, Y=%5.2f, Z=%5.2f\r\n", controller1Pose.x, controller1Pose.y, controller1Pose.z);
		}

		if (controllerList.count > 1) {
			PSM_GetControllerPosition(controllerList.controller_id[1], &controller2Pose);
			printf("controller2 = %5.2f, Y=%5.2f, Z=%5.2f%f\r\n", controller2Pose.x, controller2Pose.y, controller2Pose.z);
		}

		printf("\r\nPress escape to exit");
	}

	if (hmdList.count > 0)
	{
		PSM_StopHmdDataStream(hmdList.hmd_id[0], PSM_DEFAULT_TIMEOUT);
		PSM_FreeHmdListener(hmdList.hmd_id[0]);
	}

	if (controllerList.count > 0)
	{
		PSM_StopControllerDataStream(controllerList.controller_id[0], PSM_DEFAULT_TIMEOUT);
		PSM_FreeControllerListener(controllerList.controller_id[0]);
	}

	if (controllerList.count > 1)
	{
		PSM_StopControllerDataStream(controllerList.controller_id[1], PSM_DEFAULT_TIMEOUT);
		PSM_FreeControllerListener(controllerList.controller_id[1]);
	}

	PSM_Shutdown();

    return 0;
}