#ifndef TIME_MAN_H
#define TIME_MAN_H

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include "esp_log.h"
#include "datetime.h"

#ifdef __cplusplus
extern "C" {
#endif

void sntp_sync_time(struct timeval *tv);
void time_sync_notification_cb(struct timeval *tv);
void time_init(void);
void obtain_time(void);
void print_servers(void);
struct DateTime get_time(void);
#ifdef __cplusplus
}
#endif

#endif