//----------------------------------------------------------------------
// Titel	:	my_math.h
//----------------------------------------------------------------------
// Sprache	:	C
// Datum	:	30.01.2022
// Version	:	1.0
// Autor	:	Diveturtle93
// Projekt	:	BatteriemanagementSystem
//----------------------------------------------------------------------

// Dateiheader definieren
//----------------------------------------------------------------------
#ifndef INC_MATH_H_
#define INC_MATH_H_
//----------------------------------------------------------------------

// Include-Dateien einfuegen
//----------------------------------------------------------------------
#include "main.h"
//----------------------------------------------------------------------

// Define Debug Symbols
//----------------------------------------------------------------------
//#define DEBUG_MATH
//----------------------------------------------------------------------

// Funktionen definieren
//----------------------------------------------------------------------
static int32_t calculateMovingAverage(uint16_t oldValue, uint16_t newValue, uint16_t n);		// Gleitenden Mittelwert berechnen
static uint16_t getDifference(uint16_t a, uint16_t b);						// Differenzberechnung
//----------------------------------------------------------------------

#endif /* INC_MATH_H_ */
//----------------------------------------------------------------------
