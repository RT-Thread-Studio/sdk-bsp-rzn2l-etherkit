# RZ EtherKit Development Board RTC and Alarm Usage Example

**中文** | [**English**](./README.md)

## Introduction

This example mainly introduces how to use the RTC (Real-Time Clock) on the EtherKit. The RTC can provide precise real-time information, including year, month, day, hour, minute, and second. Most modern RTC chips use high-precision crystal oscillators as their clock source. Some clock chips also include a battery backup to keep the time information valid during power outages.

The RT-Thread RTC device provides basic services for the operating system's time system. With the increasing number of IoT scenarios, RTC has become a standard feature in products and is even indispensable in secure transmission processes like SSL.

## Hardware Description

The RTC device used in this example relies on the LSE clock and does not require additional connections.

## Software Description

The source code for this example is located in `/projects/etherkit_basic_rtc`. In the `hal_entry()` function, the RTC device is obtained, and the system time is set once. Then, the system time is retrieved to check if it has been set successfully, followed by a delay of 1 second before retrieving the system time again.

```c
rt_err_t ret = RT_EOK;
time_t now;
rt_device_t device = RT_NULL;

device = rt_device_find(RTC_NAME);
if (!device)
{
    rt_kprintf("find %s failed!\n", RTC_NAME);
}

if(rt_device_open(device, 0) != RT_EOK)
{
    rt_kprintf("open %s failed!\n", RTC_NAME);
}

/* Set date */
ret = set_date(2024, 11, 11);
rt_kprintf("set RTC date to 2024-11-11\n");
if (ret != RT_EOK)
{
    rt_kprintf("set RTC date failed\n");
}

/* Set time */
ret = set_time(15, 00, 00);
if (ret != RT_EOK)
{
    rt_kprintf("set RTC time failed\n");
}

/* Delay 3 seconds */
rt_thread_mdelay(3000);

/* Get time */
get_timestamp(&now);
rt_kprintf("now: %.*s", 25, ctime(&now));
```

The following code creates an RTC alarm that wakes up after 1 second, and then exports this function to the MSH command line.

    void user_alarm_callback(rt_alarm_t alarm, time_t timestamp)
    {
        rt_kprintf("user alarm callback function.\n");
    }
    
    void alarm_sample(void)
    {
        rt_device_t dev = rt_device_find("rtc");
        struct rt_alarm_setup setup;
        struct rt_alarm * alarm = RT_NULL;
        static time_t now;
        struct tm p_tm;
    if (alarm != RT_NULL)
        return;
    /* Get current timestamp and set the alarm for the next second */
    now = get_timestamp(NULL) + 1;
    gmtime_r(&now, &p_tm);
    
    setup.flag = RT_ALARM_SECOND;
    setup.wktime.tm_year = p_tm.tm_year;
    setup.wktime.tm_mon = p_tm.tm_mon;
    setup.wktime.tm_mday = p_tm.tm_mday;
    setup.wktime.tm_wday = p_tm.tm_wday;
    setup.wktime.tm_hour = p_tm.tm_hour;
    setup.wktime.tm_min = p_tm.tm_min;
    setup.wktime.tm_sec = p_tm.tm_sec;
    
    alarm = rt_alarm_create(user_alarm_callback, &setup);
    if (RT_NULL != alarm)
    {
        rt_alarm_start(alarm);
    }
    }
    /* Export MSH command */
    MSH_CMD_EXPORT(alarm_sample, alarm sample);

## Running

### Compilation & Download

**RT-Thread Studio**: Download the EtherKit resource package in the RT-Thread Studio package manager, then create a new project and compile it.

**IAR**: First, double-click `mklinks.bat` to generate links for the rt-thread and libraries folders; then use Env to generate the IAR project; finally, double-click `project.eww` to open the IAR project and compile it.

After compilation, connect the Jlink interface of the development board to the PC, and download the firmware to the development board.

### Running Effects

After pressing the reset button to restart the development board, you can see the following printed information on the board:

 \ | /

\- RT -   Thread Operating System

 / | \   5.1.0 build Nov 13 2024 13:35:43

 2006 - 2024 Copyright by RT-Thread team

 

Hello RT-Thread!

==================================================

This example project is an rtc and alarm routine!

==================================================

set RTC date to 2024-11-11

msh >now: Sat Nov 19 07:42:42 3385

msh >alarm_sample

user alarm callback function.

user alarm callback function.

user alarm callback function.

user alarm callback function.

## Notes

None

## References

Device and Driver: [RTC Device](#/rt-thread-version/rt-thread-standard/programming-manual/device/rtc/rtc)