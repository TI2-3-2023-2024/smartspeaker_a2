#ifndef WIFI_MAN_H
#define WIFI_MAN_H

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
void wifi_init(void);
void obtain_time(void);
void print_servers(void);
void obtain_time_now(void);
struct DateTime get_time(void);
void wifi_main(void);

#ifdef __cplusplus
}
#endif

#endif