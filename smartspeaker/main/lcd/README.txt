Firstly Include the header:

#include "lcd/lcd_man.h"


Then in your code initialise the lcd:

lcd_init();


Now you can use the following fucions:

void lcd_clear(line); 
    line ranges 0-3 to clear that line.

void lcd_fullclear(); 
    clears all lines.

void lcd_write(text, x, y, clear); 
    writes the text string, 
    onto the specified location x and y, (0-3, 0-19), 
    the clear is a bool, when true then it clears the line give at y.