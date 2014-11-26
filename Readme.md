Sensible-CVSB983
================

The CVSB983 is a cheap USB IR remote that simulates a keyboard and mouse.
Unfortunately there are a couple of problems with the keyboard implementation
not only do multiple keystrokes get presented but sometimes the same key in a 
series of keystrokes can be presented leading to confusing and strange
behaviour. (For example, the Rewind button reports a Left, then adds Ctrl and
Shift. It then releases the left key, before adding in R.)

This software uses EVIOGRAB to grab and hold the input event device associated
with this device, interprets the keystrokes and then creates keystrokes on a 
virtual device created with udev and uinput.

In the my case the usb device has the id: 1d57_ad02 and this is hardwired into
code. You may need to adjust device address for your device.
