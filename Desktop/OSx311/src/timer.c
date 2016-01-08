#include "timer.h"
#include "isr.h"
#include "monitor.h"

#define RTCaddress 0x70
#define RTCdata 0x71
// http://docs.huihoo.com/help-pc/hw-CMOS_RAM.html aldım doğrudur heralde ?
u32int tick = 0;
u8int currentTime[5]; //hh:dd:ss
static void timer_getTime()
{
    u8int tvalue,index,regB;
    tick++;
    outb(RTCaddress,0xb);
    regB=inb(RTCdata);
    regB = regB | 0x06;
    outb(RTCaddress,0x0B);
    outb(RTCdata,regB);
    if (tick % 54 == 0) //Her 54 tickte bir çalışacak
    {
	     for(index=0;index<5;index++)
	     {
		         asm volatile("cli"); // Interruptları kapadık kritik yer burda interrupt falan gelmemesi lazım
		         outb(RTCaddress,index);
		         tvalue=inb(RTCdata);
		         asm volatile("sti"); // Interruptlar tekrar aktif artık datayı aldık RTCden (8253'ten)
		         currentTime[index]=(tvalue%10)+48;
		         index++;
		         currentTime[index]=(tvalue/10)+48;
	    }
    }
}

static void timer_callback(registers_t regs)
{
    int index;
    timer_getTime();
    monitor_setTempCursorLocation(71,0); // Sağ kenara doğru git ama en dibede girme
    for(index=5;index!=0;index--)
	  {
		  monitor_put_with_colors(currentTime[index],0x0F,0); // beyaz arkaplan siyah sayı
		  if(index%2==0) {
		  	monitor_put_with_colors(':',0x0F,0);
      }
	  }
    monitor_put_with_colors(currentTime[index],0x0F,0);
    monitor_getBacktoTemp(); // nerede kaldıysak oraya geri dön

}
void setTimeAtStart() // bu saati 00:00:00 yapıyor , rebootta veya başlangıçta çağırıyoruz
{
  u8int index,regB;
	u32int i;
	outb(RTCaddress,0xb);
	regB=inb(RTCdata);
	regB = regB | 0x06;
	outb(RTCaddress,0x0B);
	outb(RTCdata,regB);
	asm volatile("cli");
	outb(RTCaddress,0);
	outb(RTCdata,0);
	outb(RTCaddress,2);
	outb(RTCdata,0);
	outb(RTCaddress,4);
	outb(RTCdata,0);
  asm volatile("sti");
}
void setTime(u8int h,u8int m, u8int s) // Bu sağlam bi fonksiyon hem kendi saatimizi(üstte gözüken hemde işte RTCdeki
{ //saatimizi düzenliyor outB RTCler falan ondan var yoksa bi sonraki okumada yine eski saate geri dönecektik
	u8int index,regB;
	u32int i;

	outb(RTCaddress,0xb);
	regB=inb(RTCdata);
	regB = regB | 0x06;
	outb(RTCaddress,0x0B);
	outb(RTCdata,regB);
	asm volatile("cli");
	outb(RTCaddress,0);
	outb(RTCdata,s);
	outb(RTCaddress,2);
	outb(RTCdata,m);
	outb(RTCaddress,4);
	outb(RTCdata,h);
	asm volatile("sti");
	monitor_write("Time has been set to: ");
	monitor_put(h/10+48);
	monitor_put(h%10+48);
	monitor_put(':');
	monitor_put(m/10+48);
	monitor_put(m%10+48);
	monitor_put(':');
	monitor_put(s/10+48);
	monitor_put(s%10+48);
  monitor_put('\n');
}

void init_timer(u32int frequency)
{
    register_interrupt_handler(IRQ0, &timer_callback);

    u32int divisor = 1193180 / frequency;

    outb(0x43, 0x36);

    u8int l = (u8int)(divisor & 0xFF);
    u8int h = (u8int)( (divisor>>8) & 0xFF );

    outb(0x40, l);
    outb(0x40, h);
}
