
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

#define SENSOR_BUFFER_SIZE 50
#define SCALE_MULTIPLIER 0.045
#define TIMESTEP 50

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

    // Update sensor data buffer
    void update();

private:
    EventQueue &_event_queue;

    // Calibration arrays
    int AccOffset[3];          
    float GyroOffset[3];    

    // Data arrays
    int16_t pDataXYZ[3];
    float pGyroDataXYZ[3];
    float pGyroDataXYZ_prev[3];
    double angle[3];

    // Slinding window buffers
    int buffer_stm_x[SENSOR_BUFFER_SIZE];
    int buffer_stm_y[SENSOR_BUFFER_SIZE];
    int buffer_stm_z[SENSOR_BUFFER_SIZE];
    int buffer_stm[SENSOR_BUFFER_SIZE];

    // Buffer pointer
    int _buffer_p;

    // Sampling variables
    int _sample_num;

    // Print Buffer
    char* ret;

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

    // Get Acce and Gyro data
    void sampling();

    // Get square value of the data
    float square(float data);

    // Get square value of the pData array
    float square_pData();

    // Get square root mean of the pData array
    float getSqrtMean_pData();

    // Calculate the angle based on gyro data
    void calculateAngle();

    void update_handler();

    void button_release_detecting();

    void button_pressed();

    void button_released();

    
};

#endif