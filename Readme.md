Sensible-CVSB983
================

The CVSB983 is a cheap USB IR remote that simulates a keyboard and mouse.
Unfortunately there are a couple of problems with the keyboard implementation,
not only do multiple keystrokes get presented but sometimes the same key in a 
series of keystrokes can be presented leading to confusing and strange
behaviour. (For example, the Rewind button reports a Left, then adds Ctrl and
Shift. It then releases the left key, before adding in R. This means that Rewind
and Left can easily be confused.)

This software uses EVIOGRAB to grab and hold the input event device associated
with this device, interprets the keystrokes and then creates keystrokes on a 
virtual device created with udev and uinput.

In my case the usb device has the id: 1d57_ad02 and this is hardwired into
code. You may need to adjust device address for your device.

An example of a product that code was built for is:

http://www.amazon.co.uk/gp/product/B00DSKA6BW

Installing on the Raspberry Pi (and Pi 2!)
==========================================

You will need to install libudev-dev:

```
:; sudo apt-get install libudev-dev
```

You'll also need to have a basic build-environment:

```
:; sudo apt-get install build-essential
```

Once you have these change in to Sensible-CVSB983 directory and use make
to build.

As Kodi/XBMC grabs the event devices, you need to start Sensible-CVSB983 before it.

```
:; ### Raspbmc uses upstart, so to get this to start on startup you will need to:
:; sudo cp sensible-cvsb983.conf /etc/init 
```

```
:; ### OSMC uses systemd, so to get this to start on startup you will need to:
:; sudo cp sensible-cvsb983.service /etc/systemd/system/multi-user.target.wants/
```

Then restart the Pi and you should get a more sensible behaving remote.

Keycodes emitted for each Key
=============================

I have chosen to emit different keys for the special keys - mostly because I 
use a harmony remote. Admittedly some of these choices may seem non-sensible
but looking at the code it should be easy a downstream user to change these to
more appropriately suit their use. In any case the below are my choices:

| Key | Mapping|
|-----|--------|
| *POWER*    | Seems rather intermittant and I was unable to get any sensible behaviour out of this I wouldn't be surprised if it was being sent down the mouse event
| *EMAIL*        | Normally would have sent a KEY_EMAIL, however, I have chosen to send a KEY_C 
| *WWW*          | Normally would have sent a KEY_HOMEPAGE. I have chosen to send KEY_I
| *CLOSE*        | `KEY_M`
| *GREEN (A)*    | `KEY_RED`
| *ORANGE (B)*   | `KEY_GREEN`
| *BLUE (C)*     | `KEY_YELLOW`
| *YELLOW (D)*   | `KEY_BLUE`
|                | To match the harmony names for these keys and to match the normal layout for colours
| *Switch Windows* | `KEY_Q`
| *Folder*       | `KEY_O`
| *Full Screen*  | `KEY_HOME`
| *Desktop*      |       `KEY_END`
| *FastForward*  | `KEY_F`
| *Rewind*       | `KEY_R`

The numlock key doesn't actually send a code - it just changes the keys the usb device sends. As I do not use this
I have not dealt with this case. You would need to adjust the parser code to account for the reported keys and either
pass them through or change them. Remember to add any key codes emitted to the set_key_bits function.

