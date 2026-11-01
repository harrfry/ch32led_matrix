#include <ch32v20x.h>
#include <debug.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    GPIO_TypeDef* port;
    uint16_t pin;
} GPIO_Pin_Map_t;

static const GPIO_Pin_Map_t col_pins[8] = {
    {GPIOB, GPIO_Pin_1},
    {GPIOA, GPIO_Pin_2},
    {GPIOA, GPIO_Pin_3},
    {GPIOA, GPIO_Pin_4},
    {GPIOA, GPIO_Pin_5},
    {GPIOA, GPIO_Pin_6},
    {GPIOA, GPIO_Pin_7},
    {GPIOB, GPIO_Pin_0}
};

// Rows (Sources, Push-Pull)
static const GPIO_Pin_Map_t row_pins[8] = {
    {GPIOA, GPIO_Pin_1},
    {GPIOA, GPIO_Pin_0},
    {GPIOB, GPIO_Pin_7},
    {GPIOB, GPIO_Pin_6},
    {GPIOB, GPIO_Pin_5},
    {GPIOB, GPIO_Pin_4},
    {GPIOB, GPIO_Pin_3},
    {GPIOA, GPIO_Pin_15}
};


void NMI_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void matrix_init(void) {
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    for(uint8_t i = 0; i < 8; i++) {
        GPIO_InitStructure.GPIO_Pin = col_pins[i].pin;
        GPIO_Init(col_pins[i].port, &GPIO_InitStructure);
        GPIO_WriteBit(col_pins[i].port, col_pins[i].pin, Bit_SET);
    }

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    for(uint8_t i = 0; i < 8; i++) {
        GPIO_InitStructure.GPIO_Pin = row_pins[i].pin;
        GPIO_Init(row_pins[i].port, &GPIO_InitStructure);
        GPIO_WriteBit(row_pins[i].port, row_pins[i].pin, Bit_RESET);
    }
}

#define TOTAL_FRAMES 28


const uint8_t raw_video_stream[TOTAL_FRAMES][8] = {
    { 0xFF, 0xFF, 0x0F, 0x09, 0x01, 0x05, 0x07, 0x0F }, // Frame 0
    { 0xFF, 0xFF, 0x3F, 0x07, 0x00, 0x00, 0x07, 0x07 }, // Frame 1
    { 0xFF, 0xFF, 0x3F, 0x07, 0x00, 0x00, 0x07, 0x07 }, // Frame 2
    { 0xFF, 0xFF, 0xFF, 0x07, 0x00, 0x00, 0x03, 0x07 }, // Frame 3
    { 0xFF, 0xFF, 0xCF, 0x07, 0x00, 0x00, 0x03, 0x07 }, // Frame 4
    { 0xFF, 0xFF, 0x87, 0x07, 0x00, 0x00, 0x07, 0x07 }, // Frame 5
    { 0xFF, 0xDF, 0x87, 0x07, 0x00, 0x00, 0x07, 0x9F }, // Frame 6
    { 0xFF, 0xFF, 0x07, 0x07, 0x00, 0x00, 0x07, 0x9F }, // Frame 7
    { 0xFF, 0xFF, 0x07, 0x05, 0x00, 0x05, 0x07, 0xFF }, // Frame 8
    { 0xFF, 0xFF, 0x0F, 0x01, 0x00, 0x05, 0x87, 0xFF }, // Frame 9
    { 0xFF, 0xFF, 0x07, 0x01, 0x01, 0x07, 0x0F, 0xFF }, // Frame 10
    { 0xFF, 0x8F, 0x07, 0x01, 0x01, 0x07, 0x0F, 0xFF }, // Frame 11
    { 0xFF, 0x0F, 0x07, 0x01, 0x01, 0x0F, 0x3F, 0xBF }, // Frame 12
    { 0xFF, 0x0F, 0x07, 0x01, 0x01, 0x0F, 0x3F, 0xFF }, // Frame 13
    { 0xFF, 0x0F, 0x07, 0x01, 0x01, 0x07, 0xFF, 0xFF }, // Frame 14
    { 0xFF, 0x0F, 0x05, 0x00, 0x00, 0x07, 0xDF, 0xFF }, // Frame 15
    { 0xFF, 0x07, 0x05, 0x00, 0x00, 0x07, 0xFF, 0xFF }, // Frame 16
    { 0xCF, 0x07, 0x01, 0x00, 0x00, 0x07, 0xDF, 0xFF }, // Frame 17
    { 0xCF, 0x07, 0x01, 0x00, 0x00, 0x07, 0x9F, 0xBF }, // Frame 18
    { 0x8F, 0x07, 0x01, 0x00, 0x00, 0x07, 0xBF, 0xFF }, // Frame 19
    { 0x9F, 0x07, 0x01, 0x00, 0x00, 0x07, 0xBF, 0xFF }, // Frame 20
    { 0xDF, 0x07, 0x01, 0x00, 0x00, 0x07, 0xFF, 0xFF }, // Frame 21
    { 0xFF, 0x07, 0x05, 0x00, 0x01, 0x07, 0xFF, 0xFF }, // Frame 22
    { 0xFF, 0x0F, 0x05, 0x01, 0x01, 0x07, 0x8F, 0xFF }, // Frame 23
    { 0xFF, 0x8F, 0x07, 0x01, 0x01, 0x07, 0x8F, 0xFF }, // Frame 24
    { 0xFF, 0x0F, 0x0F, 0x01, 0x01, 0x07, 0x0F, 0x7F }, // Frame 25
    { 0xFF, 0xBF, 0x0F, 0x01, 0x01, 0x07, 0x0F, 0x7F }, // Frame 26
    { 0xFF, 0xFF, 0x0F, 0x01, 0x01, 0x07, 0x0F, 0xBF } // Frame 27

};



volatile uint32_t current_frame = 0;



void Timer2_Init(void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = 30;
    TIM_TimeBaseStructure.TIM_Prescaler = 144 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM2, ENABLE);
}

void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void TIM2_IRQHandler(void) {
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {

    static uint8_t col_idx = 0;
    static uint8_t row_idx = 0;

    GPIO_WriteBit(row_pins[row_idx].port, row_pins[row_idx].pin, Bit_RESET);
    GPIO_WriteBit(col_pins[col_idx].port, col_pins[col_idx].pin, Bit_SET);

    col_idx++;
    if(col_idx >= 8) {
        col_idx = 0;
        row_idx++;
        if(row_idx >= 8) {
            row_idx = 0;
        }
    }

    uint8_t col_data = raw_video_stream[current_frame][col_idx];

    if((col_data & (1 << row_idx)) == 0) {
        GPIO_WriteBit(row_pins[row_idx].port, row_pins[row_idx].pin, Bit_SET);
        GPIO_WriteBit(col_pins[col_idx].port, col_pins[col_idx].pin, Bit_RESET);
    }
}
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}

int main(void) {
    SystemInit();
    SystemCoreClockUpdate();
    Delay_Init();
    matrix_init();
    Timer2_Init();
    
    while (1) {
        Delay_Ms(83);

        current_frame++;
        if(current_frame >= TOTAL_FRAMES) {
            current_frame = 0;
        }
    }
    return 0;
}



void NMI_Handler(void) {}
void HardFault_Handler(void)
{

    
    while (1)
    {
        
    }
}