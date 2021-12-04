//----------------------------------------------------------------------
// Titel	:	LTC1380.c
//----------------------------------------------------------------------
// Sprache	:	C
// Datum	:	25.09.2020
// Version	:	1.0
// Autor	:	Diveturtle93
// Projekt	:	Batteriemanagement-System
//----------------------------------------------------------------------

// Einfuegen der standard Include-Dateien
//----------------------------------------------------------------------

//----------------------------------------------------------------------

// Einfuegen der eigenen Include Dateien
//----------------------------------------------------------------------
#include "ltc1380.h"
#include "ltc6811.h"
#include "error.h"
//----------------------------------------------------------------------

// Channel auswaehlen
//----------------------------------------------------------------------
void ltc1380_write(uint8_t Address, uint8_t Channel)
{
	// Debug Nachricht
#ifdef DEBUG_LTC1380
	ITM_SendString("Aufruf von Transceive LTC1380.\n");
	ITM_SendString("Kanal auf IC einstellen.\n");
	ITM_SendString("IC:\t");
	ITM_SendNumber(Address);
	ITM_SendString("\t\tKanal:\t");
	ITM_SendNumber(Channel);
	ITM_SendString("\n");
#endif

	// Definiere Array fuer Channel auswaehlen
	uint8_t off[6] = {0x69, 0x08, 0x00, 0x89, 0x7F, 0xF9};					// Daten-Bytes fuer LTC6811 mit Daten fuer LTC1380
	
	// Addresse und Channel waehlen
	off[1] |= ((Address & 0x0F) << 4);										// Address in Array uebertragen
	off[3] |= ((Channel & 0x0F) << 4);										// Channel in Array uebertragen
	
	// LTC6811 beschreiben und ueber I2C uebertragen
	ltc6811_write(WRCOMM, off);												// Daten in LTC6811 schreiben
	ltc6811(STCOMM);														// Daten von LTC6811 an LTC1380 senden

	// Debug Nachricht
#ifdef DEBUG_LTC1380
	ITM_SendString("Daten wurde gesendet.\n");
	ITM_SendString("Folgendes wurde gesendet:");

	// Sende Daten auf UART
	for (uint8_t i = 0; i < 6; i++)
	{
		ITM_SendString(" ");
		ITM_SendNumber(off[i]);
	}
	ITM_SendString("\n");
#endif
}
//----------------------------------------------------------------------

// All Channel off
//----------------------------------------------------------------------
void ltc1380_off(uint8_t Address)
{
	// Debug Nachricht
#ifdef DEBUG_LTC1380
	ITM_SendString("Aufruf LTC1380 Funktion, alle Kanaele von IC ");
	ITM_SendNumber(Address);
	ITM_SendString(" ausschalten.\n");
#endif

	// Definiere Array fuer alle Kanaele aus
	uint8_t off[6] = {0x69, 0x08, 0x00, 0x09, 0x7F, 0xF9};					// Daten-Bytes fuer LTC6811 mit Daten fuer LTC1380
	
	// Addresse waehlen
	off[1] |= ((Address & 0x0F) << 4);										// Address in Array uebertragen
	
	// LTC6811 beschreiben und ueber I2C uebertragen
	ltc6811_write(WRCOMM, off);												// Daten in LTC6811 schreiben
	ltc6811(STCOMM);														// Daten von LTC6811 an LTC1380 senden

	// Debug Nachricht
#ifdef DEBUG_LTC1380
	ITM_SendString("Daten wurde gesendet.\n");
	ITM_SendString("Folgendes wurde gesendet:");

	// Sende Daten auf UART
	for (uint8_t i = 0; i < 6; i++)
	{
		ITM_SendString(" ");
		ITM_SendNumber(off[i]);
	}
	ITM_SendString("\n");
#endif
}
//----------------------------------------------------------------------


// All Multiplexer off
//----------------------------------------------------------------------
void ltc1380_alloff(void)
{
	// Debug Nachricht
#ifdef DEBUG_LTC1380
	ITM_SendString("Aufruf LTC1380 Funktion, alle LTC1380 ausschalten.\n");
#endif

	// Definiere Array fuer alle Multiplexer aus
	uint8_t off[6] = {0x69, 0x08, 0x00, 0x09, 0x7F, 0xF9};					// Daten-Bytes fuer LTC6811 mit Daten fuer LTC1380
	
	// Schleife um Daten an LTC6811 zu uebertragen
	for (uint8_t i = 0; i < LTC1380_DEVICES; i++)
	{
		// Addresse waehlen
		off[1] |= (((i + 1) & 0x0F) << 4);									// Address in Array uebertragen
		
		// LTC6811 beschreiben und ueber I2C uebertragen
		ltc6811_write(WRCOMM, off);											// Daten in LTC6811 schreiben
		ltc6811(STCOMM);													// Daten von LTC6811 an LTC1380 senden

		// Debug Nachricht
	#ifdef DEBUG_LTC1380
		ITM_SendString("Daten wurde gesendet.\n");
		ITM_SendString("Folgendes wurde gesendet:");

		// Sende Daten auf UART
		for (uint8_t i = 0; i < 6; i++)
		{
			ITM_SendString(" ");
			ITM_SendNumber(off[i]);
		}
		ITM_SendString("\n");
	#endif
	}
}
//----------------------------------------------------------------------
