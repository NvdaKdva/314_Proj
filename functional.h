#pragma once

#include <iostream>
#include <limits>
#include <cstring>
#include <time.h>
#include <string>

/*
 *A header file for free floating utility functions
*/

//Prints a line of stars
void Divider();

//Invokes the system clear command
void ClearScreen();

//For use after cin>> or get, extra input is ignored and cleared
void Safety();

//Pauses program until user or client presses enter, discards any input
void Wait4Enter();

//Copies an array of char from source to dest with various common error guards
bool charr_cp(char * & dest, const char * source);

//returns the number of a choice, forces a choice between low - high inclusive
int Choice(int low, int high);

//returns true for y and false for no, forces valid input
bool YorN();

