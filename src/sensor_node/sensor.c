/**
 * written by smlng
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "board.h"
#include "thread.h"
#include "xtimer.h"
#include "hdc1000.h"
// own
#include "sensor.h"

#define SENSOR_MSG_QUEUE_SIZE   (10U)
#define SENSOR_TIMEOUT_MS       (5*1000) // 5s
#define SENSOR_NUM_SAMPLES      (10)

static hdc1000_t hdcDev;

static int samples_temperature[SENSOR_NUM_SAMPLES];
static int samples_humidity[SENSOR_NUM_SAMPLES];

static char sensor_thread_stack[THREAD_STACKSIZE_DEFAULT];
static msg_t sensor_thread_msg_queue[SENSOR_MSG_QUEUE_SIZE];

int sensor_get_temperature(void)
{
    int sum = 0;
    for (int i=0; i < SENSOR_NUM_SAMPLES; ++i) {
        sum += samples_temperature[i];
    }
    return (sum/SENSOR_NUM_SAMPLES);
}

int sensor_get_humidity(void)
{
    int sum = 0;
    for (int i=0; i < SENSOR_NUM_SAMPLES; ++i) {
        sum += samples_humidity[i];
    }
    return (sum/SENSOR_NUM_SAMPLES);
}

/**
 * @brief Measures the temperature and humitity with a HDC1000.
 *
 * @param[out] temp the measured temperature in degree celsius * 100
 * @param[out] hum the measured humitity in % * 100
 */
static void sensor_hdc1000_measure(int *temp, int *hum) {
    uint16_t rawtemp;
    uint16_t rawhum;
    if (hdc1000_startmeasure(&hdcDev)) {
        puts("[sensors] ERROR: HDC1000 starting measure failed");
        return;
    }
    // wait for the measurment to finish
    xtimer_usleep(HDC1000_CONVERSION_TIME); //26000us
    hdc1000_read(&hdcDev, &rawtemp, &rawhum);
    hdc1000_convert(rawtemp, rawhum,  temp, hum);
    printf("[sensors] INFO: HDC1000 Data T: %d   RH: %d\n", *temp, *hum);
}

/**
 * @brief Intialise all sensores.
 *
 * @return 0 on error
 */
static int sensor_init(void) {
    // initialise temperature/humidity sensor
    if (!(hdc1000_init(&hdcDev, 0, HDC1000_I2C_ADDRESS) == 0)) {
        puts("[sensors] ERROR: HDC1000 initialisation failed");
        return (-1);
    }
    int hi, ti;
    sensor_hdc1000_measure(&ti,&hi);
    for (int i=0; i <SENSOR_NUM_SAMPLES; ++i) {
        samples_humidity[i] = hi;
        samples_temperature[i] = ti;
    }
    puts("[sensors] INFO: HDC1000 initialisation success");
    return 0;
}

/**
 * @brief udp receiver thread function
 *
 * @param[in] arg   unused
 */
static void *sensor_thread(void *arg)
{
    (void) arg;
    int count = 0;
    msg_init_queue(sensor_thread_msg_queue, SENSOR_MSG_QUEUE_SIZE);
    while(1) {
        sensor_hdc1000_measure( &samples_temperature[count%SENSOR_NUM_SAMPLES],
                                &samples_humidity[count%SENSOR_NUM_SAMPLES]);
        ++count;
        xtimer_usleep(SENSOR_TIMEOUT_MS);
    }
    return NULL;
}

/**
 * @brief start udp receiver thread
 */
int sensor_start_thread(void)
{
    // init sensor
    sensor_init();
    // start thread
    return thread_create(sensor_thread_stack, sizeof(sensor_thread_stack),
                         THREAD_PRIORITY_MAIN, THREAD_CREATE_STACKTEST,
                         sensor_thread, NULL, "sensor_thread");
}
