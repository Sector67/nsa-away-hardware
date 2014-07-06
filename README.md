nsa-away-hardware
=================

Random number generator that fills out two SD cards, dumps to a HID keyboard, or decrypts data.

Block diagram:

LCD - displays progress, function
Microcontroller - writes to USB/SD
SD card slots (2)
USB - optional output
switch - on/off
two buttons (mode and start)
	- create SD clones
	- copy SD card
	- random->usb keyboard
	- usb keyboard
RNG * 8? 16?
Power supply: battery or USB


Purpose:

There are 5 possible uses for this device:

1) Create two identical SD cards that contain the same OTP with the same random data. These SD cards are then distributed to the two people who need to communicate, and they have a source of secure shared random data.

2) Copy an SD card. If an OTP needs to be transferred to a third party, or the second party loses their key, this device can clone an existing SD card.

3) Random number generator over USB. It is sometimes necessary to have a random number generator for other purposes. This generates random data and pipes it out over USB as an HID keyboard. It does this so that it won't allow traffic IN to the machine, potentially compromising it.

4) USB Keyboard for translating. When a message is sent, the user needs to be able to encrypt or decrypt it. This allows the user to plug in the device and consume the random data from the OTP.

5) In a pinch, the device could be used to decrypt when a USB connection isn't available. In this mode, the user specifies the offset and starts reading characters, which are displayed one by one. Time consuming, but it works.

Theory of operation:

The device uses two transistors with their bases touching to create avalance noise. This noise is amplified and sent to a multiplexor. 8 of these noise circuits together are used to generate a byte of data. The data is then de-biased and whitened, and passed appropriately depending on the mode.

Security:

There are a few potential vectors for attack. The first and most obvious is to compromise the microcontroller. It is our belief that in large volumes a custom IC could be designed to prevent this from happening. In the short term, we can burn the fuses on the microcontroller to prevent it from being overwritten.

UI:

A simple LCD based on an HD44780 is sufficient. This display would allow the user to change modes, and could possibly be used to read and translate data in a pinch.

Back button allows the user to go to a previous screen.

Home Screen:
	Shows if the battery is charging and what its status is.
	Up/Down arrows let the user choose between modes. If no USB is detected, then it will not show USB-required modes.
	Enter button to go into a mode.

Create SD Clones:
	Up/Down to select data size
	Enter button to start. Doesn't start immediately but rather asks to verify that both cards will be erased and won't work in computers.
	While running, shows % complete, Enter button to stop.
	When complete, says "complete."

Copy SD card:
	Always copies from one side to the other. No option to swap (too confusing and dangerous).
	Enter to start. Doesn't start immediately but rather asks the user to verify the original is in the correct slot.
	Enter again to start.
	Once complete, says "Complete".

Random->USB Keyboard:
	Up/Down arrows to select amount (stream, 1 byte, 8 bytes, 256 bytes, 1kb, 512kb, 1mb, 10mb, 128mb, 1gb.)
	Enter to have it spit out that much. % complete to show progress.
	Back to prematurely cancel.

SD->USB Keyboard:
	Up/Down/Enter to select each digit of the offset. Enter on Blank to complete offset.
	Every enter pressed thereafter shows offset and in parentheses character count for that session and the character for that offset.
	Every passed character gets zeroed out on the SD card so that it is lost forever.
	Back to escape to the home screen.

SD->Display:
	Same as SD->USB Keyboard, except that it doesn't try to pump the result out over USB. Just for the display.
