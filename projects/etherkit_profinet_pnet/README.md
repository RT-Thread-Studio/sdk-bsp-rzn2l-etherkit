# EtherKit Development Board BSP Documentation

**English** | [**中文**](./README_zh.md)

## Introduction

PROFINET is an industrial Ethernet standard developed and promoted by PI (PROFIBUS and PROFINET International) and is widely used in the industrial automation field.

P-Net is an open-source PROFINET implementation, specifically designed for real-time network communication in embedded devices. It is a lightweight PROFINET protocol stack aimed at providing developers with a quick and efficient way to integrate PROFINET functionality into embedded platforms.

In this example, we will use the P-Net software package to implement PROFINET master-slave communication.

## Prerequisites

**Software Environment**:

- [CODESYS](https://us.store.codesys.com/) (PROFINET master simulation)
    - CODESYS
    - CODESYS Gateway (Gateway device)
    - CODESYS Control Win SysTray (PLC device)

**Hardware Environment**:

- EtherKit development board

## Software Package Configuration

Double-click to open RT-Thread Settings, go to **-> RT-Thread online packages -> IoT**, find **[\*] P-Net stack for Profinet device implementation --->** and enable it. Below is the related user configuration information explanation:

```c
-*- Default netif name for P-NET  --->
    -> (e00) default ethernet interface name for p-net app, default as 'e00'
-*- Enable P-NET sample board config  ---> 
    -> (0x0209) p-net user led pin
    -> (0x0005) p-net user key pin
-*- Default root filesystem path for P-NET  ---> 
    -> [*] p-net using ramfs filesystem by default, or you can turn this off and choose another way to enable the filesystem
    -> (8192)  default memory size for ramfs
-*- P-NET sample slave network ip config  ---> 
    -> (192.168.137.196) set static IP address for PROFINET slave
    -> (192.168.137.1) set static gateway address for PROFINET slave
    -> (255.255.255.0) set static mask address for PROFINET slave
version (latest)  ---> 
```

- **Default netif name for p-net**: p-net network interface name, default is `e00`;
- **Enable pnet sample board config**: p-net app user LED and key configuration;
- **Default root filesystem path for p-net**: p-net filesystem configuration, default uses ramfs with 8K memory allocation;
- **P-NET sample slave network ip config**: Static IP configuration for the p-net slave device (**Make sure to disable RT_LWIP_DHCP and use static IP**).

After configuring these settings, compile and download the program to the development board.

## Network Configuration

Connect the development board to the PC using an Ethernet cable, and configure a static IP on the PC:

![image-20241126114040869](figures/image-20241126114040869.png)

Check the IP information on the development board and test connectivity:

![image-20241126114049493](figures/image-20241126114049493.png)

## Soft PLC Startup

**CODESYS Overview**: CODESYS is a PLC software developed by 3S (Germany), integrating PLC logic, motion control, configuration, display, and other functions. CODESYS, short for **Controller Development System**, is an industrial automation programming tool based on the IEC 61131-3 standard. It supports various programming languages (e.g., Ladder Diagram, Structured Text, Function Block Diagram) and provides libraries and function modules, making it a widely used platform in industrial automation for developing and debugging PLCs and control systems.

### Creating a Standard Project in CODESYS

Ensure that you have CODESYS installed. After installation, the following three software components are required:

- **CODESYS V3.5 SP20 Patch 3**: PROFINET master simulation
- **CODESYS Gateway V3**: Gateway device
- **CODESYS Control Win V3 -x64 SysTray**: Soft PLC device

First, open **CODESYS V3.5 SP20 Patch 3**, choose **New Project** -> **Projects** -> **Standard project**, configure the project name and location, and click **OK**:

![image-20241126114127411](figures/image-20241126114127411.png)

In the pop-up window, keep the default configuration and click **OK**:

![image-20241126114137199](figures/image-20241126114137199.png)

> **Note**: If you have purchased **CODESYS Control RTE SL**, select the device: **CODESYS Control RTE V3 (CODESYS) / x64 (CODESYS)**. For evaluation purposes, you can choose **CODESYS Control Win V3 (CODESYS) / x64** to create the project.

After creation, you will see the main interface:

![image-20241126114149792](figures/image-20241126114149792.png)

### Starting Gateway and Soft PLC

Open the following two software components:

- **CODESYS Gateway V3** (right-click **Start Gateway**)
- **CODESYS Control Win V3 -x64 SysTray** (right-click **Start PLC**)

![image-20241126114204739](figures/image-20241126114204739.png)

Back in CODESYS, double-click **Device(CODESYS Control Win V3 x64)** -> **Communication Settings** -> **Scan Network**:

![image-20241126114216032](figures/image-20241126114216032.png)

In the user login window, configure the username and password (customizable):

![image-20241126114225267](figures/image-20241126114225267.png)

Check if the gateway and soft PLC devices are online:

![image-20241126114233871](figures/image-20241126114233871.png)

### Adding PROFINET GSDML Files

**GSD** (Generic Station Description) files are used for PROFIBUS DP and PROFINET IO communication, describing parameters, diagnostic data, and user-defined data between the PLC and I/O modules.

The GSDML file for this project is located at:

- `..\src\ports\rtthread\pn_dev`

Install the GSDML file from the above path: **GSDML-V2.4-RT-Labs-P-Net-Sample-App-20220324.xml**.

![image-20241126114247755](figures/image-20241126114247755.png)

After installation, you should see the **P-Net slave description file**:

![image-20241126114257110](figures/image-20241126114257110.png)

### Adding Devices

- **Adding Ethernet**: Right-click **Device** in the left navigation bar and select **Ethernet Adapter**:

![image-20241126114309177](figures/image-20241126114309177.png)

- **Adding PROFINET IO Master**: Right-click **Ethernet** in the left navigation bar and select **PN-Controller**:

![image-20241126114338017](figures/image-20241126114338017.png)

- **Adding PROFINET IO Slave**: Right-click **PN-Controller** in the left navigation bar and select **P-Net-multiple-module sample app**:

![image-20241126114354826](figures/image-20241126114354826.png)

### Configuring Tasks

- **Main Tasks Configuration**: Select **Application** -> **Task Configuration** in the left navigation bar, double-click **MainTask (IEC-Tasks)**, set the priority to 1, type to **Cyclic**, and cycle time to 4ms:

![image-20241126114416488](figures/image-20241126114416488.png)

- **Profinet_CommunicationTask Configuration**: Double-click **Profinet_CommunicationTask (IEC-Tasks)**, set the priority to 14, type to **Cyclic**, and cycle time to 10ms:

![image-20241126114430407](figures/image-20241126114430407.png)

### Network Configuration

- **Ethernet Configuration**: Double-click **Ethernet (Ethernet)** in the left navigation bar -> **General**, and modify the network interface to the one connected to the development board (ensure the correct master IP is selected if using multiple stations).

![image-20241126114448562](figures/image-20241126114448562.png)

- **PN_Controller Configuration**: Double-click **PN_Controller (PN-Controller)** -> **General**, and modify the default slave IP parameters as needed.

- **P-Net Slave Network Configuration**: Double-click **P-Net-multiple-module sample app** -> **General**, and modify the IP parameters to match the development board’s IP:

![image-20241126114459034](figures/image-20241126114459034.png)

![image-20241126114512924](figures/image-20241126114512924.png)

### Compile and Debug the Project

- Step 1: From the navigation bar, select **Compile** -> **Generate Code**
- Step 2: Select **Online** -> **Login**
- Step 3: Click **Debug** -> **Start**

You should see the PN master successfully online:

![image-202411261145266

57](figures/image-20241126114526657.png)

## PROFINET Slave Application Startup

Here we use the CODESYS software to test the interaction between the PN master and slave stations.

### LED Blinking

Back in the CODESYS software, in the left navigation panel, select `PN_Controller`, right-click and scan the devices. After clicking on the device name, click on "Blink LED":

![image-20241126114601251](figures/image-20241126114601251.png)

At this point, the development board (PN slave I/O) will display log outputs, accompanied by the onboard User LED blinking:

![img](figures/clip_image050.jpg)

### Modifying Slave I&M (Identification and Maintenance) Data

In the same device scanning interface, click on the I&M section in the lower left corner, modify the information, and write it to the I&M:

![image-20241209165958786](figures/image-20241209165958786.png)

At the same time, PNIO will update the slave station configuration:

![image-20241209170011069](figures/image-20241209170011069.png)

We can click to view the I&M again, and we will see that the I&M data has been successfully modified!