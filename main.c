#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/spi.h>
#include <drivers/gpio.h>
#include <sys/printk.h>
#include <sys/util.h>
#include <shell/shell.h>
#include <shell/shell_uart.h>
#include <fsl_lpspi.h>
#include <version.h>
#include <stdlib.h>
#include <drivers/pwm.h>
#include <drivers/display.h>
#include <fsl_iomuxc.h>
#include <fsl_common.h>

#define PWM_LEDR_NODE	DT_NODELABEL(red_pwm)
#define RED_CHANNEL	DT_PWMS_CHANNEL(PWM_LEDR_NODE)	
#define PWM_LEDG_NODE	DT_NODELABEL(green_pwm)
#define GREEN_CHANNEL	DT_PWMS_CHANNEL(PWM_LEDG_NODE)	
#define PWM_LEDB_NODE	DT_NODELABEL(blue_pwm)
#define BLUE_CHANNEL	DT_PWMS_CHANNEL(PWM_LEDB_NODE)

#define Frequency 50U
#define m_count DT_PROP(DT_NODELABEL(disp_node),width)
#define Period USEC_PER_SEC/Frequency

const struct device *pwm_dev1, *pwm_dev2, *pwm_dev3;

static uint8_t data[m_count]={0};
static int a,b,c;
int n,r, ht=0;
volatile bool blinking_on;
const struct device *display_dev; 

//Checking if pulse duration and period are set for each PWM device

static int pwm_func(const struct shell *shell, size_t argc, char **argv)
{		
		// GPIO Device Pins
		int ret_val;
		a=atoi(argv[1]);
		b=atoi(argv[2]);
		c=atoi(argv[3]);
		
		ret_val=pwm_pin_set_usec(pwm_dev1, RED_CHANNEL, Period, a*(Period/100),0);
		
		if(ret_val){
			printk("Error %d: Failed to set period and Pulse width\n", ret_val);
			return ret_val;
		}
		k_usleep(Period);
		ret_val=pwm_pin_set_usec(pwm_dev2, GREEN_CHANNEL, Period, b*(Period/100), 0);
		
		if(ret_val){
			printk("Error %d: Failed to set period and Pulse width\n", ret_val);
			return ret_val;
		}
		
		ret_val=pwm_pin_set_usec(pwm_dev3, BLUE_CHANNEL, Period, c*(Period/100), 0);
		
		if(ret_val){
			printk("Error %d: Failed to set period and Pulse width\n", ret_val);
			return ret_val;
		}
	return 0;
}

//ledm command functionality
static int pattern_display(const struct shell *shell, size_t argc, char **argv)
{	
		
		ht = argc-2;
		r=atoi(argv[1]);
		int j=2;
		for(int i=r; i<r+ht; i++){
			data[i]=strtol(argv[j], NULL, 16);
			j++;
		}
		
		struct display_buffer_descriptor display_bf;
		display_bf.buf_size=sizeof(data);
		display_bf.width= m_count;
		display_bf.height= ht;
		display_bf.pitch= m_count;		
		display_write(display_dev, 0, r, &display_bf, &data);	
			
	return 0;
}

//To check the argument value
static int blinking_command(const struct shell *shell, size_t argc, char **argv)
{
	n=atoi(argv[1]);
	if(n)
		blinking_on=true;
	if(n==0){
		blinking_on=false;
		display_blanking_off(display_dev);
	}
		
	return 0;
}

//Creating Sub commands
SHELL_STATIC_SUBCMD_SET_CREATE(sub_demo,
	SHELL_CMD(rgb, NULL, "Duty cycle for PWM signals", pwm_func),
	SHELL_CMD(ledm, NULL, "Pattern is displayed on LED Matrix", pattern_display),
	SHELL_CMD(ledb, NULL, "Pattern blinking functionality", blinking_command),
	SHELL_SUBCMD_SET_END
);

//Root command
SHELL_CMD_REGISTER(p2, &sub_demo, "Root command p2", NULL);


void main(void)
{	
	// IO pin names for IO multiplexing operations
	IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_11_FLEXPWM1_PWMB03, 0);

	IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_11_FLEXPWM1_PWMB03,
			    IOMUXC_SW_PAD_CTL_PAD_PUE(1) |
			    IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
			    IOMUXC_SW_PAD_CTL_PAD_SPEED(2) |
			    IOMUXC_SW_PAD_CTL_PAD_DSE(6));

	IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_10_FLEXPWM1_PWMA03, 0);

	IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_10_FLEXPWM1_PWMA03,
			    IOMUXC_SW_PAD_CTL_PAD_PUE(1) |
			    IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
			    IOMUXC_SW_PAD_CTL_PAD_SPEED(2) |
			    IOMUXC_SW_PAD_CTL_PAD_DSE(6));

	IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B0_03_FLEXPWM1_PWMB01, 0);

	IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B0_03_FLEXPWM1_PWMB01,
			    IOMUXC_SW_PAD_CTL_PAD_PUE(1) |
			    IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
			    IOMUXC_SW_PAD_CTL_PAD_SPEED(2) |
			    IOMUXC_SW_PAD_CTL_PAD_DSE(6));

	// PIN MUX OPERATIONS FOR SETTING MOSI, CS AND CLK PINS FOR LPSPI DEVI
	IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B0_02_LPSPI1_SDO, 0);

	IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B0_02_LPSPI1_SDO,
			    IOMUXC_SW_PAD_CTL_PAD_PUE(1) |
			    IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
			    IOMUXC_SW_PAD_CTL_PAD_SPEED(2) |
			    IOMUXC_SW_PAD_CTL_PAD_DSE(6));
	
	IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B0_00_LPSPI1_SCK, 0);

	IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B0_00_LPSPI1_SCK,
			    IOMUXC_SW_PAD_CTL_PAD_PUE(1) |
			    IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
			    IOMUXC_SW_PAD_CTL_PAD_SPEED(2) |
			    IOMUXC_SW_PAD_CTL_PAD_DSE(6));

	IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B0_01_LPSPI1_PCS0, 0);

	IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B0_01_LPSPI1_PCS0,
			    IOMUXC_SW_PAD_CTL_PAD_PUE(1) |
			    IOMUXC_SW_PAD_CTL_PAD_PKE_MASK |
			    IOMUXC_SW_PAD_CTL_PAD_SPEED(2) |
			    IOMUXC_SW_PAD_CTL_PAD_DSE(6));

	char* n1=DT_PROP(DT_NODELABEL(flexpwm1_pwm3),label);
	
	pwm_dev1 = device_get_binding(n1);
	if (!pwm_dev1) {
		printk("PWM Device %s is not ready!\n", pwm_dev1->name);
		return;
	}
	
	char* n2=DT_PROP(DT_NODELABEL(flexpwm1_pwm3),label);
	
	pwm_dev2 = device_get_binding(n2);
	if (!pwm_dev2) {
		printk("PWM Device %s is not ready!\n", pwm_dev2->name);
		return;
	}
	
	char* n3=DT_PROP(DT_NODELABEL(flexpwm1_pwm1),label);
	
	pwm_dev3 = device_get_binding(n3);
	if (!pwm_dev3) {
		printk("PWM Device %s is not ready!\n", pwm_dev3->name);
		return;
	}	
			
	display_dev = device_get_binding(DT_PROP(DT_NODELABEL(disp_node),label));
	
		while(1){
			if(blinking_on){
			display_blanking_on(display_dev);
			k_msleep(1000);
			display_blanking_off(display_dev);
			k_msleep(1000);
		}
		k_msleep(1);
		}
}

