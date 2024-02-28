#ifndef WIFI_MAN_H
#define WIFI_MAN_H

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include "esp_log.h"

#ifdef __cplusplus
extern "C" {
#endif

void time_sync_notification_cb(struct timeval *tv);
void sntp_sync_time(struct timeval *tv);
void obtain_time(void);
void print_servers(void);
void wifi_main(void);

#ifdef __cplusplus
}
#endif

#endif