When I built my [custom PC](SGIIndySleeper.html), I didn't go very hard on accessories outside of getting myself a monitor because I already had a bunch of stuff around the house. For the keyboard, I had an old (and yellowed) Logitech Internet Navigator keyboard which worked great because it had stuff like media keys on it so that I could control volume and music playback.

Honestly this keyboard is perfectly cromulent and I don't really have any complaints about it outside of the fact that its keys are yellowed (but that's easy to fix) and that it only has like 3 key rollover. I've never been in a rush to get a new keyboard but it would *be nice* y'know?

<p align="center">
![Internet Navigator Keyboard in Counter Strike: Source](images/BuuKeeb/InternetNavigatorCSS.jpg)</br>
Fun fact, if you ever played cs_office back in the Counter Strike: Source days, you might've seen this keyboard lying around the office.
</p>

You see, I'm very picky about my keyboards, and here's why:
- I use a Portuguese layout, which means I need an ISO enter.
- I use all 105 keys on my keyboard, including the menu key which a lot of keyboards love to *not* put in.
- I hate having random crap software, especially if it's always running in the background. Keyboard brands love to not put stuff like CAPS LOCK LEDs on the keyboard because they want you to install their software that gives you a pop up on the screen instead. Why??? Just spend the 5 cents it costs you to put in the three indicator LEDs.
- I don't need noisy switches, keys like "browns" will do. Even better if the keyboard has hotswap sockets in case I decide to experiment with other switches later on.

An extra thing that I would like on my keyboard would be support for the open source QMK firmware, because like that if there's any missing functionality  (like media keys) I can always add it in myself. Finding a keyboard that had all of that stuff (and the 3rd bullet point is apparently **a really hard ask for a lot of keyboard manufacturers**) has been impossible. Per key LEDs are also nice but not a dealbreaker for me.

Recently I came across a barebones keyboard for 30€ and it seemed to tick all the boxes except for QMK support. I thought to myself that even if it doesn't support QMK, I could always add that support in myself with a bit of hacking. Being a really cheap keyboard, I went ahead and bought it. Unfortunately for me, the specific board revision I received is a newer version that replaced the microprocessor with a crappy 8-bit one, so adding QMK support to it was infeasible. However, the case the keyboard came in was mostly metal and pretty nice, especially for the price.

In December I started a new job as an Embedded Systems Engineer, and a bunch of my colleagues had their own custom mechanical keyboards. One of them had even designed his own in KiCad and had it printed. Since the company did a lot of custom hardware work, I thought that it would be a nice learning experience for me to design a keyboard PCB. This would also allow me to be more useful around the company, as I was the only pure computer science person in a team of electrical engineers.

Thus, I decided to go down a rabbit hole to make a custom keyboard which fit inside the nice metal case.

### Making a scan of the original PCB

First and foremost, because I am making a clone of an existing board, I need to know the positions of everything as accurately as possible. One side of the keyboard was completely flat excluding some minor bumps from a through-hole connector, so I desoldered it to have a fully flat face. My idea was to use a photocopier to take a scan of the board, so that I could then have an image I could trace over. Unfortunately the scanner I had at home was a bit too small, meaning that the keyboard would have to be slightly angled and require 2 scans. It being angled was a big no-no since it would cause perspective distortions that prevent getting accurate measurements. I went to a few photocopy houses that had bigger flatbeds and had them scan the flat side for me. All of them unfortunately required taking 2 scans as well but at least the flatbed was big enough to fit the whole board. I picked the best scans, and using Photoshop I straightened the scans (I brought a ruler to keep the board straight on the flatbed but despite my best efforts it would still end up ever so slightly angled) and stitched both halves together. The stitching wasn't perfect, since scanners use a moving camera which introduces minor visual distortions. However the two scans only really diverged by 2-3 pixels which would be equivalent to like 0.1mm, so it wasn't a big deal. The result was:

<p align="center">
![Scan of the keyboard PCB](images/BuuKeeb/FinalScan.png)
</p>

I made sure the scale of the image matched its real life measurements by playing around with the Pixels Per Inch values. The next step was to trace everything that was relevant in an SVG program. Having an SVG has a few advantages, namely that SVGs have "infinite" precision compared to images, which are limited by the number of pixels. That sub-pixel precision allows me to ensure everything is placed as accurately as possible. I use [Inkscape](https://inkscape.org/) for my SVG beeds, and after a few hours I made this:

<p align="center">
![SVG version of the scan](images/BuuKeeb/SVG.png)
</p>

The SVG contains the silkscreen (the white decorative squares where each key is) in blue, the outline of the board, and holes in green. These holes include a spot for the LEDs, hotswap sockets, and mounting holes. There were 6 additional holes (in yellow in the SVG) which were brass mounts that screws were fed through. These mounts don't have any screw threads in them, so I'm not sure why they are different from the other mounting holes, but since I'm going through all this trouble I might as well mark them properly. You might have also noticed the purple star. That star exists so that I can align the layers when I import them one by one into the CAD program.

Now, it would've been really embarrassing if I went through all this work and then it turned out that all the keys were misaligned by an entire millimeter, which wouldn't fit in the case, so I decided to make a cardboard model. I wasn't sure if the printer dialog in InkScape would print the image to scale, and when I tried importing the SVG into LibreOffice Writer (open source Microsoft Word) I couldn't find a way to have it *not* scale the image. LibreOffice Impress (open source Microsoft Powerpoint) imported it perfectly, so I put the SVG in Impress, made 2 slides (since the board wouldn't fit in a single A4), printed it out, cut it, taped the two halves together, glued it to a piece of cardboard (for thickness and sturdiness), and then cut the mounting holes using an exacto knife and a drill. After all that work, I put it inside the case, and it seemed to be a perfect match.

<p align="center">
![Cardboard model](images/BuuKeeb/CardboardModel.jpg)</br>
I know it might not look like it from the image, but trust me it's perfect. If you look directly above each key, it's positioned correctly within each hole, which is why the center keys look fine but the other keys look like they're diverged.
</p>

So, comfortable enough to proceed into making the PCB, it was time to learn KiCad.

### Keyboard Matrix

Before jumping to the schematic, there is a small tangent that must be taken first. The "easy" way to think of a keyboard is a board with a bunch of switches/buttons on it. You feed voltage into each key, and then when you press the key it closes the circuit and sends the voltage to the microprocessor so it knows that a key was pressed. Each key needs to be hooked up to the microprocessor in some way, and the easy way to think about it would be to have each button take up a "leg" of the microprocessor, like this: 

<p align="center">
![Dumb keyboard wiring](images/BuuKeeb/KeyboardWiringSimple.png)
</p>

You will probably notice that you will very quickly run out of legs to hook up to the microprocessor, especially me who wants a full sized keyboard with 105 keys. So instead, what is typically done is you wire the keyboard in a matrix:

<p align="center">
![Smarter keyboard wiring](images/BuuKeeb/KeyboardWiringMatrix.png)</br>
The rows and columns are not connected to one another, they are separate, hence why the rows have a different color than the columns in this diagram. 
</p>

If I wanted to add another row of 3 keys, it would only take 1 extra leg of the processor instead of 3!

A microprocessor has a bunch of legs and some of them will have very specific uses that cannot be changed, like one that takes in power, one that's grounded, one that restarts the MCU if it receives voltage, etc... There will be some pins called "GPIO", which stands for "General Purpose Input Output". **Most** GPIO don't serve any specific purpose, you can make your program do whatever you want with them. In our case, the GPIO will be for our keyboard rows and columns.

So why am I bringing this up now? You see, in order to choose the right components for your keyboard, you need to know how many rows and columns you're going to need. In the case of my keyboard, I already have a very specific layout I have to use because I'm putting it in an already existing case, but if you are also making a custom case for your keyboard, it might be helpful to take a few minutes to use the [Keyboard Layout Editor](https://www.keyboard-layout-editor.com/) to decide how your keyboard will look. You can export the layout into a JSON file which can be used to speed up positioning the keys in CAD software, but just as importantly you can export an image of your layout:

<p align="center">
![My keyboard layout](images/BuuKeeb/Layout.png)
</p>

You should now go into a paint program and decide how you will want your keyboard matrix to be wired, so that you know how many rows and columns you have. In my case, I wired mine like this:

<p align="center">
![My keyboard layout wired up](images/BuuKeeb/LayoutMatrix.png)
</p>

That gives me 21 columns and 6 rows, for a total of 27 GPIO on the MCU.
 
### Making the Schematic

Before you can make the PCB, you need to first make a schematic which explains how all the components are wired together. The first and most important thing you need to pick is the microprocessor for your keyboard. The one you choose depends on what stuff you want on your keyboard. Small keyboards tend to go with a Teensy 2.0 or ATmega32U4 since there are drop-in QMK firmwares available for them. If your keyboard is to be wireless, a popular option is the nRF52840. However, since I need to occupy 27 pins on the MCU just for the keyboard itself I'll need an MCU with lots of GPIO, so I have chosen another popular MCU called STM32, specifically the STM32F072C8T.  You should download the spec sheet of your specific MCU and check how many GPIO pins it provides. The STM32 has up to 37 GPIO pins, which is plenty since I'll need those 27 pins for the keyboard matrix, 2 pins for the USB Data, and 1 pin for the LED logic. The STM32F072C8T has 64KiB of flash memory which is enough for the STM32 port of the QMK firmware, but you could go with a STM32F072CBT6 for 128KiB if you intend on having a lot of crazy firmware features.

To make the schematic and the subsequent PCB, I will be using [KiCAD](https://www.kicad.org/) since it is free, open source, and widely used in the industry (including at my workplace). 

Before starting my project, there is one external library which will come in handy, and that is [marbastlib](https://github.com/ebastler/marbastlib). This library provides footprints and 3D models for popular mechanical keyboard parts, such as MX style hotswap switches. There are some other useful plugins like "Keyboard Footprints Placer" which will automatically position keys on your PCB from the JSON file you exported from Keyboard Layout Editor. I won't be using it because I'll need to place keys according to my SVG file.

This blog post won't serve as a full KiCAD tutorial, I will talk about the keyboard creation process but I expect you to also look at videos and tutorials on how to use the program, as well as other mechanical keyboard designing posts/videos.

Upon creating your schematic, search for your specific microprocessor and place it on the sheet:

<p align="center">
![STM32 Schematic](images/BuuKeeb/STM32Schematic.png)</br>
There sure are a lot of legs on this creature. 
</p>

At this point, it is a good idea for you to read the specification sheet of your device **in full**. You don't need to understand everything that's there, but you do need to read it or you will risk wiring something incorrectly. Reading through it, you will learn that:
- The STM32F072C8T is expecting 3.3 volts in both VDD pins to power the device. 
- VBAT is a backup power supply that you connect a battery to so that the STM32 can do things when it's turned off (like keep track of time). We don't need that functionality, but we are still required to supply it with 3.3V. 
- VDDA is power for analogue circuitry. We're not going to use analogue logic, but we are still required to power it with 3.3V. If we were using analogue logic, we'd also need to connect this to a ferrite bead to reduce noise in the circuit.
- VDDIO2 is power for the C port GPIO pins (PC13, PC14, PC15).
- VSS is ground.
- VSSA is ground for the analogue circuitry. If we were using analogue logic we'd connect this to a less noisy ground point, but we aren't so we can just hook it to the same ground as everything else.

Wiring that is easy:

<p align="center">
![STM32 wired to power](images/BuuKeeb/STM32SchematicPowered.png)</br>
Confusingly, despite the 3.3V arrow pointing away from the circuit, the voltage direction is actually going towards the MCU. This is the drawing convention for schematics, don't ask me why.
</p>

I did skip over one important thing which is capacitors. A capacitor is like a "mini rechargeable battery", and one of their main purposes on a PCB is to serve as a voltage stabilizer (these are known as decoupling capacitors). For example, if a specific line of the circuit is being fed 3.3V but for some reason that drops to 2.7, a capacitor will provide some of its stored power to push the signal back to 3.3. Subsequently, if the line erratically gets too much voltage, like 3.7, the capacitor will absorb that voltage to put it back at 3.3. It's pretty normal to have at least one capacitor for every power source of an integrated circuit (IC). The STM32 recommends having:
- A 100nF capacitor for every power input (VDD, VBAT, VDDA, VDDIO2).
- VDDA should also have an extra 10uF capacitor hooked up to it.
- The very start of the 3.3V rail should have a 4.7uF capacitor.

That would look like this:

<p align="center">
![Decoupling capacitors hooked up directly to the MCU](images/BuuKeeb/DecouplingCaps1.png)
</p>

This is a bit annoying to read, so it's common for people to have the caps in a separate but equally wired configuration. I did so and left a note:

<p align="center">
![Decoupling capacitors on the side of the schematic ](images/BuuKeeb/DecouplingCaps2.png)
</p>

Next, we need to hook up the reset system. In order to program the STM32 with our custom firmware, it needs to be put in a safe state to receive the firmware, which requires us to wire the NRST and BOOT0 pins. The data sheet recommends putting a 100nF capacitor on NRST to prevent random parasitic voltage that gets on the line from accidentally triggering the reset:

<p align="center">
![Capacitor on NRST](images/BuuKeeb/NRST.png)
</p>

And then, in order to put the STM32 into "Bootloader Mode" (the safe state where it can be programmed with our custom firmware), we need to feed the BOOT0 pin 3.3V. If we give it 3.3V all the time then it's never going to leave bootloader mode, so I'm going to hook it up to a button so that we can choose to enter BM mode by holding the button down when plugging the board into power. This requires a 10K Ohm pulldown resistor to ensure the pin remains low when the button is not pressed, because without it the pin is "floating" and can pick up random signals from the environment. If you are using USB-C then you are going to already be using 5.1k resistors (as they're part of the spec), so it might be cheaper to use two 5.1Ks (which is a value close enough to 10K) as keeping all the resistor values the same will reduce costs due to having less varied parts. It's gonna be a very minor cost, however.

I added a label to the BOOT0 lead called BOOT, and I wired the boot circuit up separately:

<p align="center">
![Bootloader button schematic](images/BuuKeeb/BootloaderButton.png)
</p>

Lastly, we need to wire up the USB D+ and D- lines. USB has 4 main pins, which is power, ground, D+, and D-. The two D pins are data, and they contain the exact same information, however they are what is called a "differential pair", meaning D- will always contain the opposite value of D+. This is done to provide robustness and error checking for digital data signals. 

<p align="center">
![USB Pinout](images/BuuKeeb/USBPinout.png)
</p>

On the STM32, the PA11 and PA12 pins are *specifically* for USB D- and D+ respectively. USB data pins won't function anywhere else, the pins cannot be wired the other way around because they will not work and I did not have to find this out the hard way! :)

<p align="center">
![STM32 with the USB data pins](images/BuuKeeb/STM32SchematicBeforeMatrix.png)</br>
Since the D+ and D- are both inputs and outputs, the flag I gave them points in both directions. The flag directions are purely cosmetic and don't affect functionality whatsoever.
</p>

To hook up USB to the STM32, we need to have a connector. I would've loved to put USB-C on my keyboard, but after a lot of measuring and trying, there is unfortunately no space inside the case for it. So I'm gonna have to use the same 90º 5 pin connector that the original board uses (the 5th pin is "Shield Ground"). It's usually a good idea to have a 500mA fuse on the USB connector's power line in case we plug the keyboard into a faulty USB that pushes too much current down the 5V line. The fuse will pop but it will save everything else on our board. USB 2.0 is usually limited by 500mA, USB 3.0 can do 900mA, and USB-C varies because the USB-C spec is a headache. 

<p align="center">
![USB connector schematic](images/BuuKeeb/USBConnector.png)</br>
VBUS and 5V are basically the same thing, except we usually designate VBUS as "raw unsafe voltage straight from the connector", it becomes a "safe and protected" 5V after going through the fuse.
</p>

If you are using a sane type of USB connection, like USB-C, you'll need to hook up some extra components for proper functionality. Refer to your spec sheet. 

The fuse will "protect" the voltage line but we also need to protect the data pins from electrostatic discharge corrupting our data and potentially frying something. A common IC that does the job is the USB6B1:

<p align="center">
![USB6B1 schematic](images/BuuKeeb/ESDProtection.png)</br>
We can feed VBUS into the ESD protection because if you look at the spec sheet's wiring diagram, you'll see that it goes straight through the IC and does not touch anything. 
</p>

Lastly, USB provides 5 volts that we can use to power everything, however our STM32 needs 3.3v. To make sure we don't kill the processor, we need to have a step down converter that takes in the 5V from USB and provides 3.3V. We can use the XC6206 to do just that (it's technically a linear regulator but it'll serve this task as well), along with some capacitors as per the spec sheet:

<p align="center">
![Step Down Converter Schematic](images/BuuKeeb/StepDown.png)
</p>

Okay, that's everything we're going to need for the time being. Let's wire the keyboard matrix together!

### Keyboard Matrix Schematic

To keep the schematic clean, I'm going to do the matrix in a separate hierarchical sheet.

Wiring a keyboard matrix is relatively simple, you connect one end of each switch to a row, and the other end of the switch to a column, something like this:

<p align="center">
![Proper wiring diagram](images/BuuKeeb/MatrixWiring.png)
</p>

The way a microprocessor detects that a key was pressed is by sending voltage down each row, one at a time, and then checking which columns are receiving voltage. For instance, if the MCU sends voltage down Row 1 and it receives voltage in Column 2, it knows that the W key was pressed:

<p align="center">
![Test row 1 with W key pressed](images/BuuKeeb/MatrixWiringVoltage.png)
</p>

Obviously it will also poll Row2 and it will not receive voltage on any column, so it knows that A, S, and D are not pressed down.

But what if I press another key at the same time, like A? The MCU will scan row 1 first, and then row 2, and it will correctly detect both presses.

<p align="center">
![Test row 1 with W, and A key pressed](images/BuuKeeb/MatrixWiringVoltage2.png) ![Test row 2 with W and A key pressed](images/BuuKeeb/MatrixWiringVoltage3.png)
</p>

Great, now what if we press a third key, like S?

<p align="center">
![Test row 1 with W, A, and S key pressed](images/BuuKeeb/MatrixWiringVoltage4.png)
</p>

Now we have a problem, because when the first row gets scanned the current goes through the W key, then down to the S key, then to the left where the A key is, which sends the current up column 1 despite the fact that the Q key is **not** pressed. The MCU has no way of knowing that the Q key isn't pressed, it just reads whatever voltages it receives, so it will type both the letter W and Q. This problem is known as "Key Ghosting".

So how do we fix this? We place a component called a diode on each key, which only allows electricity to flow in one direction. Let's look at what happens now:

<p align="center">
![Test row 1 with W, A, and S key pressed, but with diodes](images/BuuKeeb/MatrixWiringVoltage5.png)
</p>

The diode on the S key prevents the current from flowing into it, which is perfect. Putting a diode on each key raises the cost of the keyboard ever so slightly (by a few cents), but it will allow us to have N-key rollover.

In order to place the switches, you need to have marbastlib installed, because keyboard switches are not a standardized SMD parts. Marbastlib will provide schematics for the keyboard switches of your choice, which in my case was hotswappable MX switches.

After a bit of CTRL+C and CTRL+V, you can have a nice schematic like this:

<p align="center">
![The final keyboard matrix schematic](images/BuuKeeb/MatrixSchematic.png)</br>
The labels for the rows and columns look different because they're "hierarchical labels" instead of global labels. This is so I can connect these labels to the main sheet.
</p>

You need to be careful when you're wiring because you don't want the rows and columns to be wired together. If you see a fat dot on the crossing between a row and column, delete it because it's not supposed to be there:

<p align="center">
![Wiring mistake](images/BuuKeeb/MatrixMistake.png)
</p>

Now, we need to choose how we're going to hook the rows and columns to the GPIO. We're free to pick whatever order we want, so for the time being just hook them up in numerical order. We can change this later during the PCB stage to however is more convenient for the wiring.

<p align="center">
![Almost finished STM32 Wiring](images/BuuKeeb/STM32WiringPrePCB.png)
</p>

Notice how I skipped the PF0 and PF1 leads. Many microprocessors require an external crystal oscillator to provide the timing for the logic, but the STM32 has a clock built into it. You can, however, supply it with an external clock by hooking it up to PF0 and PF1, however if you don't need the clock you can treat PF0 and PF1 as GPIO just like the rest. I've kept them as unconnected for the time being but considering that I will probably not be needing an external clock they are bound to be changed to GPIO when I get around to wiring the PCB.

### LEDs

The most popular type of LEDs for keyboards are "Neopixels", which are LEDs that are designed to be chained together to run effects and patterns. Of the "Neopixel" type, the two most used for keyboards seem to be SK6812 MINI-E and WS2812B. Typically, for LEDs, you have a blue, red, and green channel, as well as a ground, but Neopixels instead have voltage in, ground, and then a data-in and data-out pin. You send some bytes to the data-in with the RGB values for the LED to use, and then afterwards it will send the next chunk of LED data through its data-out pin into to the next LEDs's data-in. In other words, each LED will take a bit of the data and pass the rest to the next one in the chain.

<p align="center">
![LED Chain wiring example](images/BuuKeeb/LEDChain.png)</br>
These LEDs are also not a standardized SMD component, so you need marsbastlib for them as well.
</p>

The keyboard I'm cloning is using SK6812 MINI-E's, so that is what I will use as well. And since I have 105 keys, I will require 105 LEDs, as well as 3 extra LEDs for the CAPS LOCK, Num Lock, and Scroll Lock keys.  Wiring these is super simple, just connect all of them in parallel to the 5V line and the ground line, and then hook the data-out of one LED into the data-in of the next.

QMK will handle the LED logic for you because all you need to do is provide it with an XY coordinate for the LED, as well as its index in the chain. This means that you can wire them together however is most convenient to you. That can be like this:

<p align="center">
![LED Chain wiring method 1](images/BuuKeeb/LEDChainMethod1.png)
</p>

But this has the issue that the data wires are gonna be very long, so they risk losing signal integrity due to impedance in the long copper traces and interference from other signals. A better way to wire them would be to flip each second row of the LED chain, giving you this shorter path:

<p align="center">
![LED Chain wiring method 2](images/BuuKeeb/LEDChainMethod2.png)
</p>

There is one last thing missing, which is that the spec sheet for the SK6812 MINI-E recommends each LED have a 100nF capacitor hooked up to their power. It seems that most people do not bother to do this, and they claim the LEDs don't need them. But I don't mind spending a few extra cents on diodes if it means my keyboard is more robustly designed and up to spec.

I put the LED chain in a separate schematic to reduce clutter, like I did with the matrix, and then I link to it in the main page.

<p align="center">
![LED Chain final](images/BuuKeeb/LEDChainFinal.png)</br>
Notice how the chain starts on the right side, this is on purpose and will make more sense later, but you don't have to start there if you don't want to!</br>
Since I'm starting on the left, I have to flip each odd row as opposed to each even row.
</p>

Do keep in mind that each LED can pull up to 13mA if it is running at full brightness. 108 LEDs means 1.3A which will pop our 500mA fuse. Luckily, we don't need to run the LEDs at max brightness, we can limit how bright they are in software.

Now, we can't just directly connect the LED data pin to the STM32, for one specific reason: The GPIO pins output 3.3V, and this _might_ work fine, but it might not considering it's a really long chain of LEDs. Since the logic operates anywhere between 3.2 and 6V, I am going to add a "logic level shifter" to boost the LED data data voltage from 3.3 to 5V. One good device for this is the 74AHCT1G125, which has 5 leads:
1. Output enable
2. Input data
3. Ground
4. Output
5. Power

The spec sheet says "If the transceiver has an output enable pin, it will disable the outputs section", so we definitely don't want to activate the first pin. We can just hook it to ground, and wire everything else logically (and with a safety 100nF capacitor):

<p align="center">
![Voltage Step Up Wiring](images/BuuKeeb/VoltageStepUp.png)</br>
I added a little diamond to the LED flag so that we know that it's an input being "passed into" this schematic. This isn't really standard, I just did it because it helps me keep track of the labels
</p>

A 470 Ohm resistor is recommended to be placed before the start of the first SK6812 MINI-E's data-in pin (according to the spec sheet), so I added it after the transciever.

Now we just assign the LED_DATA flag into any free GPIO in the STM32, and we are pretty close to being able to put all of this on a printed circuit board.

### Footprints

So now that we have the schematic outlining what parts we need and how they're connected together, we need to know what the components look like. KiCAD has a button called "Assign Footprints" that lets you select what space your component occupies on the board. 

But how do you know what sizes to pick? Well, it depends. For instance, you can get resistors the size of a propane tank, or as small as a grain of sea salt. Different sizes usually mean different operating wattages and temperatures, and since a keyboard is not some hyper complex and power hungry beast, we can get away with small components. For keyboards, it's pretty common to use Surface Mount Devices (SMD) as opposed to larger through-hole components which are much easier to solder. SMD components come in a range of sizes, and typically those are 0402 and 0603, which stand for 0.4x0.2 inches and 0.6x0.3 inches respectively. Annoyingly for the sane parts of the world, when we refer to these sizes it's typically in inches, because if you refer to 0402 in metric you will get a component small enough to lodge itself in between the bumps on your fingerprints, and you will never see it again. Not ideal if you're planning on hand soldering.

<p align="center">
![Different sized resistors on a match head](images/BuuKeeb/MatchHead.png)</br>
Image sourced from [here](https://startup88.com/hardware/2014/09/30/prototyping_hardware_15_lessons_learnt_the_hard_way/2336/attachment/resistors-on-match-head-2)
</p>

For parts in the nano range (like the 100nF capacitors), 0402 are usually good enough, while parts that push into the micro range (like the 10uF resistors) are safer at 0603. The resistors we'd use for a keyboard are probably fine at 0402. I would suggest using a 1206 for the fuse, since it's supposed to be something that pops in case of an emergency and you want to have an easier time soldering and desoldering them. Diodes use a different designator: "Small Outline Diode", and the numbers on these are not dimensions but rather just package codes. Because electrical engineers have a sense of humor, the numbers work backwards in this case, meaning SOD-123 is significantly larger than an SOD-923. SOD-123's are already plenty small and will serve fine for a keyboard.

When in doubt, consult spec sheets for some components and see what temperatures and wattages they're safe to operate at.

Lastly, we have ICs, which have a bunch of different designations. Too many abbreviations to list, just commit this diagram to memory:

<p align="center">
![Different IC packages](images/BuuKeeb/ICPackages.png)</br>
Image sourced from [here](https://learn.sparkfun.com/tutorials/integrated-circuits/all)
</p>

You should check the spec sheet for the components you selected to see what footprints you should pick for them, it will usually tell you along with the exact size in mm. In my case, the STM32 is LQFP (low profile version of a QFP chip) with 48 leads, the transciver is a TSOP-5, the Voltage Regulator is SOT-23 with 3 leads, and the ESD protector is SOIC-8. The USB connector will depend on what you selected, I just have a simple horizontal 2mm 5 pin header. The bootloader button can be whichever SMD button that fits your requirements and that you find most adorable. Most buttons come with 4 legs, which are usually the same 2 legs mirrored on the other side for convenience.

The last footprints we need are the LEDs and the MX hotswap sockets. Like I said previously, these are available via marbastlib since they're not standardized parts. Just be careful selecting them because there's a bunch of variants, you'll want to select the one that matches the spec sheet you're using. If you right click a footprint it lets you view it so you can confirm the size and pins match.

Once all parts have footprints, we just need to confirm we have no problems with our schematic.

### Error Checking the Schematic

KiCAD conveniently provides the Electrical Rules Checker, which will check for problems in your design. This will not validate that you are following the specs of your ICs (KiCAD doesn't know anything about them or how they work), just if there are issues in your wiring. Ideally, you'd want zero warnings, and you'll need zero errors. Most of the errors are self explanatory, or explanatory enough with a quick Google search, but there is one error that is really weird: "Input Power pin not driven by any Output Power pins".

Basically, KiCAD sometimes struggles to understand where exactly power comes from. You know that the USB connector provides the power and ground, but _KiCAD_ doesn't know that. So the solution is to place a component called the PWR_FLAG near a power source to tell KiCAD to trust you. You only need to do this once for each net, so if you put the flag next to the VBUS, 5V, and GND lines of your USB connector, that should be enough to silence the error.

<p align="center">
![USB Connector after adding the PWR_FLAGs](images/BuuKeeb/USBConnectorWFlags.png)
</p>

### Making the PCB

Making the PCB is the most fun part, and it will probably be where you will spend a lot of time if you're like me and you can't help yourself but to George Lucas a project.

So, what exactly is a PCB? It's essentially a really convenient and compact package for a circuit. PCBs are usually a sandwich of multiple layers of fiberglass and copper, each copper layer is electrically isolated from one another by the fiberglass. Simple PCBs are typically 1 or 2 layers of copper, but slightly more complex designs push four layers, while motherboards can go as high as 16 or 32.

<p align="center">
![A diagram of PCB layers](images/BuuKeeb/PCBLayers.png)</br>
An example of a PCB sandwich. Notice that something is connecting the two copper layers on the right, we'll get to that.</br>
Image sourced from [here](https://www.pcbasic.com/blog/pcb-layers.html)
</p>

The important thing to understand about PCBs is that a fabrication plant will lay an entire layer of copper first and then etch away what isn't needed. You don't need to worry about spending money on copper, you're already paying for the whole layer.

For a keyboard, 2 copper layers is what is recommended. One is too constrained, and more than two is generally unecessary. KiCAD creates PCBs with two copper layers by default (convenient!), usually referred to at the "front" and "back" layer. But copper isn't the only layer you have to worry about, because there's a lot of them that KiCAD provies. The most useful ones for us are:
- F.Cu and B.Cu - The front and back copper layers (since we have 2 copper layers)
- F.Silkscreen and B.Silkscreen - The front and back silkscreen layers, where you can put drawings on your PCB.
- User.Drawings - Helper drawings that won't show up on the final PCB.
- Edge.Cuts - The outline of your PCB + any holes you will put in.

When you open the PCB editor in KiCAD, the first thing you have to do is to import the parts from the schematic. There is a button for it on the top called "Update From Schematic", which when you run it, will spawn all the parts for your keyboard at your mouse.

<p align="center">
![All the parts for the project](images/BuuKeeb/PCBParts.png)
</p>

Once you place the parts down, KiCAD will draw some wires showing how all the parts should be connected. Every time you change something in the schematic, you have to click the "Update From Schematic" button, and it will automatically spawn any new parts you created and update the legs or footprints of any parts you modified. It won't remove parts that already exist on your PCB, technology simply has not evolved that far.

If you are making your keyboard layout from scratch, you can use the Keyboard Footprints Placer plugin to import the JSON file and automatically position all the keyswitches for you. In my case, I instead had to import those SVGs I made so that I can place the switches in the exact place they were on the board I'm cloning. I put the key outlines in the Front Silkscreen layer, the LED and mounting holes in the Edge Cut layer, the PCB outline in the Edge Cut Layer, and the keyswitch holes in the UserDrawings layer. Each of these SVGs had that purple star in it to help me position all the imported drawings in the exact same place. 

<p align="center">
![The PCB after importing all the layers](images/BuuKeeb/PCBBare.png)</br>
KiCAD lets you view a 3D render of your board. It's a good idea to look at it a lot because you might spot mistakes!
</p>

The default footprints of components in KiCAD come with some drawings on the silkscreen layer. These will usually include an outline or marking that points to the ground pin of a component, and the component's name from the schematic. You can delete these if you want a super clean PCB (don't do it right now though or you will regret it), but if you're planning on hand soldering I would really recommend keeping them.

Then I moved every single keyswitch to its exact position according to the drawing. To do that, I zoomed in as far as I could and tried to manually place them as centered as possible. Even if they're not exact, we're talking sub milimeters here, which is way smaller than the fabrication houses probably have tolerance for.

Once the keyboard layout is to your liking, you need to draw an outline of your PCB in the edge cut layer to define the shape of your PCB. Since I had my outline SVG, I just imported it. Those who don't have a prepared SVG (which is probably all of you not cloning an existing keyboard like me) can just draw your own outline freestyle using the program's tools. 

Now, all those hundred diodes, capacitors, LEDs, and everything else have to be placed on your board too. Remember that each one has a specific name, so don't place the diode that is supposed to be wired to the Q key where the M key is. Good thing they have their names on the silkscreen so you can identify them, and you were smart enough to not delete the silkscreen :)

You don't have to fully commit to where you place the components, because you will likely change them around a lot as you find more convenient locations for things or realize during the wiring stage that you've run out of space. Don't forget that there is a front and back side to the PCB, so make sure you have the components on the correct side of your board. Usually the side with the switches will be bare or have an IC or two on it, while everything else is on the other side. It's up to you!

<p align="center">
![The back of the PCB with all the parts placed](images/BuuKeeb/PCBBackParts.png)
</p>

I think the only main thing you need to worry about regarding the placement of components is that it's highly recommended to place capacitors as close as possible to the pin it's supposed to be protecting. Having them far away makes them less effective. Likewise, the ESD protection should be right next to the connector.

Once you're happy with positioning everything, run the Design Rules Checker (DRC) to check if there aren't any horrible problems so far (besides the unconnected items), like overlapping components. If that passes, then I would recommend looking for the DRC rules for whichever fabrication house you intend on making the PCB at. The two most popular ones are [PCBWay](https://www.pcbway.com/) and [JLCPCB](https://jlcpcb.com/), and either one will do the job. My work colleagues use JLC for their stuff so that's what I will use as well, but my N64 colleagues use PCBWay. JLC has an article regarding their [design rules](https://jlcpcb.com/capabilities/pcb-capabilities), and there is [this project on GitHub](https://github.com/labtroll/KiCad-DesignRules) which translates them into DRC rules for KiCAD.

If the only errors you have left on the DRC are unconnected items, let's go over some recommendations for the wiring stage.

### Recommendations for Wiring the PCB

There are a lot of dos and don'ts when it comes to wiring PCBs, some exist for historical reasons but aren't as relevant today, while others are subject to incredibly fierce and exciting (read: boring) debates between engineers. Probably the big one that gets people fuming is that we can't have 90 degree bends in traces because they introduce electromagentic interference, so we should only use 45 degree bends. Whether or not this holds true for high frequency devices or high voltage PCBs does not really matter to us because a keyboard is neither. [You can go fully curved](https://community.element14.com/technologies/open-source-hardware/b/blog/posts/vintage-curvy-pcb-traces-with-kicad-7) if a vintage look is your preferred aesthetic! 

The other big one is to avoid is forks:

<p align="center">
![A track with two different forks on it](images/BuuKeeb/TraceFork.png)
</p>

The reason for this one is that the sharp bends create pockets that will cause the acid that's used during the etching of the copper to get trapped, which risks damaging the traces. *Supposedly*, modern PCB manufacturing has largely eliminated this issue, but the recommendation remains.

If you recall from our keyboard matrix, we have lanes which cross over each other, but we don't want them to electrically connect. That is why we have 2 layers of copper, it lets us cross over tracks:

<p align="center">
![Two row and two column traces crossing over each other](images/BuuKeeb/TraceCrossing.png)
</p>

In the scenario you do want the two layers to connect, you can use a "via", which is a hole that drills into the other layer (if you remember the PCB sandwich diagram from earlier, it was what was connecting the two layers together). You can use vias to help cross over existing traces:

<p align="center">
![Using vias to help cross over a trace](images/BuuKeeb/TraceUnder.png)</br>
You can also place vias on pads, but this is generally not recommended because it can introduce manufacturing defects. 
</p>

The smartest thing you can do is keep one layer with only horizontal traces, and another with only vertical traces. This will help minimize how many vias you need to place and also much space your traces occupy, because bends will take up real estate for both horizontal and vertical traces.

KiCAD, by default, uses traces that are 2mm in size, and these are good enough for most things, but it is generally recommended to make traces that carry digital data + traces that carry power thicker. For power traces, the recommendation is because smaller traces will heat up more due to the constant current being pushed through them, while data traces are recommended to be thicker to improve signal integrity. There are math formulas and calculators built into KiCAD to help figure out the ideal trace widths, but I am not enough of an electrical engineer to be able to use them, I just went with the recommendations from other people. I chose 6mm for the USB data traces, 4mm for LED data + power traces, and everything else used the default width.

The track you are probably going to be placing the most will be the ground, which will quickly occupy a lot of your track real-estate. Because of this, a common thing to do is to make a ground plane. The idea is that the entire copper layer becomes ground, and your traces and vias will be carved out of the ground. This picture demonstrates it better than I can explain:

<p align="center">
![An IC and the traces connected to it, carved out of the ground plane](images/BuuKeeb/GroundLayer.png)</br>
The ground is the entire blue plane. Notice how the ground pads on the IC  are automatically connected to the plane.I'm at 90 fm now because of this dumb grind
</p>

Having a ground plane on both the top and bottom copper layers is a good idea and a great time saver. I originally started wiring everything without making a ground plane first, which created a few headaches. The only annoying thing about the ground plane is that KiCAD will not update it every time you place something down, you need to manually regenerate the planes. Sometimes you will create an island that is electrically isolated from everything else, which is why having the ground plane on both layers is convenient, because you can connect the island to the rest of the ground plane using vias:

<p align="center">
![An example of a ground plane island](images/BuuKeeb/GroundIsland.png)</br>
The LED capacitor's ground plane is electrically isolated from the primary plane, so I placed a via to connect it to the plane in the other layer.
</p>

Sometimes KiCAD will not be able to make the ground plane automatically reach pins that have a lot going on around them, but you can usually just wire a track from the pin to the plane:

<p align="center">
![A groud track connecting to a ground plane](images/BuuKeeb/GroundHelp.png)
</p>

Complicated PCBs with more than 2 layers will usually have an entire plane be a ground plane without any traces on it, another plane which will just be 3.3V, etc... and then electrically connect to them with vias.

If you remember from your highschool physics, current running through a wire generates magnetic fields around it, which in turn messes with the electrons in adjacent wires. If you don't remember this detail, you can [re-traumatise yourself with this video](https://youtu.be/NJRDclzi5Vg?t=62). Either way, because of this physical phenomenon, it's a good idea to keep wires some distance from one another. It also means that wires (especially power wires that tend to have a lot of constant activity) should not run parallel to wires in the other layer. Try to minimize crossing wires, but of course it's not a huge deal if you must.

Everything else is, down to your aesthetic preferences. You can route stuff under ICs, and connect to pads perpendicularly, but doing this gives me severe botherations, so I avoid it as much as possible.

<p align="center">
![Routing perpendicularly](images/BuuKeeb/Botherations.png)</br>
This works, but it does not spark joy.
</p>

When it comes to connecting tracks to pads, all you really need to do is to touch them. This would be completely valid wiring but will probably result in you getting bullied by nerds:

<p align="center">
![A track barely touching the pad](images/BuuKeeb/CaressingPad.png)
</p>

And lastly, but most importantly, the golden rule is to **keep your tracks as short as possible.** If you don't know why you need to do that, go back and re-read this entire section until you do. 

Many of the rules that were outlined here are not set in stone, you can break them if you are having trouble wiring things as-is. But avoiding them as best as possible is generally advised.

### Actually Wiring the PCB Together

The very first thing you should wire in your PCB should be your USB data lines, since without them properly functioning, nothing in the keyboard will work. The USB data lines are also the only thing that needs to be wired differently from everything else in our design, because they are a differential pair. KiCAD has an option for wiring a differential pair, letting you do both tracks at the same time:

<p align="center">
![Wiring a differential pair](images/BuuKeeb/DifferentialPair.png)
</p>

The important thing about a differential pair is that both traces need to have the same length or the data will not arrive in sync. For USB 2.0, the maximum deviation you can get away with is a difference in length of 3.81mm between both tracks. When you need to have your tracks turn, it's common for one to get longer than the other, but luckily KiCAD lets you add some curves to make the shorter track longer:

<p align="center">
![Making one track longer to match the lengths](images/BuuKeeb/CoolS.png)</br>
Now the left track matches the size of the right one.
</p>

In my case, because the USB connector has the data lines swapped from the inputs on the STM32, and because I can't rotate the connector, _and because_ I wanted to avoid routing the data lines under the ESD chip for signal integrity reasons, I had to do something gnarly:

<p align="center">
![Criss-crossing the USB data lines](images/BuuKeeb/Gore.png)
</p>

USB is relatively robust so doing this isn't going to be a horrible problem, but it should be avoided in your design if possible.

An interesting recommendation that I was given is to place a bunch of vias that connect to the ground plane along the path of the USB differential pair. The reason for doing this is to give the ground on the connector a guaranteed shorter path, because otherwise there's no way to know how long the return path is gonna be and how far it'll divert away from the USB differential pair traces.

Outside of the USB data differential pair, everything else can be routed pretty straightforwardly. There's a lot of different ways of routing the keyboard, I would recommend looking at a bunch of different open source keyboard designs and seeing what other people do. I went through 4 or 5 iterations of my routing before I settled on my current wiring.

I had originally routed my rows and column tracks exactly how I had it in the schematic (IE always starting where the flags were positoned), and this resulted in tons of long tracks that surrounded the entire PCB:

<p align="center">
![The original way I wired the keys](images/BuuKeeb/WiringRound1.png)
</p>

During my many rewiring attempts, I did this instead which is perfectly valid.

<p align="center">
![The better wiring](images/BuuKeeb/WiringRound2.png)
</p>

Remember, you can change which pins in the STM32 the rows and columns are connected to, so swap things around
 to make your wiring tighter!

The other big thing that went through iterations on my design was the LED wiring. I originally had the 5V line of all the LEDs connected together:

<p align="center">
![The original LED power wiring method](images/BuuKeeb/WiringLED1.png)</br>
I did not draw the capacitors on this wiring diagram, but just know they are all right next to the 5V pin on each LED.
</p>

But then I decided to have the odd and even rows connected seperately from one another to reduce how far the power would need to travel just to reach the last LED in the chain:

<p align="center">
![The better LED power wiring method](images/BuuKeeb/WiringLED2.png)<br>
Notice how this also keeps vertical and horizontal traces in separate layers!
</p>

Though if I were gonna do one more rewiring to the keyboard, I would probably wire it like this:

<p align="center">
![Future LED power wiring method](images/BuuKeeb/WiringLED3.png)
</p>

Wiring everything else was relatively straight forward.

After many days of channeling my inner George Lucas, I settled on the final wiring:

<p align="center">
![Final front copper layer](images/BuuKeeb/WiringFinalF.png)</br>
![Final back copper layer](images/BuuKeeb/WiringFinalB.png)
</p>

When you're happy with your wiring, run the DRC and fix any potential overlapping wires, missed connections, isolated ground planes, ground connections without at least 2 paths, and other mistakes. 

I added my face as well as a piece of text marking the revision to the silkscreen, and I was ready to send the PCB off to fabrication!

### Ordering

### QMK

Matrix explanation was a lie
