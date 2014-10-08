#ifndef IIC_H
#define IIC_H

/* 1ms延时函数，用于踢狗 */
void delay_10ms(void)
{
  unsigned int i;
  for (i = 0; i < 11400; i++);
}

/*---------------------------IIC读写相关函数----------------------------*/
void iic_delay(unsigned int n)
{
  int i;
  n*=10;
  for(i=0;i<n;i++);
}
/************************************************************************/
void iic_start()
{
  sda_out;
  SDA=1;
  iic_delay(1);
  SCL=1;
  iic_delay(2);
  SDA=0;
  iic_delay(2);
  SCL=0;
  iic_delay(1);
}
/************************************************************************/
void iic_stop()
{
  sda_out;
  SDA=0;
  iic_delay(1);
  SCL=1;
  iic_delay(2);
  SDA=1;
  iic_delay(2);
}
/************************************************************************/
/* 发送ACK信号   */
/************************************************************************/
void ack()
{
  sda_out;
  SDA=0;
  SCL=0;
  iic_delay(2);
  SCL=1;
  iic_delay(2);
  SCL=0;
}
/************************************************************************/
/* 发送NOACK信号 */
/************************************************************************/
void iic_noack()
{
  sda_out;
  SDA=1;
  SCL=0;
  iic_delay(2);
  SCL=1;
  iic_delay(2);
  SCL=0;
}
/********************************************************************
等待ACK信号       
***********************************************************************/
void iic_wait_ack()
{
  int a=0;
  unsigned int tt=1000;
  sda_in;
  SCL=0;
  iic_delay(2);
  SCL=1;
  iic_delay(2);
  
  while(PINC_Bit1&&tt--){
    if(tt==1)
      a=1;
  };
  if(a==1)
  {
    iic_stop();
    IICSTOP=1;
  }
  SCL=0;
}
/********************************************************************
向iic设备发送一个字节 
***********************************************************************/
void iic_send_byte(unsigned char sbyte)
{
  unsigned char i,CY;
  sda_out;

  for(i=0;i<8;i++)
  {	
  	CY=sbyte>>(7-i);
    SCL = 0;
    iic_delay(1);
    SDA = CY;
    iic_delay(1);
    SCL = 1;
    iic_delay(1);
  }
	SCL=0;
}
/********************************************************************
MCU从iic设备接收一个字节       
***********************************************************************/
unsigned char iic_receive_byte()
{
  unsigned char i;
  unsigned char ddata=0;
  
  sda_in;
  for(i=0;i<8;i++)
  {
    ddata <<= 1;
    SCL=0;
    iic_delay(2);
    SCL=1;
    iic_delay(1);
    ddata |= PINC_Bit1;
  }
  SCL=0;
  return ddata;
}
/********************************************************************
向iic设备写入n个字节
说明：write_data,写入字节
      Address,从address处开始写入      
***********************************************************************/
void AT24C64_W(unsigned char *data, unsigned int address,unsigned char n)
{
  unsigned char i;
  unsigned char *p;
   _CLI();//写之前关中断
  p=data;
  if(n>32) n=32; //一次最多写入32个字节
  iic_start();
  iic_send_byte(0xa0);		  //器件地址
  iic_wait_ack();
  if(IICSTOP) return;
  iic_send_byte((unsigned char)(address>>8));   //传高8位地址
  iic_wait_ack(); 
  if(IICSTOP) return;
  iic_send_byte((unsigned char)(address & 0xff));  //传低8位地址
  iic_wait_ack();
  if(IICSTOP) return;
  for(i=0;i<n;i++)
  {
    iic_send_byte(*p);
    iic_wait_ack();
    if(IICSTOP) return;
    p++;
  }
  iic_stop();
  _SEI();//开中断
    delay_10ms();
}
/********************************************************************
从iic设备读取n个字节
说明：read_data ,存放字节的数组 ;
address,指定从address处开始读取 
num,读取的字节数      
***********************************************************************/
void AT24C64_R(unsigned char *data, unsigned int address, unsigned char n)
{
  unsigned char i;
  unsigned char *p;
  p=data;
  iic_start();
  iic_send_byte(0xa0);
  iic_wait_ack();
  if(IICSTOP) return;
  iic_send_byte((unsigned char)(address>>8));
  iic_wait_ack();
  if(IICSTOP) return;
  iic_send_byte((unsigned char)(address & 0xff));
  iic_wait_ack();
  if(IICSTOP) return;
  iic_start();
  iic_send_byte(0xa1);
  iic_wait_ack();	
  if(IICSTOP) return;

  for (i=0; i<n; i++)
  {
     *p = iic_receive_byte();
   if (i <(n - 1))     
    {
      ack();
    }
   p++;
  }
  iic_noack();
  iic_stop();
  iic_delay(5);  
  for(i=0;i<10;i++)
    delay_10ms();
               WDI=0;
	       delay_10ms();
	       WDI=1;
}

#endif