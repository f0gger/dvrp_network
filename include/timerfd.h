#ifndef __TIMERFD_H
#define __TIMERFD_H

typedef struct
{
    int timer_fd;
    unsigned long long wakeups_missed;
} periodic_info;

int make_periodic(unsigned int period, periodic_info *info, int insta);
void wait_period(periodic_info *info);

#endif
