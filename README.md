# FuzzingIoTDevices

This repository is for demonstrating reproducibility for the report 'Protocol fuzzing on Internet of Things Operating Systems'.
# Prerequisites: 
Building and flashing this code requires several software libraries. This implementation was only tested on Debian Linux with nRF5340 boards from Nordic Semiconductor. 
VS Code is required with the nRF connect extension installed. If you do not have a toolchain or SDK installed, you must do so. The versions of which used were v2.5.0 for both. This can be done by using the nRF connect tab. 
# Building:
Once you have the required software installed, you need to create a build folder. This can be done by using the nRF connect extension tab. 
# Modifying the code based on fuzzing attempt:
The uploaded code needs to be modified, based on the fuzz you are wanting to attempt. This can be done by uncommenting some of the code under the “////// Fuzzing section //////”. You will also need to change the parameters of the otMessageWrite function to make sure you are overwriting the correct sections of the packet. 
# Flashing:
Once you have flashed both the CoAP client and server to two separate boards, pressing button 3 on the client should send the fuzzed packet to the server. 
# Monitoring:
nRF connect allows you to connect to the UART of the devices allowing you to see outputted data from “printk” statements. Here you should be able determine if fuzzing is being handled correctly. 
