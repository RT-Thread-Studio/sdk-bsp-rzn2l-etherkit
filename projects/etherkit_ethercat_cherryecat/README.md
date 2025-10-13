# CherryECAT Driver Usage Instructions

**English** | [**中文**](./README_zh.md)

## Introduction

This project provides cherryecat demo.

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

## Running Results

![cherryecat](figures/cherryecat1.png)
![cherryecat](figures/cherryecat2.png)
![cherryecat](figures/cherryecat3.png)
![cherryecat](figures/cherryecat4.png)
![cherryecat](figures/cherryecat5.png)