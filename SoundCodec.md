# Modules #

  * [soundcodec.v](http://vector06cc.googlecode.com/svn/trunk/src/DE1/soundcodec.v) sound interface toplevel
  * [audio\_io.v](http://vector06cc.googlecode.com/svn/trunk/src/DE1/audio_io.v) codec interface
  * [I2C\_AV\_Config.v](http://vector06cc.googlecode.com/svn/trunk/src/DE1/I2C_AV_Config.v) audio codec configurator (C)2006 Altera Corporation
  * [I2C\_Controller.v](http://vector06cc.googlecode.com/svn/trunk/src/DE1/I2C_Controller.v) I2C interface (C)2006 Altera Corporation

# Connections and principle of operation #

`soundcodec.v` has very simple interface.
|`clk18`|in|1|18MHz master clock for the codec|
|:------|:-|:|:-------------------------------|
|`pulses`|in|4|1-bit sound streams, they're sampled into clk18 clock domain|
|`pcm`  |in|8|8-bit mono PCM stream from AY   |
|`tapein`|out|1|1-bit tape input, passed through adaptive threshold|
|`reset_n`|in|1|reset                           |
|`oAUD_XCK`|out|1|connect to AUD\_XCK             |
|`oAUD_BCK`|out|1|audio bit clock                 |
|`oAUD_DATA`|out|1|audio data                      |
|`oAUD_LRCK`|out|1|audio L/R clock                 |
|`iAUD_ADCDAT`|in|1|ADCdata in                      |
|`oAUD_ADCLRCK`|out|1|ADC L/R clock                   |

**NB1**: `AUD_XCK` must be assigned same clock as `clk18`, probably in the top level: see [associated issue](http://code.google.com/p/vector06cc/issues/detail?id=9);

**NB2**: 12MHz (256fs) clock can be used instead of 18MHz clock (384fs)

Pulses is a 4-bit wide bus. Bits 0..2 are ought to be the connected to outputs of 8253 PIT sound generator. Bit 3 is ought to be tape output. Music bits are passed through a simple moving average filter; tape out bit is unfiltered.

The other module that has to be explicitly instantiated is `I2C_AV_Config`. It comes from the DE1 samples, although the configuration bits are changed: line input is enabled and signal passthrough is disabled. The connections are self-explanatory.

After reset is reasserted high, I2C\_AV\_Config sends configuration to the codec chip and soundcodec.v starts data exchange through audio\_io.v. Line input is passed through an adaptive threshold to make loading from "tape" easier. "Music" outputs are filtered through a very simple moving averager. Parameters of adaptive thresholder and averager are found experimentally.

# 8253 Programmable Interrupt Timer #
Original Vector-06C used a 8253 chip to generate 3-channel audio. 8253.v is (an ineffective) behavioral description of 8253 PIT based on Intel datasheet. I didn't bother myself with redesigning the whole thing, this module just mimics what's enough for fair sound reproduction. Modes that can be useful for sound are faithful to the original though. Modes 1 and 5 are not implemented at all, GATE inputs are not used.