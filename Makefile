### Makefile.tpl -
##
## Author: Karl Ljungkvist

BOARD_TAG = nano328
BOARD_ID = arduino:avr:nano
# ARDUINO_LIBS = <space separated list of libs, arduino.mk will try to guess>
MONITOR_PORT = /dev/ttyUSB0
SKETCH=eeprom_programmer.ino



default:
	arduino -v --pref build.path=build-$(BOARD_TAG) --board $(BOARD_ID) --port $(MONITOR_PORT) --upload  $(SKETCH)


### Makefile ends here
