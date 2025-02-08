# EtherKit Development Board BSP Instructions

**English** | [**中文**](./README_zh.md)

## Introduction

This project provides basic Ethernet functionalities, including `ping`, `tftp`, `ntp`, and `iperf`.

## Hardware Connection

To use Ethernet, connect the development board to any one of the three network ports using an Ethernet cable, and the other end should be connected to a network switch that has internet access.

## FSP Configuration Instructions

Open the project configuration file `configuration.xml` and add the `r_gamc` stack:

![image-20241126104408737](figures/image-20241126104408737.png)

Next, click on `g_ether0 Ethernet`, and configure the interrupt callback function to `user_ether0_callback`:

![image-20241126104422910](figures/image-20241126104422910.png)

Now configure the PHY settings. Select `g_ether_phy0`, set the common configuration to "User Own Target", change the PHY LSI address to `1` (refer to the schematic for the exact address), and set the PHY initialization callback function to `ether_phy_targets_initialize_rtl8211_rgmii()`. Also, set the MDIO to GMAC.

![image-20241126104437432](figures/image-20241126104437432.png)

Next, configure `g_ether_selector0`, set the Ethernet mode to "Switch Mode", set the PHY link to "Default Active-Low", and choose "RGMII" for the PHY interface mode.

![image-20241126104519290](figures/image-20241126104519290.png)

Configure the Ethernet pin parameters and select the operating mode to RGMII:

![image-20241126104533098](figures/image-20241126104533098.png)

Finally, configure `ETHER_GMAC`:

![image-20241126104603633](figures/image-20241126104603633.png)

## RT-Thread Studio Configuration

Return to the Studio project, and configure RT-Thread Settings. Click on "Hardware", find the chip device driver, and enable Ethernet:

![image-20241126104852383](figures/image-20241126104852383.png)

In the RT-Thread Settings, search for the `netutils` software package and enable the `tftp`, `iperf`, and `ntp` features:

![image-20241126104905140](figures/image-20241126104905140.png)

## Ethernet IP Experiment Results

After flashing the code to the development board, open the serial terminal to view the logs:

![img](./figures/image1.png)

## **TFTP Server Send Test**

1. Install the Tftpd64-4.60-setup software from `netutils-v1.3.3\tools`:

   ![img](./figures/image2.png)

2. Go back to the development board serial terminal and input the `tftp_server` command to start the TFTP server service:

![img](./figures/image_1.png)

3. Open the installed Tftpd64-4.60 software:

   - Set **Host** to the development board's IP address;
   - Set **Port** to the TFTP server's port (default is 69);
   - Set **Local File** to the file path where the client will send the file (including filename);
   - Click **Put** to send the file to the device.

![img](./figures/image_2.png)

![img](./figures/image3.png)

4. After clicking "Put", a message will indicate that the file has been sent:

![img](./figures/image4.png)

5. Back at the development board terminal, input `ls` to see that the `1.txt` file has been received. You can input `cat 1.txt` to check if the file content matches what was sent:

   - Note: Since **ramfs** is enabled, do not send files larger than 128KB. This is for testing purposes only.

![img](./figures/image5.png)

## **TFTP Receive Test**

1. Back at the development board serial terminal, input `echo "rtthread" 2.txt` to create a file with custom content:

![img](./figures/image6.png)

2. You can verify the file creation and content:

![img](./figures/image7.png)

3. Open the installed Tftpd64-4.60 software:

   - Set **Local File** to the path where the client will save the received file (including filename);
   - Set **Remote File** to the path of the file on the server (including filename), i.e., the file to be received;
   - Set the TFTP server port to 69;
   - Click **Get** to receive the file.

![img](./figures/image8.png)

   - You should see that `2.txt` has been successfully received, and its content matches the file in the development board's file system:

![img](./figures/image9.png)

## NTP Network Time Synchronization

> NTP (Network Time Protocol) is a protocol used to synchronize computer clocks with a global time standard.

### NTP Experiment Results

After flashing the code to the development board, open the serial terminal to view the logs:

![img](./figures/image10.png)

Input the `ntp_sync` command, and you should see the network time synchronization result. Input the `date` command to check that the RTC time has been synchronized:

![img](./figures/image11.png)