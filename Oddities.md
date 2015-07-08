# MicroDOS Versions #
There are at least 3 different BIOS versions. For a casual observer they only differ in the appearance of initial message. But once you get familiarized more, they don't appear similar anymore.

One version would react to keypresses with buzz-like sounds, but the other would imitate some sort of movie computer by playing short musical notes, different for each key.

One version, let's call it "green" because I happen to have it preconfigured with green on black display, knows what it's doing well: when writing to disk, it sends exactly 1024 bytes. The other version which seems to be a little bit more robust, I call it Blue because it's preconfigured to write with yellow on dark blue, just sends however much data the controller wants, only stopping when it doesn't need data anymore.

The Blue version has some sort of weird font where it's nearly impossible to tell an angle bracket from a parenthesis. Both versions seem to have equally awful display output routines, but it's possible that they're at their limit -- it's not easy to output 6-pixel wide characters that span across column margins. I'm sure it can be helped by pre-caching the lines, but it's not very suitable for terminal output where text comes out letter-by-letter.