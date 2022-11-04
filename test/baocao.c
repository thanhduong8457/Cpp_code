/*Include nhưng thư viện cần thiết*/
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include "stm32f3xx_hal.h"
#include "driver.h"
#include "usb_device.h"
#include "usbd_custom_hid_if.h"

/*Khai báo các biến*/
static uint8_t DI_value;
static float AO_value[2];
static int AI_value[3];
static uint32_t C0_value;
static uint32_t DO_pwm_frequency[3];
static uint16_t Ts_ms =100;
static uint8_t usb_rx_buffer[64];
static uint8_t usb_tx_buffer[17];
static volatile uint8_t usb_tx_flag = 0;
static volatile uint8_t usb_rx_flag = 0;

/*Khai báo các function nhận ngắt và*/
void USB_RX_Interrupt(void);
void Sample_Timer_Interrupt(void);

int main(void)
{
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Cấu hình xung clock cho hệ thống */
    SystemClock_Config();

    /* Init các ngoại vi cần cho project */
    GPIO_Init();
    AI_Init();
    AO_Init();
    Counter_Init();
    MX_USB_DEVICE_Init();
    Sample_Timer_Init();

    /* Đoạn code trong vòng while được thực thi lặp lại vô tận */
    while (1)
    {
        /* Nếu cờ truyền usb_tx_flag lên 1 thì sẽ vào bên trong if function để thực thi đọc dữ liệu và truyền data vào usb_tx_buffer */
        if(usb_tx_flag)
        {
            usb_tx_flag = 0;            // gán lại gía trị cờ usb_tx_flag là false
            DI_value = DI_Read_All();   // Cập nhật giá trị DI_value
            C0_value = Counter_Read();  // Cập nhật giá trị CO_value
            AI_Read_All(AI_value);      // Cập nhật giá trị AI_value

            /* DIN */
            usb_tx_buffer[0] = DI_value; // gán giá trị DI_value vào usb_tx_buffer[0]

            /* counter */
            usb_tx_buffer[1] = (uint8_t)(C0_value >> 24);   // ép kiểu C0_value 8 bit không dấu và dịch sang phải 24 bit rồi gắn vào usb_tx_buffer[1]
            usb_tx_buffer[2] = (uint8_t)(C0_value >> 16);   // như comment trên nhưng dịch phải 16 bit
            usb_tx_buffer[3] = (uint8_t)(C0_value >> 8);
            usb_tx_buffer[4] = (uint8_t)(C0_value);
            /* AIN */
            usb_tx_buffer[5] = (uint8_t)(AI_value[0] >> 24);
            usb_tx_buffer[6] = (uint8_t)(AI_value[0] >> 16);
            usb_tx_buffer[7] = (uint8_t)(AI_value[0] >> 8);
            usb_tx_buffer[8] = (uint8_t)(AI_value[0]);
            usb_tx_buffer[9] = (uint8_t)(AI_value[1] >> 24);
            usb_tx_buffer[10] = (uint8_t)(AI_value[1] >> 16);
            usb_tx_buffer[11] = (uint8_t)(AI_value[1] >> 8);
            usb_tx_buffer[12] = (uint8_t)(AI_value[1]);
            usb_tx_buffer[13] = (uint8_t)(AI_value[2] >> 24);
            usb_tx_buffer[14] = (uint8_t)(AI_value[2] >> 16);
            usb_tx_buffer[15] = (uint8_t)(AI_value[2] >> 8);
            usb_tx_buffer[16] = (uint8_t)(AI_value[2]);
            USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS,usb_tx_buffer,17);
        }
        
        /* Nếu cờ nhận usb_rx_flag lên 1 thì sẽ vào bên trong if function để thực thi*/
        if (usb_rx_flag)
        {
            int i;
            usb_rx_flag = 0;
            switch (usb_rx_buffer[0])/* cmd id */
            {
                case 'N':/* quản lí DO và AO */
                {
                    /* Ghi giá trị usb_rx_buffer[1] ra các DO */
                    DO_Write_All(&usb_rx_buffer[1]);

                    /* Ghi giá trị ra các AO */
                    AO_value[0] = ((float)usb_rx_buffer[9]*256+(float)usb_rx_buffer[10])/1000;
                    AO_value[1] = ((float)usb_rx_buffer[11]*256+(float)usb_rx_buffer[12])/1000;
                    AO_Write_All(AO_value);

                    /*nếu có lệnh reset Counter thì Counter_Reset() được thực thi*/
                    if (usb_rx_buffer[13] == 'R')
                    {
                        /*reset Counter*/
                        Counter_Reset();
                    }
                    break;
                }
                case 'F':/* Điều chỉnh tần số PWM */
                {
                    /* Vòng for được thực thi thực hiện cập nhật giá trị tần số PWM cho 4 kênh ngõ ra */
                    for(i=0;i<3;i++)
                    {
                        DO_pwm_frequency[i]= (uint32_t)(usb_rx_buffer[i+1]);
                    }
                    /* Cập nhật giá trị tần số PWM cho 4 kênh ngõ ra */
                    DO_pwm_set_frequency(DO_pwm_frequency);
                    break;
                }
                case 'G':/* ADC 18 bit gain */
                {
                    AI18_Set_Gain(usb_rx_buffer[1]);
                    break;
                }
                case 'T':/* sample time */
                {
                    /* Cập nhật và set thời gian lấy mẫu */
                    Ts_ms = ((int)usb_rx_buffer[1]<<8) + (int)usb_rx_buffer[2];
                    Sample_Timer_Set_Period(Ts_ms);
                }
            }
        }
    }
}

/* function ngắt nhận */
void USB_RX_Interrupt(void)
{
    int i;
    USBD_CUSTOM_HID_HandleTypeDef *myusb=(USBD_CUSTOM_HID_HandleTypeDef *)hUsbDeviceFS.pClassData;
    //myusb->Report_buf[0]= numbers of byte data
    for (i=0;i<myusb->Report_buf[0];i++)
    {
        usb_rx_buffer[i]=myusb->Report_buf[i+1];
    }
    usb_rx_flag = 1;
}

void Sample_Timer_Interrupt(void)
{
    usb_tx_flag = 1;
}

void _Error_Handler(char *file, int line)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    while(1)
    { }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
    tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif