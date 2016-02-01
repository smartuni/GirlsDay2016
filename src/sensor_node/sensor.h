/**
 * @ingroup     smartuni
 * @{
 *
 * @file
 * @brief       Defines sensor stuff
 *
 * @author      smlng <s@mlng.net>
 *
 */

#ifndef SENSOR_H_
#define SENSOR_H_

#define SENSOR_MSG_QUEUE_SIZE   (10U)
#define SENSOR_TIMEOUT_MS       (2*1000*1000) // 2s
#define SENSOR_NUM_SAMPLES      (10U)

int sensor_get_humidity(void);
int sensor_get_temperature(void);
int sensor_start_thread(void);

#endif // SENSOR_H_
/** @} */
