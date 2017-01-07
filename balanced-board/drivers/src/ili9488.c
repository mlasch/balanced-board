#include <stm32f3xx.h>
#include <cmsis_os.h>
//#include <math.h>
#include <ili9488.h>
#include <math.h>


#define ATT (float)0.2
/* private functions */
static void ili9488_gpio_init(void);
static void ili9488_controller_init(void);
static void ili9488_write_command(uint8_t);
static void ili9488_write_data(uint8_t);
//static void ili9488_data_dir(io_dir_t);
static void ili9488_clear_screen(void);
//static void ili9488_draw_pixel(uint16_t x,uint16_t y, uint16_t color);
//static void ili9488_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
//static void ili9488_draw_circle(uint16_t x0, uint16_t y0, uint16_t radius,uint16_t color);
//static void ili9488_draw_filled_circle(uint16_t x0, uint16_t y0, uint16_t radius,uint16_t color);
//static void ili9488_draw_bitmap_circle(uint16_t x0, uint16_t y0,uint16_t radius,uint16_t bitmap[40][40]);
//static int16_t my_abs(int16_t x);

void check_border(physics_t *phy) {
	if (phy->x < BORDER) {
		phy->x_v = -1*phy->x_v*ATT;
		phy->x = BORDER;
	}
	
	if (phy->x > WIDTH - ball_bitmap.width - BORDER) {
		phy->x_v = -1*phy->x_v*ATT;
		phy->x = WIDTH - ball_bitmap.width - BORDER;
	}
	
	if (phy->y < BORDER) {
		phy->y_v = -1*phy->y_v*ATT;
		phy->y = BORDER;
	}
	
	if (phy->y > HEIGHT - ball_bitmap.height - BORDER) {
		phy->y_v = -1*phy->y_v*ATT;
		phy->y = HEIGHT - ball_bitmap.height - BORDER;
	}
}

void draw_rect(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint16_t color) {
	uint32_t i;

	volatile int32_t width = x1 - x0;
	volatile int32_t height = y1 - y0;
	
	x1--;
	y1--;
	
	if ((int32_t)(x1 - x0) < 0 || (int32_t)(y1 - y0) < 0) {
		//should be an error
		return;
	}
	
	ili9488_write_command(0x2a); // Set_column_address
	ili9488_write_data(x0>>8);
	ili9488_write_data(x0);
	ili9488_write_data(x1>>8);
	ili9488_write_data(x1);
	ili9488_write_command(0x2b); // Set_page_address
	ili9488_write_data(y0>>8);
	ili9488_write_data(y0);
	ili9488_write_data(y1>>8);
	ili9488_write_data(y1);
	ili9488_write_command(0x2c); // Write_memory_start
	
	for(i = 0; i<width*height;i++) {
			ili9488_write_data(color >> 8);  		// high byte
			ili9488_write_data(color & 0x00ff);     // low byte
	}
}

void draw_obj(pix_obj *obj) {
	uint32_t i;
	
	uint32_t x0 = obj->x0;
	uint32_t y0 = obj->y0;
	uint32_t x1 = obj->x0 + obj->width-1;
	uint32_t y1 = obj->y0 + obj->height-1;
	
	ili9488_write_command(0x2a); // Set_column_address
	ili9488_write_data(x0>>8);
	ili9488_write_data(x0);
	ili9488_write_data(x1>>8);
	ili9488_write_data(x1);
	ili9488_write_command(0x2b); // Set_page_address
	ili9488_write_data(y0>>8);
	ili9488_write_data(y0);
	ili9488_write_data(y1>>8);
	ili9488_write_data(y1);
	ili9488_write_command(0x2c); // Write_memory_start
	
	for(i = 0; i<obj->width*obj->height;i++) {
			ili9488_write_data(obj->data[i] >> 8);  		// high byte
			ili9488_write_data(obj->data[i] & 0x00ff);     // low byte
	}
}

void move_obj(pix_obj *obj, uint32_t x, uint32_t y) {
	
	int32_t dx = x - obj->x0;
	int32_t dy = y - obj->y0;
	
	//printf("% 3d % 3d\n", dx, dy);
	
	uint32_t x0 = obj->x0;
	uint32_t y0 = obj->y0;
	uint32_t x1 = obj->x0+obj->width;
	uint32_t y1 = obj->y0+obj->height;
	
	uint32_t x0_n = x;
	uint32_t y0_n = y;
	uint32_t x1_n = x+obj->width;
	uint32_t y1_n = y+obj->height;
	
	uint32_t gr_x0, gr_y0, gr_x1, gr_y1;
	uint32_t bl_x0, bl_y0, bl_x1, bl_y1;
	
	if (dx > 0) {
		if (dy > 0) {
			// dx + , dy +
			gr_x0 = x0;
			gr_y0 = y0;
			gr_x1 = x1;
			gr_y1 = y0_n;
			
			bl_x0 = x0;
			bl_y0 = y0_n;
			bl_x1 = x0_n;
			bl_y1 = y1;
			
		} else {
			// dx + , dy -
			gr_x0 = x0;
			gr_y0 = y1_n;
			gr_x1 = x1;
			gr_y1 = y1;
			
			bl_x0 = x0;
			bl_y0 = y0;
			bl_x1 = x0_n;
			bl_y1 = y1_n;
		}
	} else {
		if (dy > 0) {
			// dx - , dy +
			gr_x0 = x0;
			gr_y0 = y0;
			gr_x1 = x1;
			gr_y1 = y0_n;
			
			bl_x0 = x1_n;
			bl_y0 = y0_n;
			bl_x1 = x1;
			bl_y1 = y1;
		} else {
			// dx - , dy -
			gr_x0 = x0;
			gr_y0 = y1_n;
			gr_x1 = x1;
			gr_y1 = y1;
			
			bl_x0 = x1_n;
			bl_y0 = y0;
			bl_x1 = x1;
			bl_y1 = y1_n;
		}
	}

	// copy new coordinates to object
	obj->x0 = x0_n;
	obj->y0 = y0_n;
	

	
	//check_border(&phy, &ball_bitmap);
	
	draw_rect(gr_x0, gr_y0, gr_x1, gr_y1, 0xffff);
	draw_rect(bl_x0, bl_y0, bl_x1, bl_y1, 0xffff);
	
	draw_obj(obj);
}
	

void ili9488_init(void) {

	ili9488_gpio_init();
	ili9488_controller_init();
	//ili9488_clear_screen();
	
//	for (i = 0; i< 60; i++) {
//		move_obj(&ball_bitmap, 4 , 4);
//		osDelay(10);
//	}
//	for (i = 0; i< 120; i++) {
//		move_obj(&ball_bitmap, 1 , -1);
//		osDelay(10);
//	}
//	for (i = 0; i< 20; i++) {
//		move_obj(&ball_bitmap, -16 , 0);
//		osDelay(10);
//	}
//	for (i = 0; i< 15; i++) {
//		move_obj(&ball_bitmap, 16 , 0);
//		osDelay(10);
//	}
//	
//	for (i = 0; i< 20; i++) {
//		move_obj(&ball_bitmap, -4 , -4);
//		osDelay(10);
//	}

	
}

static void ili9488_gpio_init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	
	LCD_GPIO_CLOCK_ENABLE();
	
	GPIO_InitStructure.Pin = (PIN_WRX | PIN_RDX | PIN_D_CX | PIN_RESX | PIN_CSX);
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	
	HAL_GPIO_Init(LCD_PORT, &GPIO_InitStructure);
	
	/* set the GPIO on startup */
	WRX_IDLE;			// HIGH on idle
	RDX_IDLE;			// HIGH on idle
	D_CX_DATA;		// 0=CMD, 1=DATA
	RESX_IDLE;		// HIGH on idle
	CSX_IDLE;			// HIGH on idle
	
	GPIO_InitStructure.Pin = (PIN_D0 | PIN_D1 | PIN_D2 | PIN_D3 | \
														PIN_D4 | PIN_D5 | PIN_D6 | PIN_D7);
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(LCD_PORT, &GPIO_InitStructure);
	
	/* initialize data pins with zeros */
	LCD_PORT->BSRR = LOW_BYTE_TO_BSRR(0x00);
}

//static void ili9488_data_dir(io_dir_t io_dir) {
//	GPIO_InitTypeDef GPIO_InitStructure;
//	
//	GPIO_InitStructure.Pin = (PIN_D0 | PIN_D1 | PIN_D2 | PIN_D3 | \
//														PIN_D4 | PIN_D5 | PIN_D6 | PIN_D7);
//	
//	if (io_dir == io_out) {
//		GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
//	} else {
//		GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
//	}
//	
//	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
//	HAL_GPIO_Init(LCD_PORT, &GPIO_InitStructure);
//}

static void ili9488_write_command(uint8_t command) {
	D_CX_CMD;
	CSX_ACTIVE;
	WRX_ACTIVE;
	LCD_PORT->BSRR = LOW_BYTE_TO_BSRR(command);
	WRX_IDLE;
	CSX_IDLE;
}

static void ili9488_write_data(uint8_t data) {
	D_CX_DATA;
	CSX_ACTIVE;
	WRX_ACTIVE;
	LCD_PORT->BSRR = LOW_BYTE_TO_BSRR(data);
	WRX_IDLE;
	CSX_IDLE;
}

static void ili9488_controller_init(void) {
	
	// hard reset sequence
	RESX_IDLE;
	osDelay(5);		//5 ms
	RESX_ACTIVE;
	osDelay(15);  // 15 ms
	RESX_IDLE;
	osDelay(15);  // 15 ms
	
	ili9488_write_command(0x01);		//Soft Reset
	
	osDelay(50);
	
	ili9488_write_command(0x28);		//DisplayOff
	ili9488_write_command(0xC0);		//PowerControl1
	ili9488_write_data(0x10);
	ili9488_write_data(0x10);
	ili9488_write_command(0xC1);		//PowerControl2
	ili9488_write_data(0x41);
	ili9488_write_command(0xC5);		//VCOM Control1
	ili9488_write_data(0x00);
	ili9488_write_data(0x22);
	ili9488_write_data(0x80);
	ili9488_write_data(0x40);
	ili9488_write_command(0x36);		//MemoryAccess
	ili9488_write_data(0x28);
	ili9488_write_command(0xB0);		//Interface
	ili9488_write_data(0x00);
	ili9488_write_command(0xB1);		//FrameRateControl
	ili9488_write_data(0xC0);
	ili9488_write_data(0x11);
	ili9488_write_command(0xB4);		//InversionControl
	ili9488_write_data(0x02);
	ili9488_write_command(0xB7);		//EntryMode
	ili9488_write_data(0xC6);
	ili9488_write_command(0x3A);		//InterfacePixelFormat
	ili9488_write_data(0x55);
	ili9488_write_command(0xF7);		//AdjustmentControl
	ili9488_write_data(0xA9);
	ili9488_write_data(0x51);
	ili9488_write_data(0x2c);
	ili9488_write_data(0x82);
	ili9488_write_command(0x11);		//SleepOut	
	osDelay(150);
	ili9488_write_command(0x29);		// Display ON
	
}

static void ili9488_clear_screen(void) {
	
	//draw some pixels
	uint16_t x1 = 0;
	uint16_t y1 = 0;
	uint16_t x2 = 479;
	uint16_t y2 = 319;
	ili9488_write_command(0x2a); // Set_column_address
	ili9488_write_data(x1>>8);
	ili9488_write_data(x1);
	ili9488_write_data(x2>>8);
	ili9488_write_data(x2);
	ili9488_write_command(0x2b); // Set_page_address
	ili9488_write_data(y1>>8);
	ili9488_write_data(y1);
	ili9488_write_data(y2>>8);
	ili9488_write_data(y2);
	ili9488_write_command(0x2c); // Write_memory_start
	
	for(uint32_t i = 0; i<480*320;i++) {
		ili9488_write_data(0xff);  		// 
    ili9488_write_data(0xff);     //
	}
	
}

//static void ili9488_draw_pixel(uint16_t x,uint16_t y, uint16_t color) {
//	
//	ili9488_write_command(0x2a);
//	ili9488_write_data(x>>8);
//	ili9488_write_data(x);
//	ili9488_write_data(x>>8);
//	ili9488_write_data(x);
//	ili9488_write_command(0x2b);
//	ili9488_write_data(y>>8);
//	ili9488_write_data(y);
//	ili9488_write_data(y>>8);
//	ili9488_write_data(y);
//	ili9488_write_command(0x2c);
//	ili9488_write_data(color >> 8);
//	ili9488_write_data(color & 0x00FF);	
//}


//static void ili9488_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
//	uint16_t dx,dy,y;	
//	dx = x2 - x1;
//	dy = y2 - y1;
//	for(uint16_t x=x1;x<=x2;x++) {
//		y = y1 + dy * (x - x1) / dx;
//		ili9488_draw_pixel(x, y, color);
//	}
//}


// draw circle
//static void ili9488_draw_circle(uint16_t x0, uint16_t y0, uint16_t radius,uint16_t color) {
//	
//    uint16_t x = radius;
//    int16_t y = 0;
//    int16_t err = 0;

//    while (x >= y)
//    {
//        ili9488_draw_pixel(x0 + x, y0 + y, color);
//        ili9488_draw_pixel(x0 + y, y0 + x, color);
//        ili9488_draw_pixel(x0 - y, y0 + x, color);
//        ili9488_draw_pixel(x0 - x, y0 + y, color);
//        ili9488_draw_pixel(x0 - x, y0 - y, color);
//        ili9488_draw_pixel(x0 - y, y0 - x, color);
//        ili9488_draw_pixel(x0 + y, y0 - x, color);
//        ili9488_draw_pixel(x0 + x, y0 - y, color);

//        if (err <= 0)
//        {
//            y += 1;
//            err += 2*y + 1;
//        }
//        if (err > 0)
//        {
//            x -= 1;
//            err -= 2*x + 1;
//        }
//    }
//}

//// draw filled circle
//static void ili9488_draw_filled_circle(uint16_t x0, uint16_t y0, uint16_t radius,uint16_t color) {
//	
//	uint16_t x = radius;
//	int16_t y = 0;
//	int16_t err = 0;

//	while (x >= y)
//	{
//			ili9488_draw_line(x0 - x,y0 + y,x0 + x,y0 + y,color);
//			ili9488_draw_line(x0 - y,y0 + x,x0 + y,y0 + x,color);
//			ili9488_draw_line(x0 - x,y0 - y,x0 + x,y0 - y,color);
//			ili9488_draw_line(x0 - y,y0 - x,x0 + y,y0 - x,color);

//		if (err <= 0) {
//			y += 1;
//			err += 2*y + 1;
//		}
//		if (err > 0) {
//			x -= 1;
//			err -= 2*x + 1;
//		}
//	}
//}

//static void ili9488_draw_bitmap_circle(uint16_t x0, uint16_t y0,uint16_t radius,uint16_t bitmap[40][40]) {
//	uint16_t c;
//	uint16_t x = radius;
//	int16_t y = 0;
//	int16_t err = 0;
//	uint16_t dx,dy,y_out,x_out;
//	uint16_t x1,x2,y1,y2;
//	
//	int16_t diffx,diffy;

//	while (x >= y)
//	{
//		x1 = x0 - x;
//		y1 = y0 + y;
//		x2 = x0 + x;
//		y2 = y0 + y;
//		
//		dx = x2 - x1;
//		dy = y2 - y1;
//		for(uint16_t x_out=x1;x_out<=x2;x_out++) {
//			y_out = y1 + dy * (x - x1) / dx;
//			diffx = x_out-x0;
//			diffy = y_out-y0;
//			c = bitmap[0][0];
//			ili9488_draw_pixel(x_out, y_out,bitmap[20+diffx][20+diffy]);
//		}

//		x1 = x0 - y;
//		y1 = y0 + x;
//		x2 = x0 + y;
//		y2 = y0 + x;
//		
//		dx = x2 - x1;
//		dy = y2 - y1;
//		for(uint16_t x_out=x1;x_out<=x2;x_out++) {
//			y_out = y1 + dy * (x - x1) / dx;
//			diffx = x_out-x0;
//			diffy = y_out-y0;
//			ili9488_draw_pixel(x_out, y_out,bitmap[20+diffx][20+diffy]);
//		}
//		
//		x1 = x0 - x;
//		y1 = y0 - y;
//		x2 = x0 + x;
//		y2 = y0 - y;
//		
//		dx = x2 - x1;
//		dy = y2 - y1;
//		for(uint16_t x_out=x1;x_out<=x2;x_out++) {
//			y_out = y1 + dy * (x - x1) / dx;
//			diffx = x_out-x0;
//			diffy = y_out-y0;
//			ili9488_draw_pixel(x_out, y_out,bitmap[20+diffx][20+diffy]);
//		}

//		x1 = x0 - y;
//		y1 = y0 - x;
//		x2 = x0 + y;
//		y2 = y0 - x;
//		
//		dx = x2 - x1;
//		dy = y2 - y1;
//		for(uint16_t x_out=x1;x_out<=x2;x_out++) {
//			y_out = y1 + dy * (x - x1) / dx;
//			diffx = x_out-x0;
//			diffy = y_out-y0;
//			ili9488_draw_pixel(x_out, y_out,bitmap[20+diffx][20+diffy]);
//		}		

//		if (err <= 0) {
//			y += 1;
//			err += 2*y + 1;
//		}
//		if (err > 0) {
//			x -= 1;
//			err -= 2*x + 1;
//		}
//	}
//	
//}

//static int16_t my_abs(int16_t x) {
//	int16_t new_abs;
//	
//	if(x<0) {
//		new_abs = x*(-1);
//	}
//	else {
//		new_abs = x;
//	}
//	
//return new_abs;
//}

