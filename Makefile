all: upload

upload:
	platformio run --target upload

clean:
	platformio run --target clean

build:
	platformio run

serial:
	ls -l /dev/tty.*

monitor:
	screen /dev/tty.usbserial-AH05WAKX 115200

usbasp:
	avrdude -c usbasp -p m328p -P usb -U flash:w:.pio/build/ATmega328P/firmware.hex:i -FD

direct:
	avrdude -c arduino -p m328p -U flash:w:.pio/build/ATmega328P/firmware.hex:i -P /dev/tty.usbserial-AH05WAKX

info:
	avrdude -c usbasp -p m328p -P usb -v
