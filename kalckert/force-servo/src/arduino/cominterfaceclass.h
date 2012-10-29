/*    
    Communication protocol for the Arduino. Send an command in the form of
    X#.#; where X is a character, # are numbers, and . marks the decimal.
    Copyright (C) 2012 Alexander Skoglund, Karolinska Institute
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */

#ifndef ComInterfaceClass_h
#define ComInterfaceClass_h

#include "Arduino.h"
#include <Serial.h>

class ComInterfaceClass
{
	public:
	ComInterfaceClass();
    ~ComInterfaceClass();
    int    ReadCommand();
    char   getChar(){return command_; };
    int    getInt(){return set_value_; };
    float  getDouble(){return dec_; };
    //    getValue(){return set_value_; };
	void   ClearVars();
	
	private:
	int init();
	int intLength(int lvalue);
	
	int ready_;
	int reciveValue_;
	int set_value_;
	int value_;
	float value_float_; 
	int floatflag_;
	float dec_;
	char command_;

};

#endif