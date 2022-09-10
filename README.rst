
**********************CSE522-Real Time Embedded Systems**********************
*****************************Assignment-2*************************************


Name - Neeraja Lahudva Kuppuswamy
ASU ID - 1224187432

Description : 

In this assignment, we have developed an application program with 3 shell commands and functions to control an RGB LED along with brightness control using the PWM blocks with frequency of the signals fixed at 50 Hz, and developed the display driver for MAX7219-controlled led matrix. Interfaced the LED matrix with the driver using hardware SPI lpspi1. 


******************************************************************************
*****************Steps to compile and execute the code*****************


1. Unzip the RTES-LahudvaKuppuswamy-N_02.zip in the zephyrproject directory.

2. To build, run west build -b mimxrt1050_evk project_2.

3. Run west -v flash.
	
4. Open putty and select port /dev/ttyACM0 and enter baud rate 115200

5. Enter the root command (p2) and then sub commands as explained below.

6. For controlling the intensity of the LED:
	Enter the subcommand rgb and then 3 input values to set the duty cycle of each PWM signal to the LED

7. For displaying the row pattern
	Enter the subcommand ledm and input the starting row number and the corresponding hex values to display the row patterns
	
8. For blinking mode
	Enter subcommand ledb with argument n, with input of 1 to blink the pattern on and off with a period of 1 second, and an input of 0 to retain the pattern and stop blinking

9. To generate the patch file, use the following command:
	diff –rauN /(original zephyr tree)/drivers/display /(modified zephyr tree)/drivers/display > patch_display
 	



	



