st2_talkie
==========

Talking firmware for the [Solder Time II watch](http://spikenzielabs.com/SpikenzieLabs/SolderTime_2.html)

This firmware is based on the standard firmware, with the following extra features:
*   More rounded numeric font
*   Speech synthesis of the time

The Solder Time II doesn't have a very speech oriented audio system, so you'll probably need to hold it to your ear to hear anything. But it does work!

The speech engine is derived from the Texas Instruments speech chips of the 1980's (eg. Speak'n'Spell), similar to the version implemented in my Talkie library. As the Solder Time II uses an 8MHz clock, there isn't enough CPU time to do the full synthesis algorithm, so I dropped the calculation precision in some places. This seems to give enough of a speedup without dropping speech quality too much.

