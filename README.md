# neopixel_frame
An interactive Neopixel lit frame.

This code is running on an Adafruit ESP Feather with the OLED HAT and the VL53LOX laser range finder.
The frame has a RGBW Neopixel string running around the inside of the frame illuninating the "art" or 
whatever is in the frame.  If the sensor doesn't see anything nearby, it just lights the white LEDs in
the string.  If it detects an object, it uses the distance to change the hue of the LEDs.

There is also a Web server that a user can connect to via the code to make this an Wireless Access Point.
There are buttons (and eventually sliders) that can change the colors and brightness of the frame.

Libraries needed:
# foo
# bar
# foo
