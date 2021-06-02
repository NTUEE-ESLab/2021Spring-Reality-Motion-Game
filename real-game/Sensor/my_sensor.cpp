#include "my_sensor.h"
#include "Events.h"
#include "PinNames.h"
#include <cstdint>
#include <cstdio>
#include <iterator>
#include <string>

DataSensor::DataSensor(EventQueue &event_queue) : 
    _event_queue(event_queue), _buffer_p(0), _sample_num(0),
    AccOffset(), GyroOffset(),  pDataXYZ(), pGyroDataXYZ(),
    pGyroDataXYZ_prev(), angle(), buffer_stm(), buffer_stm_x(),
    buffer_stm_y(), buffer_stm_z(), button(USER_BUTTON), led(LED1), high_flag_start(0), high_flag_end(0), ret_type(0), motion_type()
{
    BSP_TSENSOR_Init();
    BSP_HSENSOR_Init();
    BSP_PSENSOR_Init();

    BSP_MAGNETO_Init();
    BSP_GYRO_Init();
    BSP_ACCELERO_Init();

    ret_sen = new char[1024];
    ret_std = new char[1024];
}

void DataSensor::start() {
    // The 'rise' handler will execute in IRQ context 
    button.rise(callback(this, &DataSensor::button_released));
    // The 'fall' handler will execute in the context of thread 't' 
    button.fall(callback(this, &DataSensor::button_pressed));

    // first time calibration
    calibration();

    // add update event to event queue
    _event_queue.call_every(SENSOR_TIMESTEP, this, &DataSensor::sensorUpdateHandler);
    _event_queue.call_every(STD_TIMESTEP, this, &DataSensor::stdUpdateHandler);
}



void DataSensor::calibration() {
    printf("Starting calibration... ");

    emptyCalibrationArrays();

    emptyBufferArrays();

    collectSamples();

    normalizeSamples();

    printf("done!\n");
}

char* DataSensor::printSensorValue() {
    int n = sprintf(ret_sen, "sen{\"ax\":%d,\"ay\":%d,\"az\":%d,\"gx\":%.2f,\"gy\":%.2f,\"gz\":%.2f}", pDataXYZ[0], pDataXYZ[1], pDataXYZ[2], pGyroDataXYZ[0]/1000, pGyroDataXYZ[1]/1000, pGyroDataXYZ[2]/1000);
    return ret_sen;
}

char* DataSensor::printStd() {
    int n = sprintf(ret_std, "std{\"ax\":%.2f,\"ay\":%.2f,\"az\":%.2f,\"all\":%.2f,\"ang0\":%.0f,\"ang1\":%.0f,\"ang2\":%.0f}", stm_x, stm_y, stm_z, stm_val, angle[0], angle[1], angle[2]);
    return ret_std;
}

char* DataSensor::getSensorValueWifi() {
    int n = sprintf(ret_sen, "{\"ax\":%d,\"ay\":%d,\"az\":%d,\"gx\":%.2f,\"gy\":%.2f,\"gz\":%.2f}", pDataXYZ[0], pDataXYZ[1], pDataXYZ[2], pGyroDataXYZ[0]/1000, pGyroDataXYZ[1]/1000, pGyroDataXYZ[2]/1000);
    return ret_sen;
}

char* DataSensor::getStdWifi() {
    int n = sprintf(ret_std, "{\"ax\":%.2f,\"ay\":%.2f,\"az\":%.2f,\"all\":%.2f %.2f,\"diff\":%.2f,\"ang0\":%.0f,\"ang1\":%.0f,\"ang2\":%.0f}", stm_x, stm_y, stm_z, stm_val, stm_all, stm_diff, angle[0], angle[1], angle[2]);
    return ret_std;
}

char* DataSensor::getSensorType() {
    calculateMotion();
    if (_stand == 1) { printf("Stand\t"); }
    else if (_walk == 1) { printf("Walk\t"); }
    else if (_run == 1) { printf("Run\t"); }
    else if (_raise == 1) { printf("Raise\t"); }
    else if (_punch == 1) { printf("Punch\t"); }
    else if (_twist == 1) { printf("Twist\t"); }
    return ret_type;
}

void DataSensor::calculateMotion() {
    _walk = 0;
    _stand = 0;
    _run = 0;
    _raise = 0;
    _punch = 0;
    _twist = 0;
    // if (stm_ang1 > 38) {
    //     _twist = 1;
    //     return;
    // }
    if (stm_diff < 100 && stm_all < 500) {
        _stand = 1;
        high_flag = 0;
        return;
    }

    // get the time period at diff > 1000
    if (stm_diff > 1000) {
        if (high_flag_start == 0) {
            high_flag_start = _buffer_p;
            high_flag_end = _buffer_p;
        }
        else {
            high_flag_end += 1;
        }
    }

    if (stm_all > 1500 && stm_diff < 1000) {
        _raise = 1;
        high_flag = 1;
        // high_flag_start = 0;
        // high_flag_end = 0;
        return;
    }

    if (stm_diff < 1000 && stm_all > 1000 && high_flag != 1) {
        if (stm_z < 350) {
            _punch = 1;
            // high_flag_start = 0;
            // high_flag_end = 0;
            return;
        }
    }


    if (stm_diff > 1000) {
        if (high_flag_end - high_flag_start > 5) {
            _run = 1;
            return;
        }
    }
    
    if (stm_diff > 100) {
        _walk = 1;
        // high_flag_start = 0;
        // high_flag_end = 0;
    } else {
        _stand = 1;
        high_flag = 0;
        // high_flag_start = 0;
        // high_flag_end = 0;
    }
    return;
}

uint8_t* DataSensor::getSensorTypeBLE() {
    updateMotionType();
    return motion_type;
}

void DataSensor::updateMotionType() {
    motion_type[0] = 0;
    motion_type[1] = 1;
    motion_type[2] = 0;
    motion_type[3] = 1;
}

void DataSensor::updateStmStd() {
    stm_x = getStd(buffer_stm_x);
    stm_y = getStd(buffer_stm_y);
    stm_z = getStd(buffer_stm_z);
    stm_val = getStd(buffer_stm);
    // stm_ang0 = getStd(buffer_ang0);
    // stm_ang1 = getStd(buffer_ang1);
    // stm_ang2 = getStd(buffer_ang2);
}

void DataSensor::fillBLEArr() {
    bleArr[0] = stm_x;
    bleArr[1] = stm_y;
    bleArr[2] = stm_z;
    bleArr[3] = stm_val;
    bleArr[4] = angle[0];
    bleArr[5] = angle[1];
    bleArr[6] = angle[2];
}

void DataSensor::emptyCalibrationArrays() {
    for (int i = 0; i < 3; i++) {
        GyroOffset[i] = 0; 
        AccOffset[i] = 0;
        pDataXYZ[i] = 0;
        pGyroDataXYZ[i] = 0;
        pGyroDataXYZ_prev[i] = 0;
        angle[i] = 0;
    }
}

void DataSensor::incrementSampling() {
    BSP_GYRO_GetXYZ(pGyroDataXYZ);
    BSP_ACCELERO_AccGetXYZ(pDataXYZ);

    for (int i = 0; i < 3; i++) {
        GyroOffset[i] += pGyroDataXYZ[i];
        AccOffset[i] += pDataXYZ[i];
    }
}

void DataSensor::collectSamples() {
    _sample_num = 0;

    while (_sample_num < 1000) {
        _sample_num++;
        
        incrementSampling();

        ThisThread::sleep_for(0.5);
    }
}

void DataSensor::normalizeSamples() {
    for (int i = 0; i < 3; i++) {
        GyroOffset[i] /= _sample_num;
        AccOffset[i] /= _sample_num;
    }

    _sample_num = 0;
}

void DataSensor::emptyBufferArrays() {
    for (int i = 0; i < SENSOR_BUFFER_SIZE; i++) {
        buffer_stm_x[i] = 0;
        buffer_stm_y[i] = 0;
        buffer_stm_z[i] = 0;
        buffer_stm[i] = 0;
    }
}

float DataSensor::getSum(int* buffer) {
    float sum = 0;
    for (int i = 0; i < SENSOR_BUFFER_SIZE; i++) {
        sum += buffer[i];
    }

    return sum;
}

float DataSensor::getAvg(int sum) {
    return sum / SENSOR_BUFFER_SIZE;
}

float DataSensor::getAvg(int* buffer) {
    return getSum(buffer) / SENSOR_BUFFER_SIZE;
}

float DataSensor::getVar(int* buffer) {
    float sum = 0, mean = 0, var = 0;

    sum = getSum(buffer);
    mean = getAvg(sum);

    for (int i = 0; i < SENSOR_BUFFER_SIZE; i++) {
        var += pow(buffer[i] - mean, 2);
    }

    return var;
}

float DataSensor::getStd(float var) {
    return sqrt(var / SENSOR_BUFFER_SIZE);
}

float DataSensor::getStd(int* buffer) {
    return sqrt(getVar(buffer) / SENSOR_BUFFER_SIZE);
}

void DataSensor::sampling() {
    BSP_ACCELERO_AccGetXYZ(pDataXYZ);
    BSP_GYRO_GetXYZ(pGyroDataXYZ);

    for (int i = 0; i < 3; ++i) {
        pDataXYZ[i] = pDataXYZ[i] - AccOffset[i];
        pGyroDataXYZ[i] = pGyroDataXYZ[i] - GyroOffset[i];
    }
}

float DataSensor::square(float data) {
    return pow(data, 2);
}

float DataSensor::square_pData() {
    return square((float)pDataXYZ[0]) + square((float)pDataXYZ[1]) + square((float)pDataXYZ[2]);
}

float DataSensor::square_diffData() {
    float n = square((float)buffer_stm_x[_buffer_p]) + square((float)buffer_stm_y[_buffer_p]) + square((float)buffer_stm_z[_buffer_p]);
    return sqrt(n);
}

float DataSensor::getSqrtMean_pData() {
    return sqrt(square_pData());
}

void DataSensor::calculateAngle() {
    // relative directions
    int prev_ang0 = angle[0];
    int prev_ang1 = angle[1];
    int prev_ang2 = angle[2];
    for (int i = 0; i < 3; i++) {
        if (abs(pGyroDataXYZ[i]) * SCALE_MULTIPLIER > 100) {
            angle[i] += (pGyroDataXYZ[i] + pGyroDataXYZ_prev[i]) / 2 * TIMESTEP * SCALE_MULTIPLIER * 0.0001;
        }
        pGyroDataXYZ_prev[i] = pGyroDataXYZ[i];
    }
    buffer_ang0[_buffer_p] = abs(angle[0] - prev_ang0);
    buffer_ang1[_buffer_p] = abs(angle[1] - prev_ang1);
    buffer_ang2[_buffer_p] = abs(angle[2] - prev_ang2);
}

void DataSensor::update() {
    sampling();

    buffer_stm_x[_buffer_p] = (float)(pDataXYZ[0]-prev_stm_x);
    buffer_stm_y[_buffer_p] = (float)(pDataXYZ[1]-prev_stm_y);
    buffer_stm_z[_buffer_p] = (float)(pDataXYZ[2]-prev_stm_z);

    prev_stm_x = pDataXYZ[0];
    prev_stm_y = pDataXYZ[1];
    prev_stm_z = pDataXYZ[2];

    // Not sure
    // ThisThread::sleep_for(TIMESTEP);

    calculateAngle();

    buffer_stm[_buffer_p] = getSqrtMean_pData();
    stm_diff = square_diffData();
    stm_all = buffer_stm[_buffer_p];

    // Move pointer position
    _buffer_p = (_buffer_p + 1) % SENSOR_BUFFER_SIZE;
}

void DataSensor::sensorUpdateHandler() {
    update();
}

void DataSensor::stdUpdateHandler() {
    char* xyz_sen = NULL;
    char* xyz_std = NULL;
    char* xyz = NULL;

    updateStmStd();
    // xyz_sen = printSensorValue();
    xyz_std = printStd();
    // nsapi_size_t size = strlen(xyz_stm);

    // printf("%s\n", xyz_sen);
    //printf("%s\n", xyz_std);
}

void DataSensor::button_release_detecting() {
    button.enable_irq();
}

void DataSensor::button_pressed() {
    button.disable_irq();
    _event_queue.call(printf, "pressed\n");
    press_threhold.attach(callback(this, &DataSensor::button_release_detecting), 2.0);
    _event_queue.call(printf, "start timer...\n");
}

void DataSensor::button_released() {
    led = !led;
    _event_queue.call(printf, "released\n");
    _event_queue.call(this, &DataSensor::calibration);
}