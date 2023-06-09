# CharlieBoy

A hand-held gaming console / Arduino experimenters kit.

![A1CyZcg](https://github.com/Viper-7/CharlieBoy/assets/39548/a49645fa-029b-4b7b-8a56-e1dfed359e6a)

Just yesterday I learned I have a nephew who is turning 11 next week, with interests in computers, gaming, and art. This was my 24 hour project to make a gift for him, as a toy and hopefully a gateway into the wide world of electronics.

As it comes it's a Tetris game, simple but surprisingly addictive, I caught myself getting carried away playing it more than a few times while testing 😁 The code for it is in this repository. The LCD is an older ILI9328 shield I had lying around, and I couldn't find many finished game projects for it, so I ported this one from https://github.com/vilaca/Handheld-Color-Console/tree/master/ArduinoTetris

![image](https://github.com/Viper-7/CharlieBoy/assets/39548/e8e68817-b949-4a25-9b81-8d90739ed7bc)

However the real trick is the fact that the unit itself is only held together by the pins on the shield - you can literally just rip the thing apart and start hacking. Once the cover is off there's an Arduino Uno, fully available and ready for breadboarding, with a lithium battery and charger hidden beneath it, and a touchscreen, joystick with button, and power switch for user interface.

![C0damcI](https://github.com/Viper-7/CharlieBoy/assets/39548/842e7cdc-4b6d-4751-99ef-a3462cb1dc02)

It's intended to be a platform to start learning electronics and microcontroller development. It's fully customizable, the game on it can be changed or replaced, they can add buttons, dials, lights, etc to it, use the Touchscreen and SD card reader to show photos or art, etc, etc.

The Arduino and Joystick are bolted down, with brass inserts melted into the 3d print. The switch is held in with a screw on collar, and the LCD (both the shield on the inside and the display on the outside) are attached with VHB tape.

![image](https://github.com/Viper-7/CharlieBoy/assets/39548/7be7ba5c-bce1-46e6-84d3-8fe2a0972614)

Hiding under the Arduino is a TP4056 + DW01 lithium charger + protection module, and a small 500mAh 1S LiPo battery, along with a diode and 2A PTC fuse so any mishaps are a little less dramatic. The joystick is wired to A6 and A7, which are not available on most Arduino Uno boards, but were broken out on this board so I used those to avoid occupying any "regular" pins. With the screen removed, only the switch inside the joystick shares with a regular arduino pin, A5.

![image](https://github.com/Viper-7/CharlieBoy/assets/39548/cb1ef1c9-8702-4ec9-b291-0b0203c87d91)

Total parts cost was under $20, although shipping may vary. I've included the OpenSCAD files for the chassis, and a list of everything else used in aliexpress links down below

![image](https://github.com/Viper-7/CharlieBoy/assets/39548/db019a88-63ea-4561-bb5e-ff60e87e53a9)


Complete Parts List:

* LCD Module: https://s.click.aliexpress.com/e/_DnsErhJ
* Arduino Uno with A6+A7: https://s.click.aliexpress.com/e/_DmORDdP
* TP4056+DW01: https://s.click.aliexpress.com/e/_DeaHOff
* Joystick Module: https://s.click.aliexpress.com/e/_Dd41Vq1
* Switch + Cover: https://s.click.aliexpress.com/e/_DmSk5vR
* M2 Threaded Inserts: https://s.click.aliexpress.com/e/_DDOrytL
* M2 Bolts: https://s.click.aliexpress.com/e/_DEwoBIp
* 500mAh Battery: https://s.click.aliexpress.com/e/_DCwWl85

