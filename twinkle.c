#include "msp.h"

/**
 * main.c
 */

#define BUZZER BIT7
#define BUZZER_PIN P2
#define NOTE_C 718
#define NOTE_D 639
#define NOTE_E 570
#define NOTE_F 538
#define NOTE_G 479
#define NOTE_A 427
#define NOTE_B 380

char twinkle[] = "ccggaagffeeddcggffeedggffeedccggaagffeeddc";
char twinkleDuration[] = { 1, 3, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1,
                           1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1,
                           1, 1, 1, 1, 1, 2 };
char odeToJoy[] =
        "eefggfedccdeeddeefggfedccdedccddecdefdcdefedcdeefggfedccdedcc";

int halfPeriodArray[] =
        { NOTE_A, NOTE_B, NOTE_C, NOTE_D, NOTE_E, NOTE_F, NOTE_G };
int index = 0;
int note_period;
int arraySize;

void main(void)
{

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

    //set pin2.7 as output for the buzzer
    BUZZER_PIN->DIR |= BUZZER;

    arraySize = sizeof(twinkle) / sizeof(twinkle[0]);

    note_period = halfPeriodArray[twinkle[index] - 'a'];

    //enable timer A0 in continuous mode with SMCLK divided by 4
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__CONTINUOUS
            | TIMER_A_CTL_ID__8 | TIMER_A_CTL_IE;

    //set the first interrupt interval to 1.9ms
    TIMER_A0->CCR[0] = TIMER_A0->R + note_period;

    //enable interrupt for channel 0
    //TIMER_A0->CCTL[0] =TIMER_A_CCTLN_CCIE;

    //TIMER_A1->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__UP
    // | TIMER_A_CTL_IE | TIMER_A_CTL_ID__8;

    //TIMER_A1->CCR[0] = 65535;

    NVIC_EnableIRQ(TA0_N_IRQn);

    //enable NVIC for timer A0 channel 0
    NVIC_EnableIRQ(TA0_0_IRQn);

    //global interrupt enable
    __enable_interrupts();

    while (1)
    {
        if (index >= arraySize-1)

            index = 0;
        note_period = halfPeriodArray[twinkle[index] - 'a'];
    }
}

void TA0_N_IRQHandler(void)
{
    static int keep = 0;
    static int No_Tone = 0;
    TIMER_A0->CTL &= ~TIMER_A_CCTLN_CCIFG;
    TIMER_A0->CTL &= ~TIMER_A_CCTLN_CCIFG;


    if (twinkleDuration[index] == 1 || (keep == twinkleDuration[index] - 1))
    {
        No_Tone++;
        keep = 0;

        if (No_Tone % 2)
        {
            TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;
        }
        else
        {
            TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIE;
            index++;
        }
    }
    else
    {
        keep++;
        TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;

    }

}

void TA0_0_IRQHandler(void)
{
    //clear flag
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;

    //toggle buzzer pin
    BUZZER_PIN->OUT ^= BUZZER;

    TIMER_A0->CCR[0] += note_period;
}
