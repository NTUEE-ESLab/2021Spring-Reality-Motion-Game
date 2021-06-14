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
    buffer_stm_y(), buffer_stm_z(), button(USER_BUTTON), led(LED1), high_flag_start(0), high_flag_end(0), motion_type(),
    _motion_buffer_p(0), motion_buffer(), motion_type_wifi(0), twist_flag(0), twist_time(0), raise_time(0)
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
    _event_queue.call_every(TIMESTEP, this, &DataSensor::sensorUpdateHandler);
    _event_queue.call_every(STD_TIMESTEP, this, &DataSensor::stdUpdateHandler);
    // _event_queue.call_every(CALIBRATION_CYCLE, this, &DataSensor::recalibrate);
}



void DataSensor::calibration() {
    printf("Starting calibration... ");

    emptyCalibrationArrays();

    emptyBufferArrays();

    collectSamples();

    normalizeSamples();

    printf("done!\n");
}

void DataSensor::recalibrate() {
    if (getSensorType() == 0) {
        calibration();
    }
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
    int n = sprintf(ret_std, "{\"ax\":%.2f,\"ay\":%.2f,\"az\":%.2f,\"val\":%.2f,\"all\":%.2f,\"diff\":%.2f,\"ang0\":%.2f %.2f,\"ang1\":%.2f %.2f,\"ang2\":%.2f %.2f}", stm_x, stm_y, stm_z, stm_val, stm_all, stm_diff, stm_ang0, angle[0], stm_ang1, angle[1], stm_ang2, angle[2]);
    return ret_std;
}

int DataSensor::getSensorType() {

    int current_p = _motion_buffer_p == 0 ? MOTION_BUFFER_SIZE - 1 : _motion_buffer_p - 1;
    int prev_p = current_p == 0 ? MOTION_BUFFER_SIZE - 1 : current_p - 1;
    int prev_prev_p = prev_p == 0 ? MOTION_BUFFER_SIZE - 1 : prev_p - 1;

    if (motion_buffer[current_p] == motion_buffer[prev_p]) {
        motion_type_wifi = motion_buffer[current_p];
    }
    else if (motion_buffer[current_p] == motion_buffer[prev_prev_p]) {
        motion_type_wifi = motion_buffer[current_p];
    }

    if (motion_type_wifi == 0) { printf("Stand\t"); }
    else if (motion_type_wifi == 1) { printf("Walk\t"); }
    else if (motion_type_wifi == 2) { printf("Run\t"); }
    else if (motion_type_wifi == 3) { printf("Raise\t"); }
    else if (motion_type_wifi == 4) { printf("Punch\t"); }
    else if (motion_type_wifi == 5) { printf("Right\t"); }
    else if (motion_type_wifi == 6) { printf("Left\t"); }
    return motion_type_wifi;
}

void DataSensor::calculateMotion() {
    // _stand  0
    // _walk   1
    // _run    2
    // _raise  3
    // _punch  4
    // _right  5
    // _left   6
    
    if (stm_diff < 100 && stm_all < 500) {
        raise_time = 0;
        high_flag = 0;
        motion_buffer[_motion_buffer_p] = 0;
        return;
    }

    if (stm_diff > 1000) {
        if (high_flag_start == 0) {
            high_flag_start = _buffer_p;
            high_flag_end = _buffer_p;
        }
        else {
            high_flag_end += 1;
        }
    }

    if ((stm_diff < 1000 && stm_z > 300 && stm_all > 1800 )) {
        raise_time += 1;
        // if (raise_time < 10) {
            high_flag = 1;
            motion_buffer[_motion_buffer_p] = 3;
            return;
        // }
    }

    if (stm_diff > 1500) {
        if (high_flag_end - high_flag_start > 5) {
            motion_buffer[_motion_buffer_p] = 2;
            return;
        }
    }

    if (stm_ang1 > 4.5 && (twist_flag != 1 || angle[1] > 0) ) { 
        twist_time += 1;   
        if (twist_flag == 1) {
            motion_buffer[_motion_buffer_p] = 5;
            return;
        }
        else if (twist_flag == 2) {
            motion_buffer[_motion_buffer_p] = 6;
            return;
        }
        if (angle[1] > 0) {
            motion_buffer[_motion_buffer_p] = 5;
            twist_flag = 1;
        }
        else {
            motion_buffer[_motion_buffer_p] = 6;
            twist_flag = 2;
        }
        return;
    } 
    
    if (stm_ang1 < 3.9) {
        if (twist_flag != 0) {
            twist_flag = 0;
            twist_time = 0;
        }
    }
    

    // get the time period at diff > 1000

    if (stm_diff < 1000 && stm_y > 400 && high_flag != 1) {
        if (stm_z < 350) {
            motion_buffer[_motion_buffer_p] = 4;
            return;
        }
    }
    
    if (stm_diff > 100) {
        if (raise_time > 20) {
            raise_time = 0;
        }
        motion_buffer[_motion_buffer_p] = 1;
    } else {
        raise_time = 0;
        high_flag = 0;
        motion_buffer[_motion_buffer_p] = 0;
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
    stm_ang0 = getStd(buffer_ang0);
    stm_ang1 = getStd(buffer_ang1);
    stm_ang2 = getStd(buffer_ang2);
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
        pGyroDataXYZ[i] = (pGyroDataXYZ[i] - GyroOffset[i]) * SCALE_MULTIPLIER;
    }
}

float DataSensor::square(float data) {
    return pow(data, 2);
}

float DataSensor::square_pData() {
    return square((float)pDataXYZ[0]) + square((float)pDataXYZ[1]) + square((float)pDataXYZ[2]);
}

float DataSensor::square_diffData() {
    float n = square((float)(pDataXYZ[0]-prev_buffer_x))+square((float)(pDataXYZ[1]-prev_buffer_y))+square((float)(pDataXYZ[2]-prev_buffer_z));
    // float n = square((float)buffer_stm_x[_buffer_p]) + square((float)buffer_stm_y[_buffer_p]) + square((float)buffer_stm_z[_buffer_p]);
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
        // if (abs(pGyroDataXYZ[i]) * SCALE_MULTIPLIER > 100) {
        angle[i] += (pGyroDataXYZ[i] + pGyroDataXYZ_prev[i]) / 2 * STD_TIMESTEP * SCALE_MULTIPLIER * 0.001;
        // }
        pGyroDataXYZ_prev[i] = pGyroDataXYZ[i];
    }
    buffer_ang0[_buffer_p] = abs(angle[0] - prev_ang0);
    buffer_ang1[_buffer_p] = abs(angle[1] - prev_ang1);
    buffer_ang2[_buffer_p] = abs(angle[2] - prev_ang2);
    // printf("angle x:%.0f, y:%.0f, z:%.0f\t%.2f, %.2f, %.2f\t", pGyroDataXYZ[0]*SCALE_MULTIPLIER, pGyroDataXYZ[1]*SCALE_MULTIPLIER, pGyroDataXYZ[2]*SCALE_MULTIPLIER, angle[0], angle[1], angle[2]);
}

void DataSensor::update() {
    sampling();

    buffer_stm_x[_buffer_p] = (float)(pDataXYZ[0]-prev_buffer_x);
    buffer_stm_y[_buffer_p] = (float)(pDataXYZ[1]-prev_buffer_y);
    buffer_stm_z[_buffer_p] = (float)(pDataXYZ[2]-prev_buffer_z);

    calculateAngle();

    buffer_stm[_buffer_p] = getSqrtMean_pData();
    stm_diff = square_diffData();
    stm_all = getSqrtMean_pData();

    prev_buffer_x = pDataXYZ[0];
    prev_buffer_y = pDataXYZ[1];
    prev_buffer_z = pDataXYZ[2];

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

    xyz_std = getStdWifi();
    // printf("%s\n", xyz_std);
    
    calculateMotion();
    _motion_buffer_p = (_motion_buffer_p + 1) % MOTION_BUFFER_SIZE;

    // xyz_sen = getSensorType();
    // printf("%s\t", xyz_sen);
    printf("%s\n", xyz_std);    // must print! (for frequency...)

    prev_stm_x = stm_x;
    prev_stm_y = stm_y;
    prev_stm_z = stm_z;
}

void DataSensor::button_release_detecting() {
    button.enable_irq();
}

void DataSensor::button_pressed() {
    button.disable_irq();
    _event_queue.call(printf, "pressed\n");
    press_threhold.attach(callback(this, &DataSensor::button_release_detecting), 1.0);
    _event_queue.call(printf, "start timer...\n");
}

void DataSensor::button_released() {
    led = !led;
    _event_queue.call(printf, "released\n");
    _event_queue.call(this, &DataSensor::calibration);
}