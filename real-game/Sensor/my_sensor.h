
#ifndef __MY_SENSOR_H
#define __MY_SENSOR_H

#include "EventQueue.h"
#include "mbed.h"

// Sensors drivers present in the BSP library
#include "stm32l475e_iot01_tsensor.h"
#include "stm32l475e_iot01_hsensor.h"
#include "stm32l475e_iot01_psensor.h"
#include "stm32l475e_iot01_magneto.h"
#include "stm32l475e_iot01_gyro.h"
#include "stm32l475e_iot01_accelero.h"

#include "mbed_events.h" 
#include "ThisThread.h"
#include "PinNames.h"
#include <cstdint>

#define SENSOR_BUFFER_SIZE 20
#define SCALE_MULTIPLIER 0.045
#define TIMESTEP 1
#define STD_TIMESTEP 20
#define MOTION_BUFFER_SIZE 10

extern EventQueue event_queue;

class DataSensor {
public:
    // Constructor
    DataSensor(EventQueue &event_queue);

    void start();

    // Calibrate the offset of the sensor
    void calibration();

    // Print the sensor data value
    char* printSensorValue();

    // Print standard value
    char* printStd();

    char* getSensorValueWifi();

    char* getStdWifi();

    int getSensorType();

    void calculateMotion();

    uint8_t* getSensorTypeBLE();


    // Update sensor data buffer
    void update();


    double bleArr[7];

    uint8_t valueBytes[8];


private:
    EventQueue &_event_queue;

    // Calibration arrays
    int AccOffset[3];          
    double GyroOffset[3];    

    // Data arrays
    // int16_t pDataXYZ[3];
    int16_t pDataXYZ[3];
    float pGyroDataXYZ[3];
    float pGyroDataXYZ_prev[3];
    double angle[3];

    // Slinding window buffers
    int buffer_stm_x[SENSOR_BUFFER_SIZE];
    int buffer_stm_y[SENSOR_BUFFER_SIZE];
    int buffer_stm_z[SENSOR_BUFFER_SIZE];
    int buffer_stm[SENSOR_BUFFER_SIZE];

    // Buffer for angles
    int buffer_ang0[SENSOR_BUFFER_SIZE];
    int buffer_ang1[SENSOR_BUFFER_SIZE];
    int buffer_ang2[SENSOR_BUFFER_SIZE];

    // Last buffer value
    double prev_buffer_x;
    double prev_buffer_y;
    double prev_buffer_z;
    double prev_stm_x;
    double prev_stm_y;
    double prev_stm_z;
    double stm_diff;
    double stm_all;

    // Std values
    double stm_x;
    double stm_y;
    double stm_z;
    double stm_val;
    double stm_ang0;
    double stm_ang1;
    double stm_ang2;

    // Buffer pointer
    int _buffer_p;

    // Sampling variables
    int _sample_num;

    // Print Buffer
    char* ret_sen;
    char* ret_std;
    uint8_t motion_type[5];
    int motion_type_wifi;

    // Record buffer high time for jump and run
    int high_flag_start;
    int high_flag_end;
    int high_flag;
    int twist_flag;

    // Motion buffers
    int motion_buffer[MOTION_BUFFER_SIZE];
    int _motion_buffer_p;

    // Button Event
    DigitalOut led;
    InterruptIn button;
    Timeout press_threhold;


    // Empty the offset arrays
    void emptyCalibrationArrays();

    // Sample data and add to pData array
    void incrementSampling();

    // Collect samples during a period of time
    void collectSamples();

    // Normalize the sample data by the numbers of sampling
    void normalizeSamples();

    void emptyBufferArrays();

    float getSum(int* buffer);

    float getAvg(int sum);

    float getAvg(int* buffer);

    float getVar(int* buffer);

    float getStd(float var);

    float getStd(int* buffer);

    void updateStmStd();

    void fillBLEArr();

    void updateMotionType();

    // Get Acce and Gyro data
    void sampling();

    // Get square value of the data
    float square(float data);

    // Get square value of the pData array
    float square_pData();

    // Get square value of diff pData
    float square_diffData();

    // Get square root mean of the pData array
    float getSqrtMean_pData();

    // Calculate the angle based on gyro data
    void calculateAngle();

    void sensorUpdateHandler();

    void stdUpdateHandler();

    void button_release_detecting();

    void button_pressed();

    void button_released();

    
};

#endif