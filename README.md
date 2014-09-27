nsa-away-hardware
=================

Random number generator that fills out two SD cards, dumps to a HID keyboard, or decrypts data.

<h2>Block diagram</h2>
<ul>
<li>LCD - displays progress, function</li>
<li>Microcontroller - writes to USB/SD</li>
<li>SD card slots (2)</li>
<li>USB - optional output</li>
<li>switch - on/off</li>
<li>two buttons (mode and start)
<ul>
	<li>create SD clones</li>
	<li>copy SD card</li>
	<li>random->usb keyboard</li>
	<li>usb keyboard</li>
</ul></li>
<li>RNG * 8</li>
<li>Power supply: battery or USB</li>
</ul>

<h2>Purpose</h2>

There are 6 possible uses for this device:

<ol>
<li>Create two identical SD cards that contain the same OTP with the same random data. These SD cards are then distributed to the two people who need to communicate, and they have a source of secure shared random data.</li>
<li>Copy an SD card. If an OTP needs to be transferred to a third party, or the second party loses their key, this device can clone an existing SD card.</li>
<li>Random number generator over USB. It is sometimes necessary to have a random number generator for other purposes. This generates random data and pipes it out over USB as an HID keyboard. It does this so that it won't allow traffic IN to the machine, potentially compromising it.</li>
<li>USB Keyboard for translating. When a message is sent, the user needs to be able to encrypt or decrypt it. This allows the user to plug in the device and consume the random data from the OTP.</li>
<li>In a pinch, the device could be used to decrypt when a USB connection isn't available. In this mode, the user specifies the offset and starts reading characters, which are displayed one by one. Time consuming, but it works.</li>
<li>Serial port to keyboard. In this mode, anything spit out over the USB serial port (FTDI) gets printed to the keyboard. This allows you to connect a secure device to an unsecure device without the need for drivers and without exposing the secure device.</li>
</ol>
<h2>Theory of operation</h2>

The device uses two transistors with their bases touching to create avalance noise. This noise is amplified and sent to a multiplexor. 8 of these noise circuits together are used to generate a byte of data. The data is then de-biased and whitened, and passed appropriately depending on the mode.

<h2>Security</h2>

There are a few potential vectors for attack. The first and most obvious is to compromise the microcontroller. It is our belief that in large volumes a custom IC could be designed to prevent this from happening. In the short term, we can burn the fuses on the microcontroller to prevent it from being overwritten.

<h2>UI</h2>

A simple LCD based on an HD44780 is sufficient. This display would allow the user to change modes, and could possibly be used to read and translate data in a pinch.

Back button allows the user to go to a previous screen.

<h3>Home Screen</h3>
<ul>
<li>Shows if the battery is charging and what its status is.</li>
<li>Up/Down arrows let the user choose between modes. If no USB is detected, then it will not show USB-required modes.</li>
<li>Enter button to go into a mode.</li>
</ul>

<h3>Create SD Clones</h3>
<ul>
<li>Up/Down to select data size</li>
<li>Enter button to start. Doesn't start immediately but rather asks to verify that both cards will be erased and won't work in computers.</li>
<li>While running, shows % complete, Enter button to stop.</li>
<li>When complete, says "complete."</li>
</ul>

<h3>Copy SD card</h3>
<ul>
<li>Always copies from one side to the other. No option to swap (too confusing and dangerous).</li>
<li>Enter to start. Doesn't start immediately but rather asks the user to verify the original is in the correct slot.</li>
<li>Enter again to start.</li>
<li>Once complete, says "Complete".</li>
</ul>

<h3>Random->USB Keyboard</h3>
<ul>
<li>Up/Down arrows to select amount (stream, 1 byte, 8 bytes, 256 bytes, 1kb, 512kb, 1mb, 10mb, 128mb, 1gb.)</li>
<li>Enter to have it spit out that much. % complete to show progress.</li>
<li>Back to prematurely cancel.</li>
</ul>

<h3>SD->USB Keyboard</h3>
<ul>
<li>Up/Down/Enter to select each digit of the offset. Enter on Blank to complete offset.</li>
<li>Every enter pressed thereafter shows offset and in parentheses character count for that session and the character for that offset.</li>
<li>Every passed character gets zeroed out on the SD card so that it is lost forever.</li>
<li>Back to escape to the home screen.</li>
</ul>

<h3>SD->Display</h3>
<ul>
<li>Same as SD->USB Keyboard, except that it doesn't try to pump the result out over USB. Just for the display.</li>
</ul>
