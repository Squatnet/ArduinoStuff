Preparations for the second development harness.

Aim: To design and build a modular lighting system that can interchangeably upto support 128 outputs for use in a variety of outdoor environments.

Requirements:
	1. All Enclosures, Profiles and Harnessing must adhere to ip67/8 watertight standards.
	2. Half/Full RS485 Duplex Network System that can send and receive to no less than 8 Slaves at a distance of upto 250m.
	3. Be able to send and receive all network information wirelessly, relaying information over large distances and to multiple 			slaves.
	4. Hardware and software administration panels with full control of all elements in network.
	5. Work [or be easily converted] from multiple methods of input communication (MIDI/DMX/OSC/UDP/BT etc).
	
*** ALL NETWORK DESIGNS TO BE BASED AROUND THE PADDED JITTERING OPERATIVE NETWORK (PJON) ***

DV2 Network Example:
		 MATRIX BUS 2	   LED HUB 1	    LED HUB 3			
			|		|		|		
HOST -------------------------------------------------------------------- CONVERTER
   |		|		|		|		|
   |	   MATRIX BUS A	    RS485 RELAY	    LED HUB 2	    LED HUB 4	
   |				|
  APK			    ----------
			    |	     |	
			BRANCH A - BRANCH B

Host Enclosure: 
	* To receive via HC05 and Wiznet5100 (OSC)
	* TFT shield to display raw string data.
	* I2c link to rPI / oDroid MCU display
	* To send via RS485 network (upto 4 Branches)
	* To send via SoftwareBitBang Strategy, one wire and common gnd (Pin 12 wherever possible)

Matrix Busses:
	* To receive via PJON on lightweight MCU and bus to a branch of 4 2560 matrices and one teensey 3.2.
	* DC Power lines rated to <40a

LED HUBs:
	* To receive via PJON on leightweight MCU and bus to a branch of 4 ProMini MCU's with 16/32 LED outputs
	* Rated to <10a

Converter:
	*Outputs for any uncommon methods eg OSC/UDP for VURF, MIDI/DMX data, Wireless data for esp8266 relays etc.
