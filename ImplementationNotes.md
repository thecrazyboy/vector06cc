This page contains constantly updated project notes. Also see ArchitecturalOverview.

# Contents #


# Clock Distribution #

![http://vector06cc.googlecode.com/svn/trunk/doc/clocks-2.png](http://vector06cc.googlecode.com/svn/trunk/doc/clocks-2.png)

Clock distribution:
  * clk24 is the master clock, also VGA pixel clock
  * ce12 (12MHz) is 512 pixel mode pixel clock (PAL)
  * ce6 (6MHz) is the pixel/ram address clock
  * ce3 (3MHz) this is the main CPU clock enable
  * video\_slice is high when video adapter wants RAM
  * timer\_vi53\_ce is the slow 1.5MHz clock enable for 8253 soundnik


---

# Keyboard #
In this implementation an attempt is made to make the keyboard as accessible as possible to the modern user, while keeping the impact on compatibility minimal. A complex interface maps PS/2 keyboard codes into Vector-06C keyboard matrix, while keeping track of both PS/2 and Vector Shift status. This results in smooth interaction with modern keyboards, the user doesn't need to know where certain characters were located on the original keyboard at all. One drawback is Cyrillic keyboard layout, which is currently not mapped out. This may also cause problems in programs that assume the keyboard geometry to be fixed, e.g. piano keys simulators. Such functionality can be added: see [related issue](http://code.google.com/p/vector06cc/issues/detail?id=10).

Keyboard interface consists of low level PS/2 driver located in ps2k.v, scancode converter roms in scan2matrix.v and toplevel interface with matrix encoder in vectorkeys.v.

PS/2 driver, `ps2k.v` is very basic and provides no possibility to send commands to the keyboard, e.g. to set typematic rate or change the status of keyboard LED's. The main module is `vectorkeys.v`. The state machine there handles make and release codes, while keeping track on natural, forced and negated shifts. This is necessary to properly implement keys that have different shift status on different keyboards, e.g. to enter a ':' one presses ':' key on the original Vector without Shift, while on a PS/2 one normally presses Shift+; to enter a ':'. Thus, a shift has to be neglected from the keyboard matrix. Same ':' key with shift on Vector would enter an asterisk, '`*`'. On a PS/2 keyboard, asterisk is entered by pressing Shift+8, thus the shift status is kept natural for "Shift+8". Other interesting case of shift handling is dictated by the PS/2 keyboard itself which sends make shift code before each grey arrow keypress and break shift code after it's released.

The matrix itself is simulated rather physically, see `rowbits` assignment in `vectorkeys.v`. This ensures proper emulation but, of course, is far from being compact or elegant. Ideally, keymatrix should be made a RAM block and rowbits would then be updated sequentially. Unfortunately, to make this possible, the main state machine with all logic would have to be slightly rewritten.

Special keys on the keyboard:
|**F11**|БЛК+ВВОД|сброс ПК с подключением ПЗУ загрузчика. Программа в загрузчике отслеживает состояние клавиш при старте и удерживая F1, F2, F1+F2 можно принудительно выбирать загрузку с кассеты, квазидиска или дискеты|
|:------|:-------|:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
|**F12**|БЛК+СБР |запуск загруженной программы. При нажатии на эту клавишу, отключается ПЗУ загрузчика и осуществляется сброс процессора, который начинает исполнять программу в ОЗУ с адреса $0000                       |
|**ScrollLock**|OSD Menu|нажатие на эту клавишу вызывает экранное меню из которого можно выбрать образ дискеты (DISK), сбросить ПК (RESET), перезапустить программу (RESTART), захватить шину для JTAG интерфейса/приостановить работу (HOLD).|
|**CapsLock**|РУС/ЛАТ |Переключение языка ввода в некоторых программах; см. красный светодиод #9                                                                                                                               |
|**Control**|УC      |Ввод управляющих символов                                                                                                                                                                               |
|**Shift**|CC      |Смена регистра                                                                                                                                                                                          |
|**Alt**|ПС      |Перевод строки                                                                                                                                                                                          |
|**Del**|СТР     |Не знаю                                                                                                                                                                                                 |

See also: [Scan Codes Demystified](http://www.quadibloc.com/comp/scan.htm)<sub>external</sub>

See also: [Interfacing the AT keyboard](http://www.beyondlogic.org/keyboard/keybrd.htm)<sub>external</sub>

See also: [Техническое описание](Technical_Description.md)


---

# Mock Disk #
_Aka kvaz, квазидиск_.

Same SRAM chip is used for main memory and for the RAM disk. The complete memory map can be laid out approximately like this (addresses given in byte mode, divide by 2 for physical figures):
|00000|Main RAM|
|:----|:-------|
|10000|Ramdisk page 0|
|20000|Ramdisk page 1|
|30000|Ramdisk page 2|
|40000|Ramdisk page 3|

Important signals in the toplevel:
  * `ramdisk_control_write` ramdisk control register select, port $10 (write-only)
  * `io_stack` stack I/O selector
  * `ramdisk_page` bits [16:15] of external SRAM bus, fed into `sram_map`

`kvaz` is basically just another level of SRAM multiplexer. Its mode of operation is controlled by the ramdisk control register. Depending on configuration, it would select SRAM page, bits [16:15] of external SRAM address bus, during regular memory access (in window mode) or stack access (in stack mode). Output `ramdisk_page` is then fed through to `sram_map` module where it's joined into the complete SRAM address.

See also: [Описание функционирования квазидиска](ramdisk.md), [JTAG\_Implementation](JTAG_Implementation.md).


---

# Floppy Drive #
_This section describes inner structure of floppy drive emulator. For information about using it, see [HOWTO\_Floppy](HOWTO_Floppy.md)._

## Theory ##
### Overview ###
The floppy drive system is implemented as a separate entity based on a 6502 CPU. All used memory (see breakdown below) fits within existing M4K blocks of a Cyclone II. According to Quartus reports, total of 86% of memory bits are used.

The code that lives in the 6502 unit is written in C and compiled by [cc65](http://www.cc65.org) compiler. C language library, crt0.s and linker configuration file, vector.lc, are provided. Architecture is mostly defined by vector.lc file which contains memory map and specialio.h, which defines I/O locations. It seems that for succesful operation, memory size 1 byte smaller than real should be specified -- ~~must~~ could be investigated.

FAT support is based on Elm-Chan's awesome [Generic FAT Filesystem module](http://elm-chan.org/fsw/ff/00index_e.html). This module is the best FAT implementation can ever wish for, it provides most everything right out of the box. However, being exceptionally robust, it's also not tiny.

### WD1793 ###
Some information about WD1793:
  * [MSX Info Pages of WD1793](http://msx.retro8bits.com/msxwd1793.html)<sub>external</sub>
  * [EMULib has C-source of the WD1793 emulator](http://svn.akop.org/psp/trunk/fms/EMULib/)<sub>external</sub>
  * [ELM: How to MMC/SD](http://elm-chan.org/docs/mmc/mmc_e.html)<sub>external</sub>
  * [Generic FAT Filesystem module](http://elm-chan.org/fsw/ff/00index_e.html)<sub>external</sub>
Also see Technical\_Description for Vector-06C port map, [Секреты Вектора от Лебедева](VectorSecrets_by_Lebedev.md) and [Секреты Вектора и Кристы](SVK1_Floppy.md)

## Practice ##

-
### FDC Memory Map ###
|0000-01FF|512|`lowmem_en`|Zeropage and 6502 stack|
|:--------|:--|:----------|:----------------------|
|0200-05FF|1024|`bufmem_en`|Disk exchange buffer   |
|0800-(8000)|18K|`rammem_en`|General purpose RAM    |
|E000-E0FF|   |`ioports_en`|I/O area               |
|E100-E1FF|256|`osd_en`   |Display RAM            |

-
### CPU I/O Ports ###
Ports start at memory location $E000:
|E000|MMC\_A: bit0 = SD\_DAT3/CS|
|:---|:-------------------------|
|E001|SPDR: SPI data register, same as in AVR|
|E002|SPSR: read: SPI status register: bit 0=SPIF; write: bits [6:4] number of 512-byte blocks to read, initiate transfer; bit 7=write/read|
|E003|OSD Joystick: {0,0,0,Right,Left,Up,Down,Fire}|
|E004|TxD                       |
|E005|RxD (not implemented)     |
|E006|SERIAL\_CTL (status, bit 0 == busy)|
|E007|10mS timer/counter 1      |
|E008|10mS timer/counter 2      |
|E009|CPU\_REQUEST (bit 0 == side) request port, polled|
|E00A|CPU\_STATUS (write-only) return status here|
|E00B|TRACK (read-only) track index from the hardware|
|EOOC|SECTOR (read-only) sector index from the hardware|
|EOOE|DMA buffer start MSB      |
|EOOF|DMA buffer start LSB      |
|E010|GLEDS green leds (wo)     |
|E011|OSD control (wo): {0,0,0,0,0,F11,F12,bus hold}|

-
### CPU Requests ###
|`CPU_REQUEST_READ` | 8'h10 | Request to read one sector at `oTRACK`, `oSECTOR`, bit1=drive,bit0=side (head #) |
|:------------------|:------|:---------------------------------------------------------------------------------|
|`CPU_REQUEST_WRITE`|8'h20  |  Request to write one sector at `oTRACK`, `oSECTOR`; bit1=drive,bit0=side (head #) |
|`CPU_REQUEST_READADDR` | 8'h30 | Request to read sector address (return 6 byte header),bit 1=drive,bit0=side      |
|`CPU_REQUEST_NOP`  |8'h40  | A time waster                                                                    |
|`CPU_REQUEST_ACK`  | 8'h80 | Acknowledge: clear CPU status bits                                               |

After CPU has done with processing any request other than `CPU_REQUEST_ACK`, it sets two bits in `iCPU_STATUS`:
  * bit 0 == 1: operation complete
  * bit 1 == 1: operation was successful
  * bit 3 == 1: drive not ready (SD card pulled out)

The host then enters `CPU_ENDCOMMAND` state, which issues a `CPU_REQUEST_ACK` command, with LSB containing the _boo code_. The CPU must react by clearing its status register. After the CPU has cleared its status, the machine is switched back into `STATE_READY`.

### Small Things ###
One serious behavioural difference between the emulator and the real thing: normally in the write sector mode, BUSY flag is cleared almost instantaneously after the last byte is fed to the controller. Here a write op transfers data to the RAM buffer first and some (relatively long) time is required to flush the buffer before the next operation can begin. But software may expect BUSY bit to be cleared on a really short notice. Hence, after DMA transfer is over, the BUSY bit is cleared but the state changes to `STATE_WAIT_CPUWRITE`. The software may send new requests then, but next request will only be processed after the state changes to `STATE_READY` again. This seems to be the only possible compromise that works with both versions of MicroDOS.

However, if the software sends another command before the first one started to be processed, the machine will enter `STATE_DEAD` with all error bits set, `oCPU_REQUEST` set to `CPU_REQUEST_FAIL`, sector register will contain the last written command, the track will contain 0, 0 DRQ, BUSY in the MSB and the last machine state in LSB. wtf line will be set high, which in turn should lock the host computer CPU forever in the vicinity of the offending instruction. A crude, but useful debugging measure.

### WD1793 State Diagram ###
![http://vector06cc.googlecode.com/svn/trunk/doc/wd_statemachine.png](http://vector06cc.googlecode.com/svn/trunk/doc/wd_statemachine.png)

Notes:
  * `ABORT` state not shown for clarity. Any state has a transition to `ABORT` thanks to $D0 command and it exits into `ENDCOMMAND` state.
  * `DEAD` state not shown for clarity.

---


# OSD Menu #
OSD Menu is a cooperative effort of a secondary hardware display and the floppy workhorse CPU.

## Display ##
The display implements a hardware text mode: 5x7 characters with 1-pixel padding. Bit 7 of character code is the inversion attribute. Only characters between $20-$60 exist in the character generator, although characters up to $80 can be added at the expense of some M4K. The character generator ROM is M4K-based, organized in 5-bit wide words. ASCII codes starting with $20 are mapped to 0x00 internally.

Video RAM is 256x8 dual-port RAM located in M4K. It is mapped into CPU memory space at addresses $E100-$E200. The screen configuration is 8 lines of 32 characters each.

When OSD is active, its display is overlaid on top of the primary display. High bits of the primary display true colour components are shifted to the LSB of RGB signals, thus providing simple transparency effect.

## Input ##
When ScrollLock key is pressed, "not in matrix" bit is forced for all keys in the keyboard driver. Instead, Arrow keys and the Enter key start toggling corresponding bits in the OSD joystick ($E003) register.


---

# Sound #
See [SoundCodec](SoundCodec.md)