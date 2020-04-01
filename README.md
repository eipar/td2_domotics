# td2_domotics

This is a 2015 project for the subject Digital Techniques II of the Electronic Engineering degree of the Universidad Tecnológica Nacional Facultad Regional Buenos Aires (UTN-FRBA).

Using a fingerprint reader, you can enter a room and configure it according to the preferences of the person. Each person can configure lightning preferences, an automatic blind, a fan and a heater. Thus, light and temperature sensores were used. Information about people's logging in or out is saved in a .txt file on a SD, no, there is no encription whatsoever.

A LPC1769 Stick is used as a microcontroller for all above mentioned, and it communicated through a serial port with the computer, where all user data is sent/received. 
The user interface was done with QTCreator. There is one admin which can change any other user information, add, delete and modify each user's profile. That is, to enter the program, each user will have username and password. This data is encrypted and saved on the computer. 

Hardware wise, as previously mentioned, a LPC1769 Stick is the main component. The board is power supplied by a 12V transformer, and thus, two regulators (LM78M05 and LM1117) to obtain 5V and 3.3V were used. Relés were added to control the lightning, ventilation, etc. 

Limit switches provide information about the DC blind and door motors. Both motors were control by an L293D IC.
