#include "mbed.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "mbed_events.h"
#include"math.h"
#include "algorithm"

#define UINT14_MAX        16383
// FXOS8700CQ I2C address
#define FXOS8700CQ_SLAVE_ADDR0 (0x1E<<1) // with pins SA0=0, SA1=0
#define FXOS8700CQ_SLAVE_ADDR1 (0x1D<<1) // with pins SA0=1, SA1=0
#define FXOS8700CQ_SLAVE_ADDR2 (0x1C<<1) // with pins SA0=0, SA1=1
#define FXOS8700CQ_SLAVE_ADDR3 (0x1F<<1) // with pins SA0=1, SA1=1
// FXOS8700CQ internal register addresses
#define FXOS8700Q_STATUS 0x00
#define FXOS8700Q_OUT_X_MSB 0x01
#define FXOS8700Q_OUT_Y_MSB 0x03
#define FXOS8700Q_OUT_Z_MSB 0x05
#define FXOS8700Q_M_OUT_X_MSB 0x33
#define FXOS8700Q_M_OUT_Y_MSB 0x35
#define FXOS8700Q_M_OUT_Z_MSB 0x37
#define FXOS8700Q_WHOAMI 0x0D
#define FXOS8700Q_XYZ_DATA_CFG 0x0E
#define FXOS8700Q_CTRL_REG1 0x2A
#define FXOS8700Q_M_CTRL_REG1 0x5B
#define FXOS8700Q_M_CTRL_REG2 0x5C
#define FXOS8700Q_WHOAMI_VAL 0xC7

DigitalOut led1(LED1);
InterruptIn sw2(SW2);
I2C i2c( PTD9,PTD8);
Serial pc(USBTX, USBRX);
int m_addr = FXOS8700CQ_SLAVE_ADDR1;

void FXOS8700CQ_readRegs(int addr, uint8_t * data, int len) {
   char t = addr;
   i2c.write(m_addr, &t, 1, true);
   i2c.read(m_addr, (char *)data, len);
}

void FXOS8700CQ_writeRegs(uint8_t * data, int len) {
   i2c.write(m_addr, (char *)data, len);
}

Thread t1;
Thread t2;

void Acc() 
{
   
   uint8_t who_am_i, data[2], res[6];
   int16_t acc16;
   float t[3];
   int i;
   float x1[100];
   float y1[100];
   float z1[100];
   float dis[100];
   float sum[100];
   float fivecm[100];

   for(i=0;i<100;i++){
        sum[i] = 0;
         
   }

   // Enable the FXOS8700Q

   FXOS8700CQ_readRegs( FXOS8700Q_CTRL_REG1, &data[1], 1);
   data[1] |= 0x01;
   data[0] = FXOS8700Q_CTRL_REG1;
   FXOS8700CQ_writeRegs(data, 2);

   // Get the slave address
   FXOS8700CQ_readRegs(FXOS8700Q_WHOAMI, &who_am_i, 1);

   for ( i = 0; i < 100; i++ ) 
   {

      FXOS8700CQ_readRegs(FXOS8700Q_OUT_X_MSB, res, 6);

      acc16 = (res[0] << 6) | (res[1] >> 2);
      if (acc16 > UINT14_MAX/2)
         acc16 -= UINT14_MAX;
      t[0] = ((float)acc16) / 4096.0f;

      acc16 = (res[2] << 6) | (res[3] >> 2);
      if (acc16 > UINT14_MAX/2)
         acc16 -= UINT14_MAX;
      t[1] = ((float)acc16) / 4096.0f;

      acc16 = (res[4] << 6) | (res[5] >> 2);
      if (acc16 > UINT14_MAX/2)
         acc16 -= UINT14_MAX;
      t[2] = ((float)acc16) / 4096.0f;

      float x = t[0], y = t[1], z = t[2];

      x1[i] = x;
      y1[i] = y;
      z1[i] = z;

      if( i>0 && x1[i] * x1[i-1] > 0 ){
          dis[i] = (x1[i] - x1[i-1]) * 9.8 * 0.5 * 0.1 * 0.1 ;
      }
      else{
          sum[i]=0;
          dis[i]=0;
      }

      wait(0.1);  
   }

    for(i=0;i<100;i++){
        sum[i] = sum[i] + dis[i];
        if(sum[i] >= 5) fivecm[i] = 1;
        else fivecm[i] = 0;
   }

   for ( i = 0; i < 100; i++) 
   {
      pc.printf("%1.4f\r\n%1.4f\r\n%1.4f\r\n%1.2f\r\n", x1[i], y1[i], z1[i], fivecm[i]);
      wait(0.01);
   }
}

void LED() 
{ 
   for ( int i = 0; i < 20; i++) {
      led1 = !led1;
      wait(0.5);
   }  
}

EventQueue queue1(32 * EVENTS_EVENT_SIZE);
EventQueue queue2(32 * EVENTS_EVENT_SIZE);

int main() 
{
   pc.baud(115200);
   led1 = 1;
   t1.start(callback(&queue1, &EventQueue::dispatch_forever));
   t2.start(callback(&queue2, &EventQueue::dispatch_forever));
   sw2.fall(queue1.event(LED));
   sw2.rise(queue2.event(Acc));
}