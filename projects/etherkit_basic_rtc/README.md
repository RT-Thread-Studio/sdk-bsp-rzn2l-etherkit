# RZ EtherKit Development Board RTC and Alarm Usage Guide

**English** | [**Chinese**](./README_zh.md)

## Introduction

This example demonstrates how to use the RTC (Real-Time Clock) on the EtherKit development board. The RTC provides accurate real-time information, including year, month, day, hour, minute, and second. Modern RTC chips often use a high-precision crystal oscillator as their clock source. Some RTC chips include battery backup to ensure continuous operation and valid timekeeping during main power outages.

RT-Thread's RTC device provides fundamental services for the operating system's timekeeping system. With the increasing prevalence of IoT scenarios, RTC has become a standard feature of many products. For example, RTC is indispensable in secure communication protocols like SSL.

## Hardware Description

This example uses an RTC device that relies on the LSE clock. No additional connections are required.

## Software Description

### FSP Configuration

Open FSP, locate the `configuration.xml` file in the corresponding project folder, and add an RTC stack:

![img](./figures/wps7.jpg)  

**Figure 5-1** Adding an RTC Stack

Next, configure the RTC parameters. Set the RTC stack name to `g_rtc`, specify the RTC interrupt callback function as `rtc_callback`, and configure the interrupt priority:

![img](./figures/wps8.jpg)  

**Figure 4-2** RTC Parameter Configuration

### RT-Thread Settings

Open the RT-Thread Settings, find the hardware options, and enable RTC functionality:

![img](./figures/wps9.jpg)  

**Figure** Enabling RTC

Configure the RTC by enabling RT-Thread's RTC device framework and the software alarm functionality. Note: Due to current issues with the alarm functionality in Renesas RZ/N series devices, the alarm is simulated in software without affecting usability:

![img](./figures/wps10.jpg)  

**Figure** RTC Configuration

## Example Code Description

The source code for this example is located in `/projects/etherkit_basic_rtc`. In the `hal_entry()` function, the RTC device is obtained, the system time is set, and the time is retrieved to verify the operation. After a 1-second delay, the system time is retrieved again.

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

/* Set the date */
ret = set_date(2024, 11, 11);
rt_kprintf("set RTC date to 2024-11-11\n");
if (ret != RT_EOK)
{
    rt_kprintf("set RTC date failed\n");
}

/* Set the time */
ret = set_time(15, 00, 00);
if (ret != RT_EOK)
{
    rt_kprintf("set RTC time failed\n");
}

/* Delay for 3 seconds */
rt_thread_mdelay(3000);

/* Get the time */
get_timestamp(&now);
rt_kprintf("now: %.*s", 25, ctime(&now));
```

The following code creates an RTC alarm that triggers after 1 second and integrates it into the `msh` command line:

```c
void user_alarm_callback(rt_alarm_t alarm, time_t timestamp)
{
    rt_kprintf("user alarm callback function.\n");
}

void alarm_sample(void)
{
    rt_device_t dev = rt_device_find("rtc");
    struct rt_alarm_setup setup;
    struct rt_alarm *alarm = RT_NULL;
    static time_t now;
    struct tm p_tm;

    if (alarm != RT_NULL)
        return;

    /* Get the current timestamp and set the alarm time to the next second */
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

/* Export msh command */
MSH_CMD_EXPORT(alarm_sample, alarm sample);
```

## Execution

### Compilation & Download

- **RT-Thread Studio**: Use the RT-Thread Studio package manager to download the EtherKit resource package. Create a new project and compile it.  
- **IAR**: Run `mklinks.bat` to create symbolic links for the `rt-thread` and `libraries` folders. Then use `Env` to generate the IAR project. Finally, open `project.eww` in IAR and compile it.

After compiling, connect the JLink interface of the development board to the PC and download the firmware to the board.

### Operation Effect

Press the reset button to restart the development board. The following output is displayed on the board:

```
 \ | /
- RT -   Thread Operating System
 / | \   5.1.0 build Nov 13 2024 13:35:43
 2006 - 2024 Copyright by RT-Thread team

Hello RT-Thread!
==================================================
This example project is an RTC and alarm routine!
==================================================
set RTC date to 2024-11-11
msh >now: Sat Nov 19 07:42:42 3385
msh >alarm_sample
user alarm callback function.
user alarm callback function.
user alarm callback function.
user alarm callback function.
```

## Notes

None at the moment.

## References

- Devices and Drivers: [RTC Device](https://www.rt-thread.org/document/site/#/rt-thread-version/rt-thread-standard/programming-manual/device/rtc/rtc)