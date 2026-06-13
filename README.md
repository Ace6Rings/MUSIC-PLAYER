# NOIR MUSIC PLAYER

<img width="2547" height="3296" alt="FINAL ASSEMBLY (1) (1)" src="https://github.com/user-attachments/assets/ad429f9a-fe4f-466c-a10e-2c4053002d3a" />

# Why?

Why did I build something like this? Well, I am sick and tired of buying spotify premium every month. I looked for mp3 players online but nearly all of them have the same flaw(s). 
They either have a very short battery life or they don't support bluetooth. This would mean that my headphones will last three times as long as a typical mp3 player. So, I solved all of these problems with my own music player.

# Features

  -expandable storage via micro sd card
  -bluetooth range of ~100 meters in open air
  -Dual compatibility with both wired and wireless. 
  -High quality wired and wireless audio
  -Extremely high battery life (not tested)

# Electronics

So what about my electronics makes these features possible?

I use the NRF5430 <br>
  -The NRF5430 is a dual core chip that have the capability of being hyper efficient during its operation. <br>
  -Its power consumption is measured in the microamps during sleep mode and miliamps during full operation.

The PCM5102A and the TPA6132A2RTE <br>
  -the audio from common microcontrollers are low quality and unenjoyable for music. So I used the PCM5102A. It is a high quality DAC which will be wired to the TPA6132ARTE. <br>
  -the TPA6132ARTE increases the volume of the sound and also helps with the bass.<br>
  -with these two components, the wired quality should be no different (if not, better) than a phone or a typical mp3 player.

What about some safety features?

I have 2 layers of protection.

  According to my schematic, the battery and the usbc is tied to the same net. This means that they share the same electrical path.<br>
  To prevent any damage, I used mosfets which prevents both of them from turning on at the same time.<br>
  Additionally, I used the PRTR5V0U2X. It is a tvs diode which can prevent damage to the usbc and the board if something goes horribly wrong.

The rest of the electronics can be found in the electronics subdirectory. You can also find the production files there too.
<img width="2547" height="3296" alt="step 1 (2)" src="https://github.com/user-attachments/assets/80affd2f-d546-4a83-a986-221c5e95ff83" />

# Mechanical

The case of the music player is largely modeled after the Iphone 16 pro's dimensions. It is a reasonable size for a mp3 player and is also the reason why I can fit such a big battery.

There isn't any special about the case itself. This is designed for printers like the A1 mini, A1, and X1 Carbon with PLA basic.
If you took a look at my CAD already, you might be worried about how thin the wall is at the micro sd slot. So I ran some simulations.

I put a force of 20 newtons onto the faces of the socket and made the opposite wall a fixed support.
There could be more things you might want to see. You can do that under MECHANICAL and then click on simulations. <br>
NOTE: you need both the workbench file and also the zipped file too
CONTROL
<img width="1505" height="633" alt="control" src="https://github.com/user-attachments/assets/655355c9-4f56-4fcd-af4c-0c464f9b39e5" />
This is the "before" part of the simulation.
<img width="1512" height="632" alt="defform" src="https://github.com/user-attachments/assets/3b59fc39-7af3-48ba-9706-4b7c76378423" />
And this is the after. 

Pretty much the same right? this means that my design is sound and could be used (as long as the user isn't a brute)


# Code

The backend of the code is built entirely by NORDIC, the company behind the NRF chips. I used their existing audio code so that I don't need to code my own (they have tens of thousands of lines of code)

The GUI is a WIP. I will do it after the PCB arrives so I can mess around with the settings. As of right now, only audio playing is implemented.



  
