# Qeexo AutoML Static Library Application Guide

  Qeexo AutoML Static Library are genereated by Automl WebUI, Which provde the classify interface that trained with the data collected by Automl WebUI, you can easily get this library file from Automl WebUI, and easily apply the Qeexo AutoML classify technology by integrating the static library to your embeded device.

  This document is intended as a help to the integration process.

### Get the static library package from WebUI

You can Download the Package 

![](https://github.com/leomonan/listdelsame/blob/master/download_the_static_library_package.jpg?token=AGRW7CKHCT6HMMUYIL6EWVDAUSZWM)

  Framework layer and Apps layer contain logic of setting up data logger and inference application running on board. These two layers can be reused directly on different device platforms and don't need any changes during porting. 

  HAL layer encapsulates all the interfaces to hardware and platform specific code, which need to be implemented in porting process.

### Board requirement

  Your device board should at least have: 

🔹 Arm Cortex-M series Core, from M0 to above

🔹 A RTOS system which supports multi-threads and scheduling

🔹 USB interface with at least USB CDC driver enabled

🔹 On-board sensors or external sensors connected to MCU via SPI or I2C.

> BLE is not a mandatory requirement, it could be a good wireless communication way between device and AutoML platform.

### Step 1. Implement your HAL

  Qeexo AutoML Embedded SDK release package includes directories as follow:

```
├── Apps
│   └── QxDataLogApp.cpp
├── Examples
│   └── datalog
│       ├── Inc
│       │   ├── QxAutoMLConfig.h
│       │   └── main.h
│       └── Src
│           └── main.cpp
├── Framework
│   ├── QxApplication.cpp
│   ├── QxAutoMLFramework.cpp
│   └── QxSensors.cpp
├── Hal_Impl
│   ├── QxBTHal_XXX.c
│   ├── QxOS_XXX.c
│   ├── QxSensorHal_XXX.c
│   └── QxUSBHal_XXX.c
├── Include
│   ├── Apps
│   │   └── QxDataLogApp.h
│   ├── Framework
│   │   ├── QxApplication.h
│   │   ├── QxAutoMLFramework.h
│   │   └── QxSensors.h
│   ├── Hal
│   │   ├── QxBTHal.h
│   │   ├── QxOS.h
│   │   ├── QxSensorHal.h
│   │   └── QxUSBHal.h
│   └── QxTypeDefs.h
├── Test
├── Docs
```

  Copy folders `Apps`, `Framework`, `Include` and `Hal_Impl`  to board's firmware project. Implement Sensor, USB and OS interfaces under `Hal_Impl` directory. All the HAL files are implemented in **C** language.

####   1. `QxSensorHal_XXX.c`

  Rename file `QxSensorHal_XXX.c` to adapt your own platform. (e.g. `QxSensorHal_SensorTile.box.c`)  

  This file includes all the handles to control sensors on the board. All the information of one specific sensor is well organized in below 3 key structures:

#####   Structure_1: `tQxSensorDevice`  include all the properties and operations to a sensor.

```c
typedef struct QxSensorDevice_t {
    char *sensorName; 
    tQxSensorType type; 
    tQxSensorSpec spec; 
    tQxSensorOps op; 
}tQxSensorDevice;
```

- `*sensorName` points to a constant string which you can name to identify the specific sensor.
- `type`  should be one of the enumeration members defined in `tQxSensorType`. If your board has a new sensor type which doesn't exist in the `tQxSensorType` list, add the new type in the enumeration.

  Below is an example to describe accelerometer sensor *LSM6DSO* using the structure.

```c
		{
				"LSM6DSO_ACC", //sensor name
				QXSENSOR_TYPE_ACCEL, //sensor type
				{lsm6dso_acc_odr, lsm6dso_acc_fs, TRUE, TRUE}, //sensor spec
				{
					.init = lsm6dso_acc_init,
					.enable = lsm6dso_acc_enable,
					.read_pre = lsm6dso_read_preprocess,
					.read = lsm6dso_acc_read,
				}
		}
```

#####   Structure_2: `tQxSensorSpec` include sensor properties like supported odrs, **scale ranges**,  **fifo support or not and **whether the sensor has a shared fifo with others.

```c
typedef struct {
	float *odr; 
	float *fs; 
  int odrNum;
	BOOL isFifoSupported; 
	BOOL isCombo; 
} tQxSensorSpec;
```

- `odr` and `fs` should be initialized to point to float arrays which contain sensor's possible odrs and scale ranges. `odrNum` indicates the number of odr options. 

  Below is an example of odr and scale range arrays to sensor *LSM6DSO*. You should check the specific sensor data sheet and define them accordingly for your sensor.

```
static float lsm6dso_acc_odr[] = {12.5f,26.0f,52.0f,104.0f,208.0f,416.0f,833.0f,1666.0f,3332.0f,6664.0f};
static float lsm6dso_acc_fs[] = {2.0f,4.0f,8.0f,16.0f};
```

- `isCombo` is set to **TRUE** if the specific sensor shares fifo with other sensor. In this case, a `read_pre`  function is usually required to read data from combined fifo first, then seperate the data to corresponding sensor buffer.
- `ifFifoSupported` is set to **TRUE** only if the sensor supports a fifo and the fifo should be enabled in your application.

#####   Structure_3: `tQxSensorOps` include function points to one specific sensor operations. 

```c
typedef struct {
	tQxStatus (*init)(struct QxSensorDevice_t *dev); 
	tQxStatus (*enable)(struct QxSensorDevice_t *dev, float fs, float odr, BOOL fifo); 
	tQxStatus (*read_pre)(struct QxSensorDevice_t *dev); 
	tQxStatus (*read_over)(struct QxSensorDevice_t *dev);
	void* (*read)(struct QxSensorDevice_t *dev, uint16_t* data_bits, uint16_t* data_len ); 
	tQxStatus (*disable)(struct QxSensorDevice_t *dev); 
} tQxSensorOps;
```

- `*init` point to function that initialize a specific sensor.
- `*enable` point to function that config sensor's odr and scale rage, enable a fifo as required and enable the sensor to start working.
- `*read` point to function that read data from sensor data registers and copy them to predefined buffer, from which Framework and Apps layers will get the data later.
- `*read_pre` point to function that read data from a combination fifo first, then sperate the data to correspoding sensor before `*read` function can get them.

>   Some of the operations like `*read_over`, ` *read_pre`, `*disable`  may be only useful if your sensor has a special requirement for combined fifo read, power management and read recover management. Leave them as NULL if you don't need them.

  All the functions which those structure members point to should be implemented according to your sensor's driver. Due to the nature of a porting job, it's not possible to give a clear guide about how to implement those functions on your platform. However, you can check the example we give in file `QxSensorHal_XXX.c`  as a reference.

  With all the above 3 sturctures clearly defined for supported sensors, you will get a well described sensor list array for your device board, which will be returned from HAL to upper layer via function `QxSensorHal_GetSensorList` .

  Here is an example extracted from ST board *SensorTile.box*:

```c
static tQxSensorDevice gSensorListOfDevice[] = {
		{
				"AUDIO_MICROPHONE", 
				QXSENSOR_TYPE_MICROPHONE,
				{audio_microphone_odr, audio_microphone_fs, FALSE, FALSE},
				{
					.init = audio_microphone_init,
					.enable = audio_microphone_enable,
					.read_pre = NULL,
					.read = audio_microphone_read,
				}
		},	
		{
				"LSM6DSO_ACC", 
				QXSENSOR_TYPE_ACCEL,
				{lsm6dso_acc_odr, lsm6dso_acc_fs, TRUE, TRUE},
				{
					.init = lsm6dso_acc_init,
					.enable = lsm6dso_acc_enable,
					.read_pre = lsm6dso_read_preprocess,
					.read = lsm6dso_acc_read,
				}
		},
		......
	}
```

  Up to now, we've almost finished sensor HAL implementation. You might have a question about where should we copy the sensor data to when we read data out from each sensor's register or fifo. The typical method in our SDK code is to define a static structure `tSensorDeviceData` to include all the buffers pre-allocated for each sensor.

##### Structure_4: `tSensorDeviceData` include static buffers predefined for each sensor. 

  Below is an example extracted from the HAL implmentation for *SensorTile.box*

```c
typedef struct {
	struct {
		BOOL acc_enabled;
		BOOL gyro_enabled;
		uint8_t acc_buffer[ACC_BUFF_MAX];
		uint8_t gyro_buffer[GYRO_BUFF_MAX];
...
	} lsm6dso;
	struct {
		BOOL mag_enabled;
		uint8_t mag_buffer[MAG_BUFF_MAX];
	} lis2mdl;
......
} tSensorDeviceData *tSensorDeviceDataPtr;
```

  For each sensor's `read` operation function, those `uint8_t` arrays like `acc_buffer[ACC_BUFF_MAX]`, `gyro_buffer[GYRO_BUFF_MAX]` are the desitinations to which sensor data should copy in HAL layer. And the address of those arrays will be returned to callers of `read` function, Framework or Apps, for application use. 

  The length of those arrays are suggested to be defined big enough to buffer at least 10 milliseconds' sensor data. Below example shows buffer length for accel and gyro sensor for which max odr is 6667hz and magnetometer sensor for which max odr is 100hz. 

( As accel, gyro, and magneto sensor all have 3 axes for one individual sample, and 2 bytes for each axis in one sample, the length of buffer should equale to sample count x 3 x 2 )

```c
#define ACC_BUFF_MAX (70*6) // max 6667 samples/sec x 0.01sec = 67samples + 3samples tolerrance @10ms
#define GYRO_BUFF_MAX (70*6) // max 6667 samples/sec x 0.01sec = 67samples + 3samples tolerrance @10ms
#define MAG_BUFF_MAX (1*6) // max 100 samples/sec x 0.01sec = 1 samples
```

#### 2. `QxUSBHal_XXX.c`

  Rename the file to adapt your board platform(e.g. `QxUSBHal_RA6M3_ML.c`). 

  Qeexo AutoML platform runs data collection locally on a PC, which enumerates USB connection of device board as a virtual serial port through USB CDC driver, therefore the device must has implemented USB CDC driver as a prerequisite. `QxUSBHal_XXX.c` only encapsulates basic USB CDC operations for data transmission.

  The basic functions which need implemented are `QxUSBHal_Initialize`,  `QxUSBHal_Connected`,  `QxUSBHal_Write` and `QxUSBHal_Flush` . Whether to implement `QxUSBHal_Read_Request` depeneds on your USB CDC driver strategy.

#####   Function_1: `QxUSBHal_Initialize`

  You can enable USB power supply, register USB CDC device and configure interfaces here. Some MCU platform may has its own initialization routine for USB. Don't worry, just follow your routine and make sure the initialization finishes before `QxUSBHal_Initialize` is called in Embedded SDK application.

  We recommend to create a mutex in `QxUSBHal_Initialize` to protect USB data transfer to make sure data could be sent and received correctly especially when there is no such protection lower in driver code.

#####   Function_2: `QxUSBHal_Connected`

  This function checks whether data transfer tunnel between USB host and device has been established and return **TRUE** or **FALSE** to indicate the connection status.

#####   Function_3: `QxUSBHal_Write`

  Embedded SDK application calls this function to send data out from device to USB host, laptop. For device which implementes CDC TX transfer with large buffer supported, you can buffer the data first, and then trigger to call `QxUSBHal_Flush` when the buffer is full. 

#####   Function_4: `QxUSBHal_Flush`

  If USB CDC TX transfer is implemented with large buffer supported, you can implement this function to flush the data in the TX buffer at one time, which could be a more efficient way. However, if your device doesn't support such buffer for USB transmission, just send out data in `QxUSBHal_Write` and leave this function blank.

#####   Function_5: `QxUSBHal_Read_Request`

  Some platform's USB driver implementation may require send a USB read request before device is ready to receive data from host. Encapsulates the sending request in this function.

#### 3. `QxOS_XXX.c`

  Rename the file to adapt your platform first.

  Qeexo AutoML Embedded applications are multi-threads driven applications, so there are several OS dependent APIs in this file need to be implemented according to board's specific operating system. 

  Below is a list for APIs which must be implemented:

- `QxOS_CreateThread`

- `QxOS_CreateMutex`

- `QxOS_LockMutex`

- `QxOS_UnLockMutex`

- `QxOS_Delay`

- `QxOS_GetTick`

- `QxOS_DebugPrint`

- `QxOS_InitializeBSP`

   Just as their names show, most of those APIs should be an simple interface to call your OS correspondent functions. 

  Function `QxOS_DebugPrint` is to print out debug message on the borad. Some MCU platform may support an individual port for debug purpose, for those platform, target the debug message through the specified debug port. Otherwise , you can use USB serial port to print debug message. 

  Here is an example for the implementation of `QxOS_DebugPrint` on Renesas board *Ra6m3*:

```c
#define LOG_BUFFER 125
static char gLogBuffer[LOG_BUFFER];
void QxOS_DebugPrint(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    vsnprintf(gLogBuffer, LOG_BUFFER, format, ap);
    va_end(ap);
#ifdef RTT_DEBUG_ENABLED    
    SEGGER_RTT_printf(0, "%s\n", gLogBuffer);
#else    
    QxUSBHal_Write(gLogBuffer, strlen(gLogBuffer), 10);
#endif
}
```

  Function `QxOS_InitializeBSP` is designed to call `QxUSBHal_Initialize` and `QxBTHal_Initialize` inside. You can add your neccessary BSP boot up sequence here or make sure other necessary boot up is finished before `QxOS_InitializeBSP` .

  Here is an example for the implementation of `QxOS_InitializeBSP` on *Arduino Nano 33BLE*

```c
tQxStatus QxOS_InitializeBSP()
{
    tQxStatus res = QxOK;
    QxUSBHal_Initialize();
#ifdef QXO_BUILD_BLE
    if (g_EnableBLE) {
        QxBTHal_Initialize();
    }
#endif
    return Mbed_InitHW();
}
```

#### 4. `QxBTHal_XXX.c`

  If the device board has a BLE module, you can implement the APIs in this file for wireless communication between AutoML platform and device board. BLE transfer may be not appropriate for data collection so far as there is usually a large mount of data transfering during data collection. However `QxBTHal_Write` could still be called to transfer inference results from device to host, which makes your inference application on device more flexible.

>   Please go to [QeexoAutoML_Embedded_SDK_HAL_APIs](https://github.com/qeexo/Embedded_HAL_SDK/blob/master/Docs/QeexoAutoML_Embedded_SDK_HAL_APIs.pdf) for a complete HAL API reference list.

### Step 2. Ready to verify your HAL implementation?  Try the HAL test APIs!

  Now you've already implemented all the necessary HAL APIs to run Qeexo AutoML Embedded application on your device. But before we go further, it's recommended to use the HAL test APIs in SDK release package to verify your implementation.

  Add all the implmented HAL files under folder `Hal_impl` and header files folder `Include`  to the build directories of your IDE project. If you are using Make to compile your firmware program, add the new files into your Makefile.

  Copy the files under SDK release pacakge `/Test/hal_api_test.c` and `/Test/hal_api_test.h` to your project and add them to your build directory.

  Call the function `QxTestMain` in the `main` function of your firmware program, make sure BSP boot up is finished and `QxOS_InitializeBSP` is called before `QxTestMain`. Compile the test firmware and flash it to your device to have a test.

  Please go to ***[QeexoAutoML_HAL_Implementation_TestGuide](https://github.com/qeexo/Embedded_HAL_SDK/blob/master/Docs/QeexoAutoML_HAL_Implementation_TestGuide.pdf)*** and refer to ***Test flow for calling HAL Test APIs***  section for specific test cases and expected test results.

  Once basic USB and Sensor HAL tests pass, we are ok to go to the next step.

### Step 3. Build a data logger application and check sensor signals

  Let's move on to build a Qeexo AutoML data logger application on your device.

  Add all the source files under `Apps`, `Framework`, `Hal_Impl`(your porting version) and `Examples` to your IDE's build directory. The function "`main`" in `/Examples/datalog/src/main.cpp` should be the entry of your firmware program. It doesn't matter If your program generates its own entry and has board boot up sequence there, just rename the `main` in  `/Examples/datalog/src/main.cpp` and make sure it is called after the board boot up sequence.

  Compile the data logger firmware and flash it to your device to have a test. You can use the python script we provide under `/Test` folder to check the real time signals of the supported sensors and even plotting the signals using this data logger application.

  Please go to ***[QeexoAutoML_HAL_Implementation_TestGuide](https://github.com/qeexo/Embedded_HAL_SDK/blob/master/Docs/QeexoAutoML_HAL_Implementation_TestGuide.pdf)*** and refer to ***Check Signals*** section for specific test cases and expected test results.

### Step 4. Provide command line method for compiling and flashing

  Qeexo AutoML platform is a "one-click" solution for final users. Once the new device board is integrated into the platform, user will be free to build data collection, machine learning model applications, and try those applications on their device as they like.

  Qeexo AutoML platform manages to do this by calling the compiling firmware and flashing to device via command line way. Therefore, please provide the command line way for compiling and flashing operation. 

>   For example, Renesas device *Ra6m3* use GCC+Make as the command line compiling tool, and the command is like:
>       make GCC_PATH=/Library/Application\ Support/AutoML/EmbeddedML/toolchain/bin
>
>   *Ra6m3* uses XMODEM with bootloader and a small python script to flash image as the command line way, the command is like below:
>       python ~/src/serial_Read.py ~/Desktop/SENSING1.bin  





  Congratulations ! Now all the Embedded SDK porting job has finished. Please deliver your customized SDK firmware project to Qeexo and you will see your device appear on AutoML platform very soon!





