#include<reg52.h>

sbit LATCH1=P2^0; // �q��s
sbit LATCH2=P2^1; // ����s

#define DataPort P0

unsigned char buf[8];
unsigned char TempData[8];
unsigned char code dofly_DuanMa[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
unsigned char code dofly_WeiMa[] = {0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};
unsigned char head = 0;
bit load = 0;

void InitUART(void) {
    SCON = 0x50;
    TMOD |= 0x20;
    TH1 = 0xFA;
    TR1 = 1;
    EA = 1;
    ES = 1;
}

void Display(unsigned char FirstBit, unsigned char Num) {
    static unsigned char i = 0;

    DataPort = 0;
    LATCH1 = 1;
    LATCH1 = 0;

    DataPort = dofly_WeiMa[i + FirstBit];
    LATCH2 = 1;
    LATCH2 = 0;

    DataPort = TempData[i];
    LATCH1 = 1;
    LATCH1 = 0;

    i++;
    if (i == Num)
        i = 0;
}

void DelayUs2x(unsigned char t) {
    while (--t);
}

void DelayMs(unsigned char t) {
    while (t--) {
        DelayUs2x(245);
        DelayUs2x(245);
    }
}

void UART_SER(void) interrupt 4 using 1 {
    unsigned char Temp;

    P1 = 0xFF;  // ���_�� LED �G

    if (RI) {
        RI = 0;
        Temp = SBUF;

        if (Temp >= '0' && Temp <= '9') {
            buf[head] = Temp;
            head++;
            if (head >= 8) {
                load = 1; // �q���D�`����ƺ��F
            }
        }
    }
}

void main(void) {
    unsigned char i;

    InitUART();

    for (i = 0; i < 8; i++) {
        buf[i] = '0';
        TempData[i] = dofly_DuanMa[0];
    }
    TempData[1] |= 0x80;
    TempData[5] |= 0x80;

    while (1) {
        if (load) {
            // �B�z 8 �Ӧr�����
            for (i = 0; i < 8; i++) {
                TempData[i] = dofly_DuanMa[buf[i] - '0'];
            }
            TempData[1] |= 0x80;
            TempData[5] |= 0x80;

            load = 0;
            head = 0;  // ���m head

            P1 = 0x00; // ������s�A���� LED
        }

        Display(0, 8);
        DelayMs(2);
    }
}


