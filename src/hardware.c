/**
 * @file hardware.c
 * @author Matt Krol
 */

#include "hardware.h"
#include "synth.h"

void pll_init(void)
{
    /* enable HSE oscillator */
    RCC->CR |= (1 << 16);
    /* wait for HSE to stabilize */
    while (!(RCC->CR & (1 << 17)));
    /**
     * PLL configuration
     * PLLM = 4
     * PLLN = 168
     * PLLP = 2
     * PLLQ = 7
     * set the PLL clock source to HSE
     * PLL output is 336 MHz
     * sysclock is 168 MHz
     * USB and SDIO clock is 48 MHz
     */
    RCC->PLLCFGR = 4 | (168 << 6) | (1 << 22) | (7 << 24);
    /* enable the PLL */
    RCC->CR |= (1 << 24);
    /* wait for PLL to stabilize */
    while (!(RCC->CR & (1 << 25)));
    /* set flash latency */
    FLASH->ACR |= 5;
    /* make AHB clock 168 MHz by division by 1 and reset CFGR */
    RCC->CFGR &= 0;
    /**
     * make APB1 clock 42 MHz from divison by 4 
     * make APB2 clock 84 MHz from division by 2
     * set PLL as system clock
     */
    RCC->CFGR |= (5 << 10) | (4 << 13) | 2;
    /* wait for system clock to change to PLL */
    while (!(RCC->CFGR & (2 << 2)));
}

void audio_init(void)
{
    /* enable clock on GPIOA */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    /* set GPIO PA4 output to analog mode */
    GPIOA->MODER |= (3 << 8);
    /* set GPIO PA4 output to very high speed */
    GPIOA->OSPEEDR |= (3 << 8);

    /* enable clock on DAC */
    RCC->APB1ENR |= RCC_APB1ENR_DACEN;
    /* enable triggers on DAC channel 1 */
    DAC->CR |= (1 << 2);
    /* set trigger source to timer 2 */
    DAC->CR |= (4 << 3);
    /* enable DMA on DAC channel 1 */
    DAC->CR |= (1 << 12);
    /* enable DAC channel 1 */
    DAC->CR |= 1;

    /* enable clock on DMA */
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
    /* enable interupts for the DMA1 stream 5 on the NVIC */
    NVIC_EnableIRQ(DMA1_Stream5_IRQn);
    /* set to high priority */
    DMA1_Stream5->CR |= (3 << 16);
    /* set memory data size to half-word i.e. 16 bits */
    DMA1_Stream5->CR |= (1 << 13);
    /* set peripheral data size to half-word i.e. 16 bits */
    DMA1_Stream5->CR |= (1 << 11);
    /* enable memory increment mode */
    DMA1_Stream5->CR |= (1 << 10);
    /* enable circular mode */
    DMA1_Stream5->CR |= (1 << 8);
    /* set data transfer mode to memory to peripheral */
    DMA1_Stream5->CR |= (1 << 6);
    /* enable complete and half interrupts */
    DMA1_Stream5->CR |= (3 << 3);
    /* select channel 7 */
    DMA1_Stream5->CR |= (7 << 25);
    /* set the peripheral address */
    DMA1_Stream5->PAR = (uint32_t)&(DAC1->DHR12R1);
    /* set the memory 0 address */
    DMA1_Stream5->M0AR = (uint32_t)audio_buffer;
    /* set the number of data */
    DMA1_Stream5->NDTR = AUDIO_BUFFER_SIZE;
    /* enable stream */
    DMA1_Stream5->CR |= 1;

    /* enable clock on TIM2 */
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    /* set TIM2 to overflow at 48000 Hz */
    TIM2->PSC = 0;
    TIM2->ARR = 1749;
    /* set TIM2 to master mode trigger */
    TIM2->CR2 |= (2 << 4);
    /* reset CR1 bits to default */
    TIM2->CR1 &= 0;
    /* turn on TIM2 */
    TIM2->CR1 |= 1;
}

void DMA1_Stream5_IRQHandler(void)
{
    if (DMA1->HISR & (1 << 10)) {
        /* DMA is half finished reading the buffer */
        for (uint16_t i = 0; i < (AUDIO_BUFFER_SIZE/2); i++) {
            audio_buffer[i] = render_sample();
        }
    }
    if (DMA1->HISR & (1 << 11)) {
        /* DMA finished reading the buffer */
        for (uint16_t i = (AUDIO_BUFFER_SIZE/2); i < AUDIO_BUFFER_SIZE; i++) {
            audio_buffer[i] = render_sample();
        }
    }
    /* clear interrupt flags */
    DMA1->HIFCR |= (1 << 11) | (1 << 10);
}
