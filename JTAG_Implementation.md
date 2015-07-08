# DE1 JTAG Support #

## Intro ##

DE1 is supplied with a useful sw/hw combo called USB JTAG API/DE1 Control Panel. It implements
control of various DE1 peripherals. This is very convenient for debugging. For example, memory
can be preloaded or read at any time. But for the Control Panel software to be useful, matching
hardware has to be implemented.

Vector-06cc JTAG implementation is based on that supplied with the DE1 board, obviously. Since
it is desired that Control Panel software is kept without modifications, hardware side has to
adopt to all specifics and oddities of the original implementation, however unsuitable they
may seem.

JTAG implementation consists of 3 modules:
  * DE1/USB\_JTAG.v
  * DE1/CMD\_Decode.v
  * jtag/jtag\_top.v

USB\_JTAG.v is used without modifications, it's a low level serial interface that shifts JTAG commands in and out on request. It is driven by common 24MHz system clock.

CMD\_Decode.v does the rest.

jtag\_top.v is top-level interface. It has clock, reset, raw JTAG
connections, SRAM address, data in, data out, SRAM write enable, bus hold/hold acknowledge..

## vector06cc.v receptacle ##

In the top level module, follwing signals are JTAG-related:

  * SRAM\_OE\_N
> > -- common SRAM output enable output

  * mJTAG\_ADDR
  * mJTAG\_DATA\_FROM\_HOST
  * mJTAG\_DATA\_TO\_HOST
  * mJTAG\_SELECT
  * mJTAG\_SRAM\_WR\_N
> > -- these go into the main SRAM multiplexer

  * jHOLD
  * HLDA
> > -- bus hold request/acknowledge: used by CMD\_Decode to gain exclusive bus access

# Principle of operation #

The following describes principle of operation if compiled with JTAG\_AUTOHOLD option.


  * If any JTAG command is received, HOLD signal is set and timer is loaded with maximum counter value.
    * When JTAG command is coming through, timer value is decreased
    * When timer counter has reached 0, HOLD signal is released
  * When JTAG command is a SRAM read or write command, enable SRAM operation (f\_SRAM)
  * When HLDA is asserted and SRAM operation selected, the state machine is enabled

A note about the timer. Control Panel doesn't have any means of notification of a read/write operation before it actually happens. There are SRAM multiplexer selection modes and there are different device selectors, but they are only called when switching modes. As if this was not enough uncertainity, feedback to host doesn't exist and once a command starts, it has to finish. This means that usually there's no time for HLDA signal to be asserted. The timer device just tries to request the bus as early as possible and tries to hold the signal for a while. This is a hack that's not guarranteed to work but unless Control Panel software is modified to have pace, there's no better solution. Control Panel implements a small delay when reading single word, but skips it during group read/write operations.

Just in case auto bus hold device fails, the bus can be held manually.

### State Machine ###
![http://vector06cc.googlecode.com/svn/trunk/doc/jtag_statemachine.png](http://vector06cc.googlecode.com/svn/trunk/doc/jtag_statemachine.png)

## SRAM Multiplexer ##
Normally SRAM multiplexer maps (N)x8 logical memory onto (N/2)x16 physical memory space.
However in JTAG mode, access is done by words and thus jtag\_jtag (oJTAG\_SELECT) signal overrides this logic.