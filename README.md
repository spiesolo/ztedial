# ztedial

I brought a ZTE 4G LTE MF832s USB dongle and it works fine on Windows with ZTE provided
dial program, however it seems not working very well in Linux desktop (Ubuntu) with ModemManger
or wvdial. And this drives me to write this simple app in order to use it under Linux.

ztedial reuse build files from wvdial and libwvstreams for modem communication. libudev is also
included for hotplug event, it runs as a system service in the backend and dails to internet
on detection of ZTE MF832s.

To use this app:
* usb-modeswitch (>= 2.2.5) is required to be installed
* Need to blacklist this device in ModemManager
