#include "monitor.h"
#include "descriptor_tables.h"
#include "timer.h"
#include "keyboard.h"

int main(struct multiboot *mboot_ptr)
{
    init_descriptor_tables();
    setTimeAtStart();
    monitor_clear();
    //MONITORUN DEFAULT HALI beyaz yazı siyah arka plan , bunun haricinde bişey yazdırmak için
    //monitor_color_write("string",arkaplan,yazırengi); kullanılmalıdır.
    monitor_welcome_message();


    //monitor_write("Calisiyor.\n");
    //monitor_write_dec(10);

    //asm volatile("int $0x3");
    //asm volatile("int $0x4");
    asm volatile("sti");
    init_keyboard();
    init_timer(85);

    return 0xDEADBABA;
}
