/* 
 * A motion data sensor class.
 * 
 * This class provides functionality that collects data from the real world.
 * After collecting data, it decide the motion and then save to motion buffer.
 * When the outer WiFi class call for the motion, the sensor choose the most frequent one.
 */

#ifndef __MY_SENSOR_H
#define __MY_SENSOR_H

// Sensors drivers present in the BSP library
#include "stm32l475e_iot01_tsensor.h"
#include "stm32l475e_iot01_hsensor.h"
#include "stm32l475e_iot01_psensor.h"
#include "stm32l475e_iot01_magneto.h"
#include "stm32l475e_iot01_gyro.h"
#include "stm32l475e_iot01_accelero.h"

#include "EventQueue.h"
#include "mbed.h"
#include "mbed_events.h" 
#include "ThisThread.h"
#include "PinNames.h"
#include <cstdint>

// Sensor parameters
#define SENSOR_BUFFER_SIZE 10
#define SCALE_MULTIPLIER 0.045
#define TIMESTEP 1
#define STD_TIMESTEP 10
#define MOTION_BUFFER_SIZE 5
#define CALIBRATION_CYCLE 10s

extern EventQueue event_queue;

class DataSensor {
public:
    /* 
     * Initialized some parameters including sensor and buffer.
     */
    DataSensor(EventQueue &event_queue);

    /* 
     * Start sensing data.
     * 
     * This method registers button, sensing, and motion events for given timestep.
     * It will also do the first calibration.
     */
    void start();

    /* 
     * Get the user's motion type for outer connection service. It is of ["type":$motion] format.
     */
    int getSensorType();

private:
    EventQueue &_event_queue;

    // Calibration arrays
    int AccOffset[3];          
    double GyroOffset[3];    

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
    int motion_type_wifi;

    // Record buffer high time for jump and run
    int high_flag_start;
    int high_flag_end;
    int high_flag;
    int twist_flag;
    int twist_time;
    int raise_time;
    int right_flag;
    int left_flag;

    // Motion buffers
    int motion_buffer[MOTION_BUFFER_SIZE];
    int _motion_buffer_p;

    // LED and Button
    DigitalOut led;
    InterruptIn button;
    Timeout press_threhold;

    /* 
     * Calibrate the sensor.
     * 
     * This methods calculate the current offset of the sensor.
     */
    void calibration();

    /* 
     * Recalibrate the sensor.
     */
    void recalibrate();

    /* 
     * Print the raw data including acce and gyro.
     */
    char* printSensorValue();

    /* 
     * Print the standard value of data in the buffer including acce, all, and angle.
     */
    char* printStd();

    /* 
     * Get the json-formatted raw data string including acce and gyro.
     */
    char* getSensorValueWifi();

    /* 
     * Get the json-formatted standard value of data in the buffer including acce, all, and angle.
     */
    char* getStdWifi();

    /* 
     * Update data sensor buffer.
     */
    void update();

    /* 
     * Determine the motion type.
     */
    void calculateMotion();

    /* 
     * Empty the calibration offset arrays
     */
    void emptyCalibrationArrays();

    /* 
     * Sample data and add to pData array
     */
    void incrementSampling();

    /* 
     * Collect samples during a period of time
     */
    void collectSamples();

    /* 
     * Normalize the sample data by the numbers of sampling
     */
    void normalizeSamples();

    /* 
     * Empty the data buffer array.
     */
    void emptyBufferArrays();

    /* 
     * Get sum of data in the buffer.
     */
    float getSum(int* buffer);

    /* 
     * Get average of the sum. The denominator is set in the macro SENSOR_BUFFER_SIZE.
     */
    float getAvg(int sum);

    /* 
     * Get average of data in the buffer. The denominator is set in the macro SENSOR_BUFFER_SIZE.
     */
    float getAvg(int* buffer);

    /* 
     * Get variance of data in the buffer. The denominator is set in the macro SENSOR_BUFFER_SIZE.
     */
    float getVar(int* buffer);

    /* 
     * Get standard value of a variance. The denominator is set in the macro SENSOR_BUFFER_SIZE.
     */
    float getStd(float var);

    /* 
     * Get variance of data in the buffer. The denominator is set in the macro SENSOR_BUFFER_SIZE.
     */
    float getStd(int* buffer);

    /* 
     * Update the standard value of the buffer.
     */
    void updateStmStd();

    /* 
     * Get Acce and Gyro data
     */
    void sampling();

    /* 
     * Get square value of the data
     */
    float square(float data);

    /* 
     * Get square value of the pData array
     */
    float square_pData();

    /* 
     * Get square value of diff pData
     */
    float square_diffData();

    /* 
     * Get square root mean of the pData array
     */
    float getSqrtMean_pData();

    /* 
     * Calculate the angle based on gyro data
     */
    void calculateAngle();

    /* 
     * The handler for updating sensor buffer
     */
    void sensorUpdateHandler();

    /* 
     * The handler for updating sensor standard value
     */
    void stdUpdateHandler();

    /* 
     * Helper function to perform long press event
     */
    void button_release_detecting();

    /* 
     * The button pressed handler
     */
    void button_pressed();

    /* 
     * The button released handler
     */
    void button_released();
};

#endif