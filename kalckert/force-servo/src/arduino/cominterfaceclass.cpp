#include "cominterfaceclass.h"


ComInterfaceClass::ComInterfaceClass()
{
	Serial.begin(9600);
	init();
}

ComInterfaceClass::~ComInterfaceClass()
{
	
}

void ComInterfaceClass::ClearVars()
{
}

int ComInterfaceClass::init()
{

	ready_ = 0;
	reciveValue_= 0;
 	floatflag_ = 0;
	
	return 1;
}


int ComInterfaceClass::ReadCommand()
{
	//Serial.println("ReadCommand");
	int ch;
	//int 
	ready_ = 0;
	long time_start = millis();  
	
	long tmax = 10;
	long dur;
	
	dur = millis() - time_start;
	//while( (ready_ == 0) || (dur < tmax) )
	while( (ready_ == 0) && (dur < tmax) )
	{
		if ( Serial.available() )
  		{	
    		ch = Serial.read();
	    	if( ch == ';')
    		{
    			reciveValue_ = 0;
    	  		set_value_ = value_;   		
    	  		ready_ = 1;
    	  		if (floatflag_ == 1)
    	  		{
					dec_ = value_float_/( pow(10,intLength(value_float_) ) );
        			floatflag_ = 0;
        			set_value_ = value_ + dec_ ;   		
      			}
      			value_ = 0;   
      			value_float_ = 0;
    		}
			//else if( ch >= '0' && ch <= '9' )
			else if( isDigit(ch) )
    		{
    			if(reciveValue_ == 1)
      			{
        			if (floatflag_ == 1)
		    			value_float_ = (value_float_*10)+(ch - '0');	    
					else
	    				value_ = (value_*10)+(ch - '0');
    			}
    		}
    		else if( ch == '.' )
    		{
      			floatflag_ = 1;
      		} // else if( ch >= 'A' && ch <= 'Z' ) // Capital letter
    		else if( isAlpha(ch) )
    		{
      			command_ = ch;
      			reciveValue_ = 1;
    		}
  		}
  		dur = millis() - time_start; 
  	}
  	//set_value_ = 
	return(ready_);
}


int ComInterfaceClass::intLength(int lvalue)
{
  int valLen = 0;

  if(lvalue > 9999)
    valLen = 5;
  else if(lvalue > 999)
    valLen = 4;
  else if(lvalue > 99)
    valLen = 3;
  else if(lvalue > 9)
    valLen = 2;
  else
    valLen = 1; 
  
  return valLen;

}