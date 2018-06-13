# SNES-Controller
An Arduino sketch that utilizes bluetooth to act as an SNES controller
Created by Brandon Rolston

== Requirements ==
Tested on an Arduino Uno and an Arduino Mega, but it should work on any Arduino that
has at least two available interrupt pins and a 16 MHz clock. This sketch also utilizes a 
BlueSMiRF Silver, although any device that can output serial bluetooth data should work fine.

If you want to make this work, you'll need to create (or reappropriate) a way to plug into the SNES controller port.
The following diagram should help there.

== Controller Port Pinout Reference ==
       ----------------------------- ---------------------
      |                             |                      \
      | (1)     (2)     (3)     (4) |   (5)     (6)     (7) |
      |                             |                      /
       ----------------------------- ---------------------


        Pin     Description           
        ===     ===========            
        1       +5v                    
        2       Data clock           
        3       Data latch         
        4       Serial data       
        5       *Unused                   
        6       *Unused                
        7       Ground 
        
        * Not just unused by this script, those pins are actually connected
        to absolutely nothing. On some combo SNES/NES devices I have seen,
        they are often used for extra functions for custom controllers. You
        do not need to worry about them however.
       
== Data Out Reference ==

       If you notice buggy output when running this code on whatever you're using, here's the order
       of the buttons as they get pushed out on the Serial Data line.
       
        Clock Cycle     Button Reported
        ===========     ===============
        1               B
        2               Y
        3               Select
        4               Start
        5               Up on joypad
        6               Down on joypad
        7               Left on joypad
        8               Right on joypad
        9               A
        10              X
        11              L
        12              R
        13              none (always high)
        14              none (always high)
        15              none (always high)
        16              none (always high)
        
        Yes, I know the 4 unused bits are odd. That's just how it is. Not sure what plans
        Nintendo may have had for those at one point.
        
== Bluetooth Serial "Codes" ==
       The currently pressed buttons within the script are saved just as an actual SNES controller would:
       as a 16-bit register, with one bit representing each button. In this script, a "HIGH" value indicates
       a button that **hasn't** been pressed, as a SNES controller writes logic high for unpressed buttons.
       There is one "code" for a button press, and one "code" for when a button is released. This reduces
       the number of commands per cycle that need to be processed. Each 'code' is an ASCII character.
       
       The codes are as follows:
      
        BUTTON      PRESS     RELEASE       
        ======      =====     =======
        B           'b'       'B'
        Y           'y'       'Y'
        Select      '-'       '_'
        Start       's'       'S'
        Up          'u'       'U'
        Down        'd'       'D'
        Left        'l'       'L'
        Right       'r'       'R'
        A           'a'       'A'
        X           'x'       'X'
        L           '1'       '2'
        R           '3'       '4'
        
        As you can tell, I was *very* original with my character choices.
        
== Basic Operation Info ==

As the code functions, it doesn't even CARE when the SNES decides to poll the controller, it'll get that data
out there as fast as possible whenever. Which is great, because I have experienced an aftermarket SNES that
actually ran at a MUCH faster clock speed than a standard one, and it worked fine.

Essentially, the Latch pin goes high, triggering the "controller" to latch the values of the currently pressed
buttons. It immediately provides the first register value, as the Data clock will start soon after, giving 16 
12 us long pulses at 50% duty. The clock samples on falling edges, so the data line is set on the rising edge.

== The Caveat ==

As is, this works well. However, if you start mucking around with things, you should know one thing in particular:

THIS IS EXTREMELY SENSITIVE TO ANYTHING THAT CHANGES THE TIMING OF THE INTERRUPTS

As it is, a 16 MHz processor does a calculation every 62.5 nanoseconds. This gives you a healthy 192 instruction cycles
to catch that clock edge coming through. However, with the bluetooth module running, it cuts it close at times. As it is,
there is a *very rare* occurance where the upper register of controller state values being accessed cause enough delay
to cause a "stutter" effect. This is most noticeable on held "A" presses, as that is the first value on the upper register.

If you add any code in the interrupts, you do so risking it just going all to pieces. As is, they use bitwise operators
to cut down on how much time needs to be spent in there as possible, but even enabling a second serial line can cause
some odd behavior.

Also, while I'm thinking about it, DON'T USE digitalWrite() IN AN INTERRUPT EVER. Stupid thing takes 50 clock cycles on its
own. Bitwise operators are your friend. Look to the code for some simple examples.

== What else can this do?==

I'm glad you asked, header! If one wanted to, this is *very* easy to adapt into an NES controller as well! Mostly because
the SNES and NES controller protocols are identical for timing and the latch behavior. It just polls for fewer buttons.
Even better, the fewer buttons are a single byte! Much faster on the processing end, if you put the modification in.
Anyway, if you are interested in making this modification, the following pinout and data order may be of interest:

== NES Contoller Port == 
        
                  +----> Power
                  |
            5 +---------+  7    
              | x  x  o   \     
              | o  o  o  o |    
            4 +------------+ 1  
                |  |  |  |
                |  |  |  +-> Ground
                |  |  +----> Pulse
                |  +-------> Latch
                +----------> Data  
                
== NES Controller Order ==

        Clock Cycle     Button Reported
        ===========     ===============
        1               A
        2               B
        3               Select
        4               Start
        5               Up on joypad
        6               Down on joypad
        7               Left on joypad
        8               Right on joypad

You might notice that these orders are almost exactly the same. Isn't that handy?
