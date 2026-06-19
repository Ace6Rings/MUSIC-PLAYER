SOME NOTES FOR THE REVIEWER 

B12 and B14 are intentionally connected to provide a path for B14 to go out. B12 serves no purpose and is physically disconnected according to the Nordic datasheet at
https://www.mouser.com/datasheet/2/297/nRF5340_OPS_v0_5_1-1929595.pdf 
There is also an official source at https://devzone.nordicsemi.com/f/nordic-q-a/61084/nrf52840-reset-pin-routing-to-avoid-pad-on-via which was replied to by "Simonr,"
a Nordic support engineer.

You will likely run the drc and find errors about the courtyard for the crystal. Its just a cosmetic issue.

Silkscreen labels for important/unique components are included. Since its so dense, I did not include any for passive components.


