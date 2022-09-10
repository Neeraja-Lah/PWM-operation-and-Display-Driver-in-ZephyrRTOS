#define DT_DRV_COMPAT maxim_max7219


#include <device.h>
#include <drivers/spi.h>
#include <sys/byteorder.h>
#include <drivers/display.h>
#include <logging/log.h>
#include <sys/printk.h>
LOG_MODULE_REGISTER(display_max7219, CONFIG_DISPLAY_LOG_LEVEL);

#define REG_INTENSITY	0x0A
#define REG_LMT_SCN	0x0B
#define REG_MD_DEC	0x09
#define T_REG_DISP	0x0F
#define REG_SHTDWN	0x0C

#define m_count DT_PROP(DT_NODELABEL(disp_node),width)

static uint8_t BUFFER[m_count]={0};

struct conf_m7219 {
	const char *spi_name;
	struct spi_config spi_config;
	uint16_t height;
	uint16_t width;
};

struct max7219_data {
	const struct conf_m7219 *config;
	const struct device *spi_dev;
	uint16_t width;
	uint16_t height;
};

static int trans_m7219(struct max7219_data *data, uint8_t address, uint8_t tx_data)
{	

	uint16_t p = (address<<8)|tx_data;

	struct spi_buf tx_buf = { .buf = &p, .len = 2 };
	struct spi_buf_set tx_bufs = { .buffers = &tx_buf, .count = 1 };
	int ret_value;

	ret_value = spi_write(data->spi_dev, &data->config->spi_config, &tx_bufs);
	if (ret_value < 0) {
		return ret_value;
	}
	
	return 0;
}


static int blankon_m7219(const struct device *dev1)
{
	struct max7219_data *data = (struct max7219_data *)dev1->data;

	return trans_m7219(data, REG_SHTDWN, 0x00);
}

static int blankoff_m7219(const struct device *dev1)
{
	struct max7219_data *data = (struct max7219_data *)dev1->data;

	return trans_m7219(data, REG_SHTDWN, 0x01);
}

static int max7219_read(const struct device *dev1,
			const uint16_t a,
			const uint16_t b,
			const struct display_buffer_descriptor *display_bd,
			void *buf)
{
	return -ENOTSUP;
}

static int buffer_clear_m7219(struct max7219_data *data){
	
	for(int i=m_count-1; i>=0;i--){
		trans_m7219(data, m_count-i, 0x00);
	}
	return 0;
}

static int write_m7219(const struct device *dev1,
			 const uint16_t a,
			 const uint16_t b,
			 const struct display_buffer_descriptor *display_bd,
			 const void *buf)
{	
	uint8_t height = display_bd->height;
	uint8_t width = display_bd->width;
	uint8_t *data = ((uint8_t*)buf);
 
	for(int i=b; i<b+height;i++){
		for(int j=width-1; j>=0;j--){
			if(((data[i]>>j) & 1)>0){
				BUFFER[width-j-1]|= (1<<(i));
				}
			else{
				BUFFER[width-j-1]&= ~(1<<(i));
				}
		}
	}
	for(int k=a; k<width; k++){
		trans_m7219((struct max7219_data*)dev1->data, k+1, BUFFER[k]);
		BUFFER[k]=0;
	}
	return 0;
}

static void *framebuffer_m7219(const struct device *dev1)
{
	return NULL;
}

static int max7219_set_brightness(const struct device *dev1,
				  const uint8_t brightness)
{
	return -ENOTSUP;
}

static int max7219_set_contrast(const struct device *dev1,
				const uint8_t contrast)
{
	return -ENOTSUP;
}

static void capabiliites_m7219(const struct device *dev1,
				     struct display_capabilities *capabilities)
{
	return;
}

static int max7219_set_pixel_format(const struct device *dev1,
				    const enum display_pixel_format pixel_format)
{
	return -ENOTSUP;
}

static int max7219_set_orientation(const struct device *dev1,
				   const enum display_orientation orientation)
{

	return -ENOTSUP;
}

static int lcd_init_m7219(struct max7219_data *data)
{
	// const struct conf_m7219 *config = data->config;
	int ret_value;

	//max7219_set_lcd_margins(data, data->x_offset, data->y_offset);

	ret_value = trans_m7219(data, REG_SHTDWN, 0x01);
	if (ret_value < 0) {
		return ret_value;
	}
	
	ret_value = trans_m7219(data, REG_INTENSITY, 0x08);
	if (ret_value < 0) {
		return ret_value;
	}

	ret_value = trans_m7219(data, REG_LMT_SCN, 0x07);
	if (ret_value < 0) {
		return ret_value;
	}

	ret_value = trans_m7219(data, REG_MD_DEC, 0x00);
	if (ret_value < 0) {
		return ret_value;
	}

	ret_value = trans_m7219(data, T_REG_DISP, 0x00);
	if (ret_value < 0) {
		return ret_value;
	}

	ret_value = buffer_clear_m7219(data);
	if (ret_value < 0) {
		return ret_value;
	}
	return 0;
}

static int init_m7219(const struct device *dev1)
{
	struct max7219_data *data = (struct max7219_data *)dev1->data;
	struct conf_m7219 *config = (struct conf_m7219 *)dev1->config;
	int ret_value;

	data->spi_dev = device_get_binding(config->spi_name);
	if (data->spi_dev == NULL) {
		LOG_ERR("Failed to get SPI device for LCD");
		return -ENODEV;
	}
	ret_value = lcd_init_m7219(data);

	if(ret_value!=0){
		LOG_ERR("Failed to initialize LED Panel");
	}

	return 0;
}

static const struct display_driver_api max7219_api = {
	.blanking_on = blankon_m7219,
	.blanking_off = blankoff_m7219,
	.write = write_m7219,
	.read = max7219_read,
	.get_framebuffer = framebuffer_m7219,
	.set_brightness = max7219_set_brightness,
	.set_contrast = max7219_set_contrast,
	.get_capabilities = capabiliites_m7219,
	.set_pixel_format = max7219_set_pixel_format,
	.set_orientation = max7219_set_orientation,
};


#define init_m7219(inst)							\
	static struct max7219_data max7219_data_ ## inst;			\
										\
	const static struct conf_m7219 max7219_config_ ## inst = {	\
		.spi_name = DT_INST_BUS_LABEL(inst),				\
		.spi_config.slave = DT_INST_REG_ADDR(inst),			\
		.spi_config.frequency = 10000000, 				\
		.spi_config.operation = SPI_OP_MODE_MASTER | SPI_WORD_SET(16) | SPI_TRANSFER_MSB,\
		.spi_config.cs = NULL,			               	\
		.width = DT_INST_PROP(inst, width),				\
		.height = DT_INST_PROP(inst, height),				\
										\
	};									\
										\
	static struct max7219_data max7219_data_ ## inst = {			\
		.config = &max7219_config_ ## inst,				\
										\
	};									\
	DEVICE_DT_INST_DEFINE(inst, init_m7219, NULL,		\
			      &max7219_data_ ## inst, &max7219_config_ ## inst,	\
			      APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY,	\
			      &max7219_api);

DT_INST_FOREACH_STATUS_OKAY(init_m7219)

