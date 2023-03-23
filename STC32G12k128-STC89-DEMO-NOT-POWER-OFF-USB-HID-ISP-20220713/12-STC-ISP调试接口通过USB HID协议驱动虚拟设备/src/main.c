/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* --- Web: www.STCMCUDATA.com  ---------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ����STC�����ϼ�����            */
/*---------------------------------------------------------------------*/

/*************  ����˵��    **************

�����̻���STC32G����ת�Ӱ壨�����������б�д���ԡ�

ͨ��USB����STC-ISP(V6.90�Ժ�汾);

STC-ISP���Խӿ��������������ϵ������������Ӧ�ļ�ֵ��MCU;

MCU�յ���ִֵ�ж�Ӧ�Ĺ���;

���������ʾ���ָ�λ����USB����ģʽ�ķ�����
1. ͨ��ÿ1����ִ��һ�Ρ�KeyResetScan��������ʵ�ֳ���P3.2�ڰ�������MCU��λ������USB����ģʽ��
   (�����ϣ����λ����USB����ģʽ�Ļ������ڸ�λ�����ｫ IAP_CONTR ��bit6��0��ѡ��λ���û�������)
2. ͨ�����ء�stc_usb_hid_32g.lib���⺯����ʵ��ʹ��STC-ISP�������ָ���MCU��λ������USB����ģʽ���Զ����ء�

����ʱ, ѡ��ʱ�� 24MHZ (�û��������޸�Ƶ��)��

******************************************/

#include "stc.h"
#include "usb.h"
#include "vk.h"
#include "pic.h"

void sys_init();
void DelayXms(int n);

char *USER_DEVICEDESC = NULL;
char *USER_PRODUCTDESC = NULL;

char *USER_STCISPCMD = "@STCISP#";                      //�����Զ���λ��ISP�����û��ӿ�����

BYTE xdata cod[8];

void main()
{
    sys_init();
    usb_init();
    EA = 1;

    while (1)
    {
        if (DeviceState != DEVSTATE_CONFIGURED)        //�ж�USB�豸ʶ���Ƿ����
            continue;

        if (bUsbOutReady)
        {
            if ((UsbOutBuffer[0] == 'K') &&
                (UsbOutBuffer[1] == 'E') &&
                (UsbOutBuffer[2] == 'Y') &&
                (UsbOutBuffer[3] == 'P'))
            {
                switch (UsbOutBuffer[5])
                {
                case VK_DIGIT_1:
                    printf("%08lx", 0x1234abcdL);      //�����������ʾ�ַ���
                    break;
                case VK_DIGIT_2:
                    SEG7_ShowLong(0x98765432, 16);     //�����������ʾ4�ֽڳ�������
                    break;
                case VK_DIGIT_3:
                    SEG7_ShowFloat(3.1415);            //�����������ʾIEEE754��ʽ�����ȸ�����
                    break;
                case VK_DIGIT_4:
                    cod[0] = 0x3f;
                    cod[1] = 0x06;
                    cod[2] = 0x5b;
                    cod[3] = 0x4f;
                    cod[4] = 0x66;
                    cod[5] = 0x6d;
                    cod[6] = 0x7d;
                    cod[7] = 0x27;
                    SEG7_ShowCode(cod);                //���������ֱ����ʾ�����Ķ���
                    break;
                case VK_DIGIT_5:
                    cod[0] = 0x01;    //����λ��P0��Ч
                    cod[1] = 0xaa;    //P0״̬
                    LED40_SendData(cod, 2);            //����DIP40�ĸ����ܽ���LED��״̬
                    break;
                case VK_DIGIT_6:
                    cod[0] = 0x0f;    //����λ��P0~P3��Ч
                    cod[1] = 0x12;    //P0״̬
                    cod[2] = 0x34;    //P1״̬
                    cod[3] = 0x56;    //P2״̬
                    cod[4] = 0x78;    //P3״̬
                    LED40_SendData(cod, 5);            //����DIP40�ĸ����ܽ���LED��״̬
                    break;
                case VK_DIGIT_7:
                    cod[0] = 0x01;    //����λ��P0��Ч
                    cod[1] = 0xaa;    //P0״̬
                    LED64_SendData(cod, 2);            //����DIP64�ĸ����ܽ���LED��״̬
                    break;
                case VK_DIGIT_8:
                    cod[0] = 0x0f;    //����λ��P0~P3��Ч
                    cod[1] = 0x12;    //P0״̬
                    cod[2] = 0x34;    //P1״̬
                    cod[3] = 0x56;    //P2״̬
                    cod[4] = 0x78;    //P3״̬
                    LED64_SendData(cod, 5);            //����DIP64�ĸ����ܽ���LED��״̬
                    break;
                case VK_ALPHA_A:
                    LCD12864_DisplayOff();             //�ر�LCD12864����ʾ
                    break;
                case VK_ALPHA_B:
                    LCD12864_DisplayOn();              //��LCD12864����ʾ
                    break;
                case VK_ALPHA_C:
                    LCD12864_CursorOff();              //����LCD12864�Ĺ��
                    break;
                case VK_ALPHA_D:
                    LCD12864_CursorOn();               //��ʾLCD12864�Ĺ��
                    break;
                case VK_ALPHA_E:
                    LCD12864_CursorMoveLeft();         //��������ƶ�һ����λ��16�����أ�
                    break;
                case VK_ALPHA_F:
                    LCD12864_CursorMoveRight();        //��������ƶ�һ����λ��16�����أ�
                    break;
                case VK_ALPHA_G:
                    LCD12864_CursorReturnHome();       //���ص����Ͻǳ�ʼλ��
                    break;
                case VK_ALPHA_H:
                    LCD12864_ScrollLeft();             //��Ļ�������һ����λ��16�����أ�
                    break;
                case VK_ALPHA_I:
                    LCD12864_ScrollRight();            //��Ļ���ҹ���һ����λ��16�����أ�
                    break;
                case VK_ALPHA_J:
                    LCD12864_ScrollUp(16);             //��Ļ���Ϲ���һ����λ��16�����أ�
                    break;
                case VK_ALPHA_K:
                    LCD12864_AutoWrapOff();            //�ر��Զ����й���
                    break;
                case VK_ALPHA_L:
                    LCD12864_AutoWrapOn();             //�����Զ����й���
                    break;
                case VK_ALPHA_M:
                    LCD12864_ReverseLine(1);           //������ʾָ������
                    break;
                case VK_ALPHA_N:
                    LCD12864_DisplayClear();           //�����ʾ
                    break;
                case VK_ALPHA_O:
                    LCD12864_ShowString(0, 0, "���ڹ�о�˹��������޹�˾");  //��LCD12864����ʾASCII��ͼ��������ַ�
                    break;
                case VK_ALPHA_P:
                    LCD12864_ShowPicture(0, 0, 8, 64, (BYTE *)&LCD12864_IMG);  //��LCD12864��Ļ����ʾͼƬ
                    break;
                case VK_ALPHA_Q:
                    OLED12864_DisplayOff();            //�ر�OLED12864����ʾ
                    break;
                case VK_ALPHA_R:
                    OLED12864_DisplayOn();             //��OLED12864����ʾ
                    break;
                case VK_ALPHA_S:
                    OLED12864_DisplayContent();        //��ʾ��Ļ����
                    break;
                case VK_ALPHA_T:
                    OLED12864_DisplayEntire();         //ȫ�����������ڲ��ԣ�
                    break;
                case VK_ALPHA_U:
                    OLED12864_HorizontalMirror();      //ˮƽ������ʾ
                    break;
                case VK_ALPHA_V:
                    OLED12864_VerticalMirror();        //��ֱ������ʾ
                    break;
                case VK_ALPHA_W:
                    OLED12864_DisplayReverse();        //������ʾ
                    break;
                case VK_ALPHA_X:
                    OLED12864_SetContrast(0x10);       //����ֵ����Χ0��255��
                    break;
                case VK_ALPHA_Y:
                    OLED12864_SetContrast(0x80);       //����ֵ����Χ0��255��
                    break;
                case VK_ALPHA_Z:
                    OLED12864_SetContrast(0xf0);       //����ֵ����Χ0��255��
                    break;
                case VK_UP:
                    //����1�������������ʼҳ����Χ0��7��
                    //����2����������Ľ���ҳ����Χ0��7��
                    //����3������������ֵ�����ڵ�λΪ���룩
                    OLED12864_ScrollUp(1, 6, 200);     //���ú÷����ͨ����OLED12864_ScrollStart��ָ�ʼִ��
                    break;
                case VK_DOWN:
                    OLED12864_ScrollStart();           //��ʼִ�й�������
                    break;
                case VK_LEFT:
                    //����1�������������ʼҳ����Χ0��7��
                    //����2����������Ľ���ҳ����Χ0��7��
                    //����3������������ֵ�����ڵ�λΪ���룩
                    OLED12864_ScrollLeft(0, 2, 200);   //���ú÷����ͨ����OLED12864_ScrollStart��ָ�ʼִ��
                    break;
                case VK_RIGHT:
                    //����1�������������ʼҳ����Χ0��7��
                    //����2����������Ľ���ҳ����Χ0��7��
                    //����3������������ֵ�����ڵ�λΪ���룩
                    OLED12864_ScrollRight(0, 2, 200);  //���ú÷����ͨ����OLED12864_ScrollStart��ָ�ʼִ��
                    break;
                case VK_HOME:
                    OLED12864_SetAddressMode(0);       //Ѱַģʽ��0��ˮƽѰַ�� 1����ֱѰַ�� 2��ҳѰַ��
                    break;
                case VK_END:
                    OLED12864_SetAddressMode(1);       //Ѱַģʽ��0��ˮƽѰַ�� 1����ֱѰַ�� 2��ҳѰַ��
                    break;
                case VK_PRIOR:    //PageUp
                    OLED12864_ScrollStop();            //ִֹͣ�й�������
                    break;
                case VK_NEXT:     //PageDown
                    OLED12864_ShowPicture(0, 0, 128, 8, (BYTE *)&OLED12864_IMG);  //��OLED12864��Ļ����ʾͼƬ
                    break;
                }
            }
            else
            {
                memcpy(UsbInBuffer, UsbOutBuffer, 64);      //ԭ·����, ���ڲ���HID
                usb_IN();
            }
            usb_OUT_done();
        }
    }
}

void sys_init()
{
    WTST = 0;  //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    EAXFR = 1; //��չ�Ĵ���(XFR)����ʹ��
    CKCON = 0; //��߷���XRAM�ٶ�

    P0M1 = 0x00;   P0M0 = 0x00;   //����Ϊ׼˫���
    P1M1 = 0x00;   P1M0 = 0x00;   //����Ϊ׼˫���
    P2M1 = 0x00;   P2M0 = 0x00;   //����Ϊ׼˫���
    P3M1 = 0x00;   P3M0 = 0x00;   //����Ϊ׼˫���
    P4M1 = 0x00;   P4M0 = 0x00;   //����Ϊ׼˫���
    P5M1 = 0x00;   P5M0 = 0x00;   //����Ϊ׼˫���
    P6M1 = 0x00;   P6M0 = 0x00;   //����Ϊ׼˫���
    P7M1 = 0x00;   P7M0 = 0x00;   //����Ϊ׼˫���

    P3M0 &= ~0x03;
    P3M1 |= 0x03;

    IRC48MCR = 0x80;
    while (!(IRC48MCR & 0x01));
}
