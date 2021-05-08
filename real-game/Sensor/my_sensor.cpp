#include "my_sensor.h"
#include "PinNames.h"

DataSensor::DataSensor(EventQueue &event_queue) : 
    _event_queue(event_queue), _buffer_p(0), _sample_num(0),
    AccOffset(), GyroOffset(),  pDataXYZ(), pGyroDataXYZ(),
    pGyroDataXYZ_prev(), angle(), buffer_stm(), buffer_stm_x(),
    buffer_stm_y(), buffer_stm_z(), button(USER_BUTTON), led(LED1)
{
    BSP_TSENSOR_Init();
    BSP_HSENSOR_Init();
    BSP_PSENSOR_Init();

    BSP_MAGNETO_Init();
    BSP_GYRO_Init();
    BSP_ACCELERO_Init();

    ret = new char[1024];
}

void DataSensor::start() {
    // The 'rise' handler will execute in IRQ context 
    button.rise(callback(this, &DataSensor::button_released));
    // The 'fall' handler will execute in the context of thread 't' 
    button.fall(callback(this, &DataSensor::button_pressed));
    _event_queue.call_every(10, this, &DataSensor::update_handler);
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
    int n = sprintf(ret, "sensor{\"ax\":%d,\"ay\":%d,\"az\":%d,\"gx\":%.2f,\"gy\":%.2f,\"gz\":%.2f}", pDataXYZ[0], pDataXYZ[1], pDataXYZ[2], pGyroDataXYZ[0]/1000, pGyroDataXYZ[1]/1000, pGyroDataXYZ[2]/1000);
    printf("ACC %d, %d, %d  Gyro %.2f %.2f %.2f\n", pDataXYZ[0], pDataXYZ[1], pDataXYZ[2], pGyroDataXYZ[0]/1000, pGyroDataXYZ[1]/1000, pGyroDataXYZ[2]/1000);
    return ret;
}

char* DataSensor::printStd() {
    float stm_x = getStd(buffer_stm_x);
    float stm_y = getStd(buffer_stm_y);
    float stm_z = getStd(buffer_stm_z);
    float stm_val = getStd(buffer_stm);

    int n = sprintf(ret, "{\"ax\":%.2f,\"ay\":%.2f,\"az\":%.2f,\"all\":%.2f,\"ang0\":%.0f,\"ang1\":%.0f,\"ang2\":%.0f}", stm_x, stm_y, stm_z, stm_val, angle[0], angle[1], angle[2]);
    return ret;
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

    while (_sample_num < 500) {
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

float DataSensor::getSqrtMean_pData() {
    return sqrt(square_pData());
}

void DataSensor::calculateAngle() {
    // relative directions
    for (int i = 0; i < 3; i++) {
        if (abs(pGyroDataXYZ[i]) * SCALE_MULTIPLIER > 50) {
            angle[i] += (pGyroDataXYZ[i] + pGyroDataXYZ_prev[i]) / 2 * TIMESTEP * SCALE_MULTIPLIER;
        }
        pGyroDataXYZ[i] = pGyroDataXYZ[i];
    }
}

void DataSensor::update() {
    sampling();

    buffer_stm_x[_buffer_p] = (float)pDataXYZ[0];
    buffer_stm_y[_buffer_p] = (float)pDataXYZ[1];
    buffer_stm_z[_buffer_p] = (float)pDataXYZ[2];

    // Not sure
    // ThisThread::sleep_for(TIMESTEP);

    calculateAngle();

    buffer_stm[_buffer_p] = getSqrtMean_pData();

    // Move pointer position
    _buffer_p = (_buffer_p + 1) % SENSOR_BUFFER_SIZE;
}

void DataSensor::update_handler() {
    char* xyz_sen = NULL;
    char* xyz_stm = NULL;
    char* xyz = NULL;

    update();

    xyz_stm = printStd();
    nsapi_size_t size = strlen(xyz_stm);
    xyz = xyz_stm;

    printf("%s\n", xyz);
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