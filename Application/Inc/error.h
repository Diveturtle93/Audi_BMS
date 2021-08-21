//----------------------------------------------------------------------
// Titel	:	Error.h
//----------------------------------------------------------------------
// Sprache	:	C
// Datum	:	16.01.2021
// Version	:	1.0
// Autor	:	Diveturtle93
// Projekt	:	Batteriemanagement-System
//----------------------------------------------------------------------

// Dateiheader definieren
//----------------------------------------------------------------------
#ifndef ERROR_H_
#define ERROR_H_
//----------------------------------------------------------------------

// Festlegen der Fehlercodes
//----------------------------------------------------------------------
#define ERROR_NONE								0
#define ERROR_LTC6811_THERMAL					1
#define ERROR_LTC6811_TEST1						2
#define ERROR_LTC6811_TEST2						3
#define ERROR_LTC6811_DIAGN						4
#define ERROR_LTC6811_OPENWIRE					5
#define ERROR_LTC6811_OVERVOLT					6
#define ERROR_LTC6811_UNDERVOLT					7
//----------------------------------------------------------------------

// Funktionen definieren
//----------------------------------------------------------------------
void hal_error(uint8_t status);
void software_error(uint8_t errorcode);
void ITM_SendString(char *ptr);
//----------------------------------------------------------------------

#endif /* INC_ERROR_H_ */
//----------------------------------------------------------------------
