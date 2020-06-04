#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <atomic>
#include <chrono>
#include <cstring>
#include <iostream>
#include <thread>
#include "HighResClock.hpp"
#include "MvCameraControl.h"

// 等待用户输入enter键来结束取流或结束程序
// wait for user to input enter to stop grabbing or end the sample program
void PressEnterToExit(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
    fprintf(stderr, "\nPress enter to exit.\n");
    while (getchar() != '\n')
        ;
}

bool PrintDeviceInfo(MV_CC_DEVICE_INFO* pstMVDevInfo) {
    if (NULL == pstMVDevInfo) {
        printf("The Pointer of pstMVDevInfo is NULL!\n");
        return false;
    }
    if (pstMVDevInfo->nTLayerType == MV_USB_DEVICE) {
        printf("Device Model Name: %s\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.chModelName);
    } else {
        printf("Not support.\n");
    }

    return true;
}

int main() {
    int nRet = MV_OK;

    void* handle = NULL;

    MV_CC_DEVICE_INFO_LIST stDeviceList;
    memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));

    // 枚举设备
    // enum device
    nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &stDeviceList);
    if (MV_OK != nRet) {
        printf("MV_CC_EnumDevices fail! nRet [%x]\n", nRet);
        return 1;
    }
    if (stDeviceList.nDeviceNum > 0) {
        for (int i = 0; i < stDeviceList.nDeviceNum; i++) {
            printf("[device %d]:\n", i);
            MV_CC_DEVICE_INFO* pDeviceInfo = stDeviceList.pDeviceInfo[i];
            if (NULL == pDeviceInfo) {
                return 1;
            }
            PrintDeviceInfo(pDeviceInfo);
        }
    } else {
        printf("Find No Devices!\n");
        return 1;
    }

    printf("Please Intput camera index: ");
    unsigned int nIndex = 0;
    scanf("%d", &nIndex);

    if (nIndex >= stDeviceList.nDeviceNum) {
        printf("Intput error!\n");
        return 1;
    }
    // 选择设备并创建句柄
    // select device and create handle
    nRet = MV_CC_CreateHandle(&handle, stDeviceList.pDeviceInfo[nIndex]);
    if (MV_OK != nRet) {
        printf("MV_CC_CreateHandle fail! nRet [%x]\n", nRet);
        return 1;
    }

    // 打开设备
    // open device
    nRet = MV_CC_OpenDevice(handle);
    if (MV_OK != nRet) {
        printf("MV_CC_OpenDevice fail! nRet [%x]\n", nRet);
        return 1;
    }

    MV_CC_SetEnumValue(handle, "UserSetSelector", 0 /*default*/);
    MV_CC_SetCommandValue(handle, "UserSetLoad");
    MV_CC_SetEnumValue(handle, "UserSetDefault", 0 /*default*/);

    MV_CC_SetEnumValue(handle, "ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
    MV_CC_SetFloatValue(handle, "ExposureTime", 200);

    MV_CC_SetEnumValue(handle, "TriggerMode", false);
    MV_CC_SetEnumValue(handle, "TriggerSource", MV_TRIGGER_SOURCE_SOFTWARE);

    MV_CC_SetEnumValue(handle, "AcquisitionMode", MV_ACQ_MODE_CONTINUOUS);
    MV_CC_SetBoolValue(handle, "AcquisitionFrameRateEnable", true);
    MV_CC_SetFloatValue(handle, "AcquisitionFrameRate", 100000);

    MV_CC_SetIntValue(handle, "Height", 104);
    MV_CC_SetIntValue(handle, "Width", 2592);
    MV_CC_SetEnumValue(handle, "PixelFormat", PixelType_Gvsp_Mono8);

    MV_CC_StartGrabbing(handle);


    std::thread t1([](void* handle){
        MV_FRAME_OUT stOutFrame;
        memset(&stOutFrame, 0, sizeof(MV_FRAME_OUT));
        HighResClock clock("", 1000);
        while (true) {
//            std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

            clock.start(1);

            MV_CC_GetImageBuffer(handle, &stOutFrame, 10000);
            MV_CC_FreeImageBuffer(handle, &stOutFrame);

            clock.stop(1);

//            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
//            std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;

        }
    }, handle);

    t1.join();

    //    PressEnterToExit();

    //    // 停止取流
    //    // end grab image
    //    nRet = MV_CC_StopGrabbing(handle);
    //    if (MV_OK != nRet) {
    //        printf("MV_CC_StopGrabbing fail! nRet [%x]\n", nRet);
    //        break;
    //    }

    //    // 关闭设备
    //    // close device
    //    nRet = MV_CC_CloseDevice(handle);
    //    if (MV_OK != nRet) {
    //        printf("MV_CC_CloseDevice fail! nRet [%x]\n", nRet);
    //        break;
    //    }

    //    // 销毁句柄
    //    // destroy handle
    //    nRet = MV_CC_DestroyHandle(handle);
    //    if (MV_OK != nRet) {
    //        printf("MV_CC_DestroyHandle fail! nRet [%x]\n", nRet);
    //        break;
    //    }

    //    if (nRet != MV_OK) {
    //        if (handle != NULL) {
    //            MV_CC_DestroyHandle(handle);
    //            handle = NULL;
    //        }
    //    }

    //    printf("exit\n");

    return 0;
}
