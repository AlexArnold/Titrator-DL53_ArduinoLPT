/*************************************************************************************************************************************************************
 * PrinterCaptureInterrupt.ino
 * Monitor a parallel port printer output and capture each character. Output the  character on the USB serial port so it can be captured in a terminal program.
 * By............: Paul Jewell
 * Date..........: 29th January 2015
 * Version.......: 0.1a
 * Modification : Change Interrupt Routine so Arduino respond "faster" to Printer Writing Busy Signal directly from interrupt routine
 *                Ecirbaf 12 Jan 2017 Test on a PC with a "generic Printer Text only" printer Printing a test page OK
 *                Even if using somme accent characters    Depend How your Terminal Software is handling that.
 *------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Wiring Layout
 * Parallel Port Output               Arduino Input
 * --------------------               -------------
 * Name      Dir.   Pin                Name    Pin
 * ----      ----   ---                ----    ---
 * nSTROBE    >       1................INT0      2 (as interupt)
 * DATA BYTE  >     2-9.......................3-10   
 * nACK       <      10.........................11
 * BUSY       <      11.........................12
 * OutofPaper <      12................GND
 * Selected   <      13.................5v
 * GND        <>  18-25................GND
 * ----------------------------------------------------------------------------------------------------------------------------------------------------
 *********************  Timing Event Order  *************************
  (A) DATA must be valid prior to STROBE going LOW.
  (B) STROBE* must go LOW for 1.5usec +/- 500nsec, then go HIGH.
  (C) BUSY must go HIGH within 500nsec of STROBE going LOW.
  (D) When the printer is ready to acknowledge the data, ACK* must go LOW for at least 5usec before going HIGH.
  (E) BUSY must go LOW within 5usec of ACK* going LOW.
  (F) ACK* must go HIGH within 5usec of BUSY going LOW.
  (G) DATA may go invalid after ACK goes HIGH.
  ----------------------------------------------
  Abbreviation     Name                Code (Hex)
  ----------------------------------------------
      NUL          Null                0x00
      LF           Line Feed           0x0A
      ESC          Escape              0x1B
      GS           Group Separator     0x1D
  ----------------------------------------------    
 ************************************************************************************************************************************/
//#define LEN 256
#include <SPI.h>
#include <TimerOne.h>

int nStrobe = 2;
int Data0   = 3;
int Data1   = 4;
int Data2   = 5;
int Data3   = 6;
int Data4   = 7;
int Data5   = 8;
int Data6   = 9;
int Data7   = 10;
int nAck    = 11;
int Busy    = 12;

//enum States {
//  READY,
//  BUSY,
//  ACK,
//  STDBY
//} State;

enum States {
  READY,
  BUSY,
  ACK
} State;

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------

void setup() {
  //for (int n = Data0; n < (Data7+1); n++) {
  //  pinMode(n, INPUT_PULLUP);
  //}
  // Configure pins
  pinMode(nStrobe, INPUT_PULLUP);
  pinMode(Data0, INPUT_PULLUP);
  pinMode(Data1, INPUT_PULLUP);
  pinMode(Data2, INPUT_PULLUP);
  pinMode(Data3, INPUT_PULLUP);
  pinMode(Data4, INPUT_PULLUP);
  pinMode(Data5, INPUT_PULLUP);
  pinMode(Data6, INPUT_PULLUP);
  pinMode(Data7, INPUT_PULLUP);
  pinMode(22, INPUT_PULLUP);
  pinMode(23, INPUT_PULLUP);

digitalWrite(nStrobe, LOW);
digitalWrite(Data0, LOW);
digitalWrite(Data1, LOW);
digitalWrite(Data2, LOW);
digitalWrite(Data3, LOW);
digitalWrite(Data4, LOW);
digitalWrite(Data5, LOW);
digitalWrite(Data6, LOW);
digitalWrite(Data7, LOW);

digitalWrite(22, LOW);
digitalWrite(23, LOW);
  
  pinMode(nAck, OUTPUT);
  pinMode(Busy, OUTPUT);
 
  Serial.begin(9600);    // ** Actual com port could at least go this speed  ** //
  //while (!Serial) {   ;   }
 
  attachInterrupt(0,Interrupt,FALLING);  // ** Name :Interrupt,  was clearer for me than DataReady  ** //

  State = READY;
  delay(100);
  Serial.println("Initialised");
}
//-----------------------------------------------------------------------------------------------------
               
void loop() {
  Serial.print(State);
  Serial.print(" ");
  Serial.print(digitalRead(nStrobe));
  Serial.print(" ");
  Serial.print(digitalRead(nAck));
  Serial.print("    ");
  Serial.print(digitalRead(22));
  Serial.print(" ");
  Serial.print(digitalRead(23));
  Serial.print("             ");
  
//  Serial.print(analogRead(A0));
//  Serial.println("-");
//  Serial.print(analogRead(A1));
//  Serial.println("-");
//  Serial.print(analogRead(A2));
//  Serial.println("-");
//  Serial.print(analogRead(A3));
//  Serial.println("-");
//  Serial.print(analogRead(A4));
//  Serial.println("-");
//  Serial.print(analogRead(A5));
//  Serial.println("-");
  

  switch (State) {
    case READY:
      Serial.print("++READY++");
      digitalWrite(Busy, LOW);
      digitalWrite(nAck, HIGH);
      //Serial.println("");
      break;
     
    case BUSY: // nStrobe signal received by interrupt handler
      Serial.print("++BUSY++");
      digitalWrite(Busy, HIGH);
      ProcessChar();
      State = ACK;
      break;
// ** All this case is made during Interrupt (Avoid some missed characters with "fast" printer) **
 
    case ACK:
      Serial.print("++ACK++");
      digitalWrite(nAck,LOW);
      delay(1); //milliseconds. Specification minimum = 5 us    ** Reduced to 1 is ok **
      State = READY;
      break;
  }
//Serial.println("");     
}
//------------------------------------------------------
void Interrupt() {
    digitalWrite(Busy, HIGH); 
    ProcessChar();
    State = ACK;
}
//------------------------------------------------------

void ProcessChar() {
  Serial.print("-");
  Serial.print(State);
  Serial.print(" ");
  Serial.print(digitalRead(nStrobe));
  Serial.print(" ");
  Serial.print(digitalRead(nAck));
  Serial.print(" ");
  Serial.print(digitalRead(22));
  Serial.print(" ");
  Serial.print(digitalRead(23));
  Serial.print("----");

  byte Char;
 
  Char = digitalRead(Data0) +
         (digitalRead(Data1) << 1) +
         (digitalRead(Data2) << 2) +
         (digitalRead(Data3) << 3) +
         (digitalRead(Data4) << 4) +
         (digitalRead(Data5) << 5) +
         (digitalRead(Data6) << 6) +
         (digitalRead(Data7) << 7);
Serial.print(digitalRead(Data0));
Serial.print(digitalRead(Data1));
Serial.print(digitalRead(Data2));
Serial.print(digitalRead(Data3));
Serial.print(digitalRead(Data4));
Serial.print(digitalRead(Data5));
Serial.print(digitalRead(Data6));
Serial.print(digitalRead(Data7));
  Serial.print("___________");
  Serial.print(Char, HEX); 
  Serial.print("___");
  Serial.print(Char, OCT);
  Serial.print("___");
  Serial.print(Char, DEC);
  Serial.print("___");
  Serial.print((char)Char);
Serial.println("");

}
