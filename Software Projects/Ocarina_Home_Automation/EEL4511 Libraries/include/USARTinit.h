#ifndef USART
#define USART

/*** p r o t o t y p e s ***/

__interrupt void inCharISR();

void interruptInit();
void timerInit();
void outChar(char c);
void outWord(char* word);

void USARTinit();
void USARTtest();
void startUSART();

#endif
