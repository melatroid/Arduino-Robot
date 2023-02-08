

////////////////////////////////////////////////////
//KAI//SCHMITT///DEVELOPMENT////////////////////////
//KW//49////////////////////////////////////////////
////////////////////////////////////////////////////
//AENDERUNGSINDEX///////////////////////////////////
//2021//09//15//BETA TEST
//2021//10//18//INTEGRATION LINEARVERFAHREINHEIT
//2021//11//15//MENUFÜHRUNG
//2021//11//20//ERWEITERUNG OPTIONSMENUE
//2021//12//08//NEUE NAMENSINDEX
//2021//21//12//NOTAUSINTEGRATION
//2022//09//01//ZWISCHENSTAND
//2022//08//05//INTEGRATION NEUER APPLIKATIONEN
//2022//28//06//INTEGRATION HANDBEDIENUNG
//2022//05//09//PROZESSTEST SOP (SEQUENZ OPERATION)
//2022//05//10//SD CARD READER INTEGRATION
//2022//20//10//SD CARD INPUT-OUTPUT LOGIC
//2022//20//10//ROBOT DRIVING PROGRAM
//2022//02//11//ROBOT IS TEACHABLE
//2022//20//11//BUG FIXES
//2022//13//12//DEBUGGER
//2023//01//01//ADD TEACHMODUS INTEGRATION: CREATE A NEW POSITION BEVOR OR AFTER, OVERWRITE POSITION, DRIVE BACKWARTS, DRIVE FORWARDS
//2023//20//01//INTEGRATION ADVANCED EDITOR
///////////////////////////////////////////////////////////////////////////
////////////////////////////BIBLIOTHEKEN///////////////////////////////////
///////////////////////////////////////////////////////////////////////////
#include <VarSpeedServo.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SdFat.h> 
#include "display.h"


//#include <Arduino.h>
//#include <stdlib.h>
///////////////////////////////////////////////////////////////////////////
/////////////////////PIN_DEKLERATION/////SERVOMOTOREN//////////////////////
///////////////////////////////////////////////////////////////////////////
VarSpeedServo achse1;            			//Achse 1 ist ein Servomotor
VarSpeedServo achse2;           			//Achse 2 ist ein Servomotor
VarSpeedServo achse3;           			//Achse 3 ist ein Servomotor
#define PIN_SERVO1 8            			//Achse 1 am PIN 8
#define PIN_SERVO2 9            			//Achse 2 am PIN 9
#define PIN_SERVO3 7           				//Achse 3 am PIN 7
///////////////////////////////////////////////////////////////////////////
/////////////////////PIN_DEKLERATION/////LINEARACHSE///////////////////////
///////////////////////////////////////////////////////////////////////////
bool    debugger = true;              //DEBUGGER ACTIV
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

uint8_t ENABLE = 16;              			//Pin für Aktivierung der Motorsteuerung
uint8_t PULS = 15;                     		//Pin für Pulsweite für Geschwindigkeit
uint8_t DIRECTION = 14;               		//Pin für Bewegungsrichtung
uint16_t SCHRITTE;               			//Programminterne Variable
uint16_t linearlaenge;             			//Programminterne Variable
uint8_t anlauf;                 			//Programminterne Variable
uint8_t anlaufspeed;           	   			//Programminterne Variable

///////////////////////////////////////////////////////////////////////////
//////////////////// PIN_DEKLERATION//EINGAENGE////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint8_t TASTER01 = 22;          			//Taster am PIN 22
uint8_t TASTER02 = 23;          			//Taster am PIN 23
uint8_t RUNTER = 44;          				//Taster am PIN 44
uint8_t HOCH = 45;          				//Taster am PIN 45
uint8_t ACHSWECHSEL = 42;          			//Taster am PIN 42
uint8_t AUTOMANUEL = 35;          			//Taster am PIN 51
uint8_t NOTAUSSCHALTER = 34;        	    //NOTAUS am PIN 27
uint8_t ENDSCHALTER_LINKS = 24;     	 	//Endschalter Links am PIN 24
uint8_t ENDSCHALTER_RECHTS = 25;    	    //Endschalter Rechs am PIN 24
uint8_t MAGNETSCHALTER_ROBOTER = 26;	    //Kugelabfrage am Roboter am PIN 26
uint8_t INITZIATOR_MAGNETSPIEL = 27;	    //Initziator Kugelspiel

bool MAGNETSCHALTER_WERT;            		//Programminterne Statusabfrage
bool teachActDisplay = 1;      				//Programminterne Statusabfrag
bool TASTER01WERT;               			//Programminterne Statusabfrage
bool TASTER02WERT;              			//Programminterne Statusabfrage
bool ENDSCHALTER_LINKSWERT;        			//Programminterne Statusabfrage
bool ENDSCHALTER_RECHTSWERT;      			//Programminterne Statusabfrage
///////////////////////////////////////////////////////////////////////////
////////////////////PIN_DEKLERATION//AUSGAENGE/////////////////////////////
///////////////////////////////////////////////////////////////////////////
#define STATUSANZEIGE_ROTLICHT 30     	//LEDs Rot am Energiepaket
#define STATUSANZEIGE_PROZLICHT 31      //LEDs Prozesslicht
#define MAGNETGREIFER 40              	//Magnetgreifer

//#define STRING 19                         //STRINGVERARBEITUNG

LiquidCrystal_I2C lcd(0x27, 20, 4);         //4x20 Zeilen Display
File robotprogram; 							//HELLO TXT_FILE
SdFat SD;   							    //HELLO SD_CARD

const uint8_t chipSelect = 53; 			 	//SETZE SIGNAL PIN SD_CARD AUF 53
bool sdcheck=false; 						//HCeckstatus for SD card
///////////////////////////////////////////////////////////////////////////
///////////////////PROGRAMMINTERNE VERARBEITUNG////////////////////////////
///////////////////////////////////////////////////////////////////////////
uint8_t robotspeed;                		   	//Geschwindigkeit der Roboterbewegungsprogramms
uint8_t zeitledtest = 133;         		  	//Geschwindigkeit der LEDs
uint8_t referenzierungsstatus;     		 	//Referenzierungsstatus hochsetzen
uint8_t welcomeStat;         				//Willkommenaufruf hochsetzen
uint8_t tasterwahllcdloop;         			//Tasterwahlaufruf LCD hochsetzen
uint8_t flytime;                   			//Geschwindikeit der LCD Bilder
uint8_t testmodus = 9;             			//Testmodus zum Freisetzen
uint8_t optionenlcdweiter;         		  	//Optionsmenue hochsetzen
uint8_t robotspeedsetuplcdweiter;  		  	//Robotsetupspeed menue hochsetzen
uint8_t endstopsetuplcdweiter;     		  	//Endstoptest menue hochsetzen
uint8_t endschalterlinkstest;      			//Endschalterueberpruefung hochsetzen
uint8_t endschalterrechtstest;     		  	//Endschalterueberpruefung hochsetzen
uint8_t lcdbilder_on_off;         			//Ein Aussschalten von Dislay Animationen
uint8_t axis3save;           				//Wertrueckgabe fuer Achse 3++
uint8_t quitierung;           				//Quitierung für Roboter Weiterschaltung
uint8_t animationensetup;       			//Displayanimationen aussschalten
uint8_t notaus;                   			//Notaussignal
uint8_t magnetspiel_wert =0;      			//Zähler für Magnetspiel
uint8_t checksd=0; 							//choose sd action
uint8_t whatPos=0; 							//-> ON ACTUAL MEMORY POSITION
uint8_t checkint=0;							//CHECK IN TXT HOW MUCH POINTS ARE INSIDE
uint8_t prjpoi =0;							//ERSTE PROGRAMMPOSITION AM ANFANG AUSWÄHLEN
uint8_t LINEAR_ACHSE_SPEED = 80;   			//Geschwindigkeit der Linearachse
uint8_t actualPos=0;          				//Uebergebe die aktuelle Position
uint8_t ax=0;                 				//WECHSEL DIE ACHSE
uint8_t whatprog=0;							//Program laden von SD welches?
uint8_t prgcot =0;  		           		// Anzahl der derzeitig verfübaren Programme

int8_t dtp=0;								//Übergabe für Fahre zu Position im Teachmodus

uint16_t realLinearAxis =0;    		  		//Istwert der Linearachse

char axis3chars [3];				 		//Chararray for translating int to char
char axis2chars [3]; 						//Chararray for translating int to char
char axis1chars [3]; 						//Chararray for translating int to char
char axis4chars [6]; 						//Chararray for translating int to char
char axisposchars [3]; 						//Chararray for translating int to char
bool notausquittierung = LOW;   			//Quittierung fuer Notaus7
bool teachInPosition = false;				//Variable fuer Teachinfunktion DriveRobot();
bool saveModusChoose = false;       		//Variable fuer die Anzeigewahl beim speichern
bool checkLastled;    						//OVERWRITE
bool checkLastMagnet; 						//OVERWRITE
bool specifprog = false;					//Variable für Programmladen 
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
	uint8_t prgpoi=0;						//Aktuelle Programmposition ZEIGE AUF EINE PROGRAMMPOSITION
	struct programmspeicher
	{
		uint8_t a1=90;
		uint8_t a2=90;
		uint8_t a3=90; 
		uint16_t a4=0; 
		bool motion=0; 
		uint8_t speedmot=10;
		bool gripper=false;
		bool led1=1;
		bool led2=1;
	}
	  prs[51]; 								//RESERVIERE EINEN PROGRAMMSPEICHER 
	
	uint8_t memoryslots = 51;         		//Sum of Memoryslots
  
	char* meop[6] 							//STRINGS FOR EDITOR
    {
     "      select...    "
    ,"    save program   "
    ,"   select program  "
    ,"   delete program  "
    ,"   delete s-point  "
    ,"     mainmenue     "
    };
	
  String show[9];
    
  struct namen
   {
   String mem;
   } prog[9]; 
  
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

void setup ()
{
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
  lcd.init();
  lcd.begin(0x27, 20, 4);                	//Starte das LCD Display
  lcd.backlight();            				//Starte die Hintergrundbeleuchtung
  Serial.begin(9600);           			//Serielle Schnittstelle 
///////////////////////////////////////////////////////////////////////////
////////////////////////SD KARTENLESER START///////////////////////////////
///////////////////////////////////////////////////////////////////////////  
  Serial.println(F("Initialisiere SD-Karte"));  
	pinMode(53,OUTPUT);
/////////////////////////////////////////////////////////////////////////////
/////////////////////////SERVOMOTOREN//STARTEN///////////////////////////////
/////////////////////////////////////////////////////////////////////////////
    achse1.attach(PIN_SERVO1);        //SERVOMOTOR 1 achse1
      achse1.write(90, 10,true);      //HOME
    achse2.attach(PIN_SERVO2);        //SERVOMOTOR 2 achse2
      achse2.write(90, 10,true);      //HOME
    achse3.attach(PIN_SERVO3);        //SERVOMOTOR 3 achse3
      achse3.write(90, 10,true);      //HOME
/////////////////////////////////////////////////////////////////////////////
////////////////////////SCHRITTMOTOR/////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
    pinMode (ENABLE,             OUTPUT);                    //ENABLE
    pinMode (PULS,               OUTPUT);                    //PULS
    pinMode (DIRECTION,          OUTPUT);                    //DIRECTION
/////////////////////////////////////////////////////////////////////////////
////////////////////////DIGITAL//OUTPUTS/////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
    pinMode(MAGNETGREIFER,      OUTPUT);          			//Magnetgreifer
    pinMode(STATUSANZEIGE_ROTLICHT,   OUTPUT);              //Statusanzeige
    pinMode(STATUSANZEIGE_PROZLICHT, OUTPUT);               //Statusanzeige
/////////////////////////////////////////////////////////////////////////////
////////////////////////DIGITAL//INPUTS//////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
    pinMode (TASTER01,    INPUT_PULLUP);      			//Taster01 ist 10kohm PULLDOWN geschaltet
    pinMode (TASTER02,    INPUT_PULLUP);      			//Taster02 ist 10kohm PULLDOWN geschaltet
    pinMode (RUNTER,      INPUT_PULLUP);      			//RUNTER ist 10kohm PULLDOWN geschaltet
    pinMode (HOCH,        INPUT_PULLUP);      				//HOCH ist 10kohm PULLDOWN geschaltet
    pinMode (ACHSWECHSEL, INPUT_PULLUP);      		//ACHSWECHSEL ist 10kohm PULLDOWN geschaltet
    pinMode (AUTOMANUEL,  INPUT_PULLUP);      		//AUTOMANUEL ist 10kohm PULLDOWN geschaltet                    
    pinMode (ENDSCHALTER_LINKS,  INPUT);    
    pinMode (ENDSCHALTER_RECHTS, INPUT);
    pinMode (MAGNETSCHALTER_ROBOTER, INPUT_PULLUP);
    pinMode (INITZIATOR_MAGNETSPIEL, INPUT_PULLUP);    
    pinMode (NOTAUSSCHALTER, INPUT_PULLUP);     		//NOTAUS ist 10kohm PULLDOWN geschaltet
///////////////////////////////////////////////////////////////////////////
//////////////////////RECHENVARBIABLEN/////////////////////////////////////
/////////////////////////STARTWERTE////////////////////////////////////////
    robotspeed      =15;     			//Geschwindigkeitsdefault
    linearlaenge    = 0;      			//Startwert nicht verändern
    referenzierungsstatus = 0;        	//Startwert nicht verändern
    welcomeStat     = 0;        		//Startwert nicht verändern
    flytime         = 200;    			//Startwert nicht verändern
    optionenlcdweiter     = 0;        	//Startwert nicht verändern
    robotspeedsetuplcdweiter =0;      	//Startwert nicht verändern
    endstopsetuplcdweiter =0;     		//Startwert nicht verändern
    endschalterlinkstest =0;        	//Startwert nicht verändern
    endschalterrechtstest =0;       	//Startwert nicht verändern
    lcdbilder_on_off  =1;       		//Startwert nicht verändern
    animationensetup = 1;        		//Startwert nicht verändern
    notausquittierung = 0;      		//Startwert nicht verändern
    String robotext = "robotprogram";
    String txt = ".txt";
    Serial.println(F(" #### Speicherplatz bereitgestellt #### "));

  for (int i=0; i<9;++i)      // CREATE NAMES
  {
      String robtext (robotext);
                        if(debugger) Serial.println(robtext);     
      String numb (i+1); 
                        if(debugger) Serial.println(numb);      
      String finish = robtext;
                        if(debugger) Serial.println(finish);
      finish.concat(numb);
                        if(debugger) Serial.println(finish);        
      String ergebnis = finish;
      ergebnis.concat(txt);
                        if(debugger) Serial.println(ergebnis); 
       if(debugger) {Serial.println(F(" Erstelle Pruefstring ")); Serial.println(ergebnis); Serial.println(F(" ##### PROGRAM #### "));}
        prog[i].mem = ergebnis;
 }
///////////////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////////////
void loop ()
{
  ///////////////////////////////////////////////////////////////////////////
//////////////////LOOP__VOID///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////

//////////////////Willkommen///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
        ledtest ();           //LED TEST
  if (referenzierungsstatus == 0 && welcomeStat == 0)
  {  
    soundalive();
   // welcomeLcd();                                       //LCD Display
    welcomeStat ++;                               //welcomeStat hochsetzen
  }

///////////////////////////////////////////////////////////////////////////
//////////////////TASTERABFRAGE////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
// CHECKE AM ANFANG OB DIE SD KARTE VORHANDEN IST UND INITZALISIERE DAS PROGRAMM
  
  if (!SD.begin(chipSelect) || sdcheck==false)  
  {
    Serial.println(F("Initialisierung SD Karte fehlgeschlagen!")); 
    noSdCardlcd();  
    delay(300);
    if (SD.begin(chipSelect)) 
    {   
    Serial.println(F("SD Initialisierung Erfolgreich!"));   
		foundSdCardlcd();
		sdcheck = true;
		delay(500);
		
		checkSdProgramlcd ();
		if (checksd == 1)
		{
			Serial.println(F(" Lösche Temporäres Roboterprogramm! "));
			SD.remove ("robottempfile.txt");
		}
		else if (checksd == 2)
		{
    // LESE DEN SD SPEICHER AUS, WENN DAS NICHT KLAPPT BRINGE FEHLER UND LÖSCHE DIE KARTE
		if (readProgfromSD(whatPos)>0) showMemoryonlcd(); // SHOW ON LCD
		else  {showErroronlcd(); Serial.println(F(" Teachmodus aktivieren! ")); SD.remove ("robottempfile.txt"); createNewPath();}// SHOW ON LCD
		}    
	}	
 }

///////////////////////////////////////////////////////////////////////////
//////////////////TASTERABFRAGE////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
 
	if (welcomeStat == 1)
	{
		if (tasterwahllcdloop == 0)
		{
		  tasterwahllcd ();
		  tasterwahllcdloop ++;
		}
	}
  
	TASTER01WERT = digitalRead(TASTER01);         //Abfrage und beschreiben, Taster 1
    TASTER02WERT = digitalRead(TASTER02);        //Abfrage und beschreiben, Taster 2 

   if(digitalRead(AUTOMANUEL)==HIGH && TASTER01WERT == LOW) //SHOW HINT 1
	{
		hint1lcd();
		tasterwahllcdloop = 0;  
	}
///////////////////////////////////////////////////////////////////////////
//////////////////ABFRAGE/////FUER/////INITZALISIERUNG/////////////////////
///////////////////////////////////////////////////////////////////////////
   if (digitalRead(AUTOMANUEL)==HIGH && checkint==0 && referenzierungsstatus == 1) 
	{
	 teachmodus(90,90,90,0,1,20,0,1,0); // STARTWERTE prjpoi,int a,int b, int c, bool motion, int speed, bool gripper, bool led1, bool led2
	}

   if (TASTER01WERT == LOW && referenzierungsstatus == 0 && digitalRead(AUTOMANUEL)==LOW)    // Referenzierungsstatus ist am Anfang 0
	{
     Serial.println(F("################################"));
     Serial.println(F("### Eingabe  Referenzierung  ###"));
     Serial.println(F("################################"));
     ledtest ();
     delay(50);
     initzalisierung();  //AUFRUF
     delay(50);
     referenzierungsstatus ++;                              //Referenzierungsstatus hochsetzen
     tasterwahllcdloop = 0;                               //LCD Displaywert für Programmwahl zuruecksetzen
     delay(50);
	}
  
    TASTER01WERT = digitalRead(TASTER01);         //Abfrage und beschreiben, Taster 1
    TASTER02WERT = digitalRead(TASTER02);        //Abfrage und beschreiben, Taster 2

   if (TASTER01WERT == LOW && referenzierungsstatus == 1 && digitalRead(AUTOMANUEL)==LOW)    // Wenn Taster 01 gedrückt wurde und der Roboter Initzalisiert wurde
    {
      Serial.println(F("################################"));
      Serial.println(F("### Eingabe  Roboterprogramm ###"));
      Serial.println(F("################################"));
       delay(100);
       ledtest ();
       delay(50);
    	
    if (checkint > 0 || prgpoi > 2) driveRobot(); // WENN EIN PROGRAMM VORHANDEN IST, FAHRE DEN ROBOTER
    else noPrgFound();
       
       tasterwahllcdloop = 0;               //LCD Displaywert für Programmwahl zuruecksetzen
       delay(50);
    }

	if(TASTER02WERT == LOW)
	{ 
	  Serial.println(F("################################"));
	  Serial.println(F("### Eingabe ADVANCED EDITOR  ###"));
	  Serial.println(F("################################"));
	programBuildingSettings();
  tasterwahllcdloop = 0; 
	} 

	endstellung();                //Roboter fährt zu HOME
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

void saveProgtoSD (uint8_t whatPos) 		//SPEICHERE EINE TEXT FILE
{

  if (prs[whatPos].a4 != 0)
  {
    Serial.println(F("	Schreibe auf SD... "));
	
	
		if (!specifprog) robotprogram = SD.open("robottempfile.txt", FILE_WRITE); //ÖFFNE SD ZUM LESEN    
    
	
  createWhatPosChars  (whatPos);
  robotprogram.println(axisposchars);
  
  createAxis1Chars  (prs[whatPos].a1);
  robotprogram.println(axis1chars);
  
  createAxis2Chars  (prs[whatPos].a2);
  robotprogram.println(axis2chars);

  createAxis3Chars  (prs[whatPos].a3);
  robotprogram.println(axis3chars);

  createAxis4Chars  (prs[whatPos].a4);
  robotprogram.println(axis4chars);
  
  robotprogram.println(prs[whatPos].motion);
  
  robotprogram.println(prs[whatPos].speedmot);
  
  robotprogram.println(prs[whatPos].gripper); 
  
  robotprogram.println(prs[whatPos].led1);
  
  robotprogram.println(prs[whatPos].led2);
  
  robotprogram.close(); 
  
    Serial.println(F("	Punkt gespeichert !!!	"));  
  }
}
///////////////////////////////////////////////////////////////////////////
int readProgfromSD (uint8_t whatPos) 		//LESE EINE TEXT FILE
{
	//ARRAYS FOR TRANSLATION FROM CHAR TO INT ,  FOR CORRECT TRANLASTING ARRAY MUST ENDING WITH \0
    char whatPosPoi[3];  whatPosPoi[2] = '\0';
    char a1[3];       a1[2] = '\0';
    char a2[3];       a2[2] = '\0';
    char a3[3];       a3[2] = '\0';
    char a4[6];       a4[5] = '\0';
    char speedmot[3]; speedmot[2] = '\0';
    char motion[2];   motion[1] = '\0';
    char gripper[2];  gripper[1] = '\0';   
    char led1[2];     led1[1] = '\0';
    char led2[2];     led2[1] = '\0';
    char ca[2040]; 								 		//PUFFER FOR TEXTREAD
    char checkchar[1]; 							 		//CHECK THE TXT CHAR
     
    //FOR CORRECT ARRAY READING
    int charcount=0; 							 		//COUNT THE CHARS IN ARRAY
    int getpos=0; 								 		//ZÄHLE 38 TEXTSCHIRRTE NACH OBEN COUNTER
    int jump[10] = {0,4,8,12,16,23,26,30,33,36}; 		//TEXT JUMP SETTINGS

    

	//LESE DIE DATEN EIN
	if (specifprog) robotprogram = SD.open(show[whatprog], FILE_READ);
	else robotprogram = SD.open("robottempfile.txt", FILE_READ); //ÖFFNE SD ZUM LESEN
	
	
	robotprogram.seek(0);									 //Setze Dateizeiger auf Anfang
	Serial.println("Schreibe in Arbeitspeicher!");

	while (robotprogram.available())  // READ IN WHILE CHARS AVAILABLE
		{
		   ca[charcount] = robotprogram.read();
		   charcount++;
		}   

	Serial.println (F(" ### \n Ermittle Anzahl")); 
	checkchar[0] = ca[charcount-39];
	checkchar[1] = ca[charcount-38];
	checkint = atoi(checkchar);
	Serial.println (checkint); 
	Serial.println (F("###")); 

	int i=0;
	int j=0;

	for (;j<=checkint;j++) // SHOW WHAT CHARS ARE READED
	{
		Serial.println (F("###FILEPOSITION###"));
		Serial.println (getpos);
		Serial.println (F("## NEW   #########"));

		while (i<=38+getpos)
		{ 
			if ((jump[0]+getpos) == i)
			{  
			  Serial.write (" Speicherplatz = ");
			  whatPosPoi[0] = ca[i];
				whatPosPoi[1] = ca[i+1];  
			  whatPos = atoi(whatPosPoi);
			}
		 
			if ((jump[1]+getpos)  == i)
			{ 
			  Serial.write (" A1 = ");
			  a1[0] = ca[i];
			  a1[1] = ca[i+1];
			  prs[whatPos].a1 = atoi(a1);
			}
			
			if ((jump[2]+getpos) == i)
			{
			  Serial.write ("  A2 = ");
			  a2[0] = ca[i];
			  a2[1] = ca[i+1];
			  prs[whatPos].a2 = atoi(a2);
				  
			}
			
			if ((jump[3]+getpos) == i)
			{
			  Serial.write ("    A3 = ");
			  a3[0] = ca[i];
			  a3[1] = ca[i+1];    
			  prs[whatPos].a3 = atoi(a3);   
			}
			
			if ((jump[4]+getpos)  == i)
			{
			  Serial.write ("      A4 = ");
			  a4[0] = ca[i];
			  a4[1] = ca[i+1];
			  a4[2] = ca[i+2];
			  a4[3] = ca[i+3]; 
			  a4[4] = ca[i+4];  
				prs[whatPos].a4 = atoi(a4); 
			}    
			if ((jump[5]+getpos)  == i)
			{
			  Serial.write ("        Motion = ");
			  motion[0] = ca[i];
			  prs[whatPos].motion = atoi(motion);
			}   
			if ((jump[6]+getpos) == i)
			{
			  Serial.write ("          Speed = ");
			  speedmot[0] = ca[i];
			  speedmot[1] = ca[i+1];
			  prs[whatPos].speedmot = atoi(speedmot);      
			}   
			
			if ((jump[7]+getpos)  == i)
			{
			  Serial.write ("             Gripper = ");
			  gripper[0] = ca[i];
			  prs[whatPos].gripper = atoi(gripper);         
			}   

			if ((jump[8]+getpos)  == i)
			{
			  Serial.write ("               Led1 = ");
			  led1[0] = ca[i];
				   prs[whatPos].led1 = atoi(led1);  
			  
			}   
			if ((jump[9]+getpos)  == i)
			{
			  Serial.write ("                 Led2 = ");
			  led2[0] = ca[i];   
		   
				   prs[whatPos].led2 = atoi(led2);       
			}   
			Serial.write (ca[i]);
			i++;
		}

	  getpos+=39;
  if(debugger)
  {
  	 // PRÄPROZESS
  		Serial.println ("########### SHOW ############");
  		Serial.println (whatPosPoi[0]);
  		Serial.println (whatPosPoi[1]);
  		Serial.println (a1[0]);
  		Serial.println (a1[1]);
  		Serial.println (a2[0]);
  		Serial.println (a2[1]);
  		Serial.println (a3[0]);
  		Serial.println (a3[1]);
  		Serial.println (a4[0]);
  		Serial.println (a4[1]);
  		Serial.println (a4[2]);
  		Serial.println (a4[3]);
  		Serial.println (a4[4]);
  		Serial.println (motion[0]);
  		Serial.println (speedmot[0]);
  		Serial.println (speedmot[1]);
  		Serial.println (gripper[0]);
  		Serial.println (led1[0]);
  		Serial.println (led2[0]);
  		Serial.println ("########### READ ############");
  	}
	}
  	if(debugger)
  {
  		Serial.println (F("\n ### Ergebnis der Einlesung ###"));
  		Serial.println (F("\n Anzahl der Chars: "));
  		Serial.println (charcount);
  		Serial.println (F("\n Anzahl der Position: "));
  		Serial.println (checkint+1);
  		Serial.println (F("\n ### UMWANDLER ###"));
    
  		Serial.println ("#############");
  		Serial.println (" SHOW MEMORY ");
  		Serial.println ("#############");
  
  	for (i=0; i<checkint+1;i++) // SHOW THE WRITEN MEMORY SLOTS
  	{
  		Serial.println(" #########MEMORY########### ");
  		Serial.println(i+1);
  		Serial.println(" ##########RUNS############ ");
  		Serial.println(" Achse 1 ");     
  		Serial.println(prs[i].a1);
  		Serial.println(" 	Achse 2 ");             
  		Serial.println(prs[i].a2);
  		Serial.println(" 		Achse 3 ");       
  		Serial.println(prs[i].a3);
  		Serial.println(" 			Achse 4 ");       
  		Serial.println(prs[i].a4);
  		Serial.println(" 				Motion ");  
  		Serial.println(prs[i].motion);
  		Serial.println(" 					Speed ");       
  		Serial.println(prs[i].speedmot);
  		Serial.println(" 						Greifer "); 
  		Serial.println(prs[i].gripper);
  		Serial.println(" 							Led 1");      
  		Serial.println(prs[i].led1);
  		Serial.println(" 								Led 2");
  		Serial.println(prs[i].led2);  
  	}
  }
	robotprogram.close(); 
	return charcount;
}
///////////////////////////////////////////////////////////////////////////
void teachmodus( uint8_t a,uint8_t b, uint8_t c, uint16_t d, bool motion, uint8_t speedmot, bool gripper, bool led1, bool led2)		//Handbedienungsmodus teachen
{
	uint8_t schwelzeittaster=50;
	bool goaxis = false; 			  //CHECK BOOL FOR BREAK THE SWITCH LOOP
	bool chooseSave = false;
	beleuchtungsledon ();             //Programmbeleuchtung starten
	uint8_t BoA = 0;
	uint8_t check =0;

	teachActDisplay=1;				  //Aktualisieren das Display
	
	/////////////////////////////////
	////TASTERABFRAGE
	/////////////////////////////////

  while (digitalRead(AUTOMANUEL)==HIGH)  // SOLANGE DER TEACH SCHALTER UMGELEGT IST
  { 
  saveAxisFriend(); // CHECK FOR BAD AXIS PARAMETER

  robotactivaxislcd(ax,motion,speedmot,prgpoi+1); //AKTUALISIERE WERTE IM LCD
  
    if(!digitalRead(TASTER01))  //WENN SPEICHERN GEDRÜCKT WURDE, SPEICHERE DIE WERTE
    {
		if (!teachInPosition)
		{
			writePoiToMemory(prgpoi,a,b,c,realLinearAxis,motion,speedmot,gripper,led1,led2); // NEHME DIE WERTE UND SCHREIBE INS ARRAY
			saveProgtoSD (prgpoi);  	//SPEICHER DIE WERTE IN DIE TXT DATEI
			newworld();        			//HELLO SOUND
			pointTeachedlcd();			//LCD ANZEIGE
			teachActDisplay=1; 			//AKTUALISIERE DISPLAY
			prgpoi++;       			//ZEIGE AUF DEN NÄCHSTEN SPEICHERPLATZ, FAHRE ZUR NÄCHSTEN PROGRAMMPOSITION 
			Serial.println (F(" PRJPOI =  "));
			Serial.println (prgpoi);  
			delay (500);
		}
		
		else if (teachInPosition) // TEACH IN FUNCTION, WORKS ONLY WHEN driveRobot();
		{
			chooseSaveOption();
			delay(200);
  			Serial.println(F(" Lösche Inhalt der SD! "));
  			SD.remove ("robottempfile.txt");
			while (!chooseSave)
			{
				if (!digitalRead(TASTER01)) // OVERWRITE THE EXISTET POINT
				{
					writePoiToMemory(actualPos,a,b,c,realLinearAxis,motion,speedmot,gripper,led1,led2); // NEHME DIE WERTE UND SCHREIBE INS ARRAY	
					newworld();     			//HELLO SOUND
					pointTeachedlcd();			//LCD ANZEIGE
					teachActDisplay=1; 			//AKTUALISIERE DISPLAY
	
					for (uint8_t j=0;j<=checkint+1;++j) // Beschreibe den kompletten Speicher auf die SD
					saveProgtoSD (j);  // SAVE ALL POINTS TO SD
					
					readProgfromSD(0); // READ ALL POINTS IN MEMORY			
					chooseSave = true;					
					delay(500);
				}
				
				else if (!digitalRead(TASTER02)) // ADD A NEW POINT IN MEMORY
				{			
					  beforeOrAfterlcd();
					  while (check==0) {
						delay(500);
						if (!digitalRead(TASTER01)) {newworld();BoA = 0; check=1;}
						if (!digitalRead(TASTER02)) {newworld();BoA = 1; check=1;}
					  }
						
						checkint++; //ANZAHL DER PROGRAMMPUNKTE		
            		
									      //KOPIERE DAS SPEICHERREGISTER			  
				for (uint8_t i=memoryslots; i>=actualPos; --i) // SOLL SOLANGE UMKOPIEREN BIS DIE AKTUELLE POSITION ERREICHT IST.
				{
					if (i==50) i=49; // DONT LEAVE THE ARRAY
					// SCHREIBE IN SPEICHERREGISTER
					if (i==actualPos) writePoiToMemory((actualPos+BoA),a,b,c,realLinearAxis,motion,speedmot,gripper,led1,led2); // NEHME DIE WERTE UND SCHREIBE INS ARRAY 
					else
					{
						prs[i+1].a1 = 	  	prs[i].a1;			
						prs[i+1].a2 = 	  	prs[i].a2;
						prs[i+1].a3 = 		prs[i].a3;
						prs[i+1].a4 = 	  	prs[i].a4;
						prs[i+1].motion = 	prs[i].motion;
						prs[i+1].speedmot = prs[i].speedmot;
						prs[i+1].gripper = 	prs[i].gripper;
						prs[i+1].led1 = 	prs[i].led1;
						prs[i+1].led2 = 	prs[i].led2; 
					}
				}
						saveModusChoose = true; // Umschaltung der Textanzeige			
						pointTeachedlcd();			//LCD ANZEIGE
						teachActDisplay=1; 			//AKTUALISIERE DISPLAY
							
						 //WRITE CHANGES TO SD
						for (uint8_t j=0;j<=checkint;++j) // Beschreibe den kompletten Speicher auf die SD
						saveProgtoSD (j);  // SAVE POINTS ONE BY ONE TO SD

						readProgfromSD(0); // READ ALL POINTS IN MEMORY		
						chooseSave = true;
						delay(500);
				}
			}
		}
	}
 
	if (teachInPosition)
    if(!digitalRead(TASTER02)) 	//WENN NEXT GEDRÜCKT WURDE
    { 
      check = 0;
      showJumpOptionsLcd();
        while (check==0)
        {
          delay(500);
          if (!digitalRead(TASTER01)) {newworld(); check=1; dtp = actualPos-1;  if (dtp==0)dtp=(-1); return;} // Drive to last step 
          if (!digitalRead(TASTER02)) {newworld(); check=1; return;} // Drive to Next Step
        }
    }


/////////////////////////////////
////LCD STEUERUNG
/////////////////////////////////
      
     if(teachActDisplay==1) {robotteachsetuplcd(prgpoi,a,b,c,realLinearAxis,motion,speedmot,gripper,led1,led2); teachActDisplay=0;} // DONT REPEAT

/////////////////////////////////
////BEDIENUNGSABFRAGE
/////////////////////////////////

    if (digitalRead(ACHSWECHSEL)==HIGH) {ax++; delay(300);} //Wechsel zwischen den Achsen

    if (8<ax)ax=0; //SWITCH TO NON SELECT
	
    switch (ax) //TASTERABFRAGE
    {
    case 1:
      while (digitalRead(RUNTER)==LOW) 
        {if (a>98)a=97; else if (a==0)a=1;      a++;achse1.write(a, speedmot,true); teachActDisplay=1;delay(schwelzeittaster);}
      
      while (digitalRead(HOCH)==LOW)
        {if (a>98)a=97; else if (a==0)a=1;     a--;achse1.write(a, speedmot,true); teachActDisplay=1;delay(schwelzeittaster);}
        
        break;  
    
    case 2:
      while (digitalRead(RUNTER)==LOW) 
        {if (b>98)b=97; else if (b==0)b=1;   b--;achse2.write(b, speedmot,true); teachActDisplay=1; delay(schwelzeittaster);} 
      
      while (digitalRead(HOCH)==LOW)
        {if (b>98)b=97; else if (b==0)b=1;    b++;achse2.write(b, speedmot,true); teachActDisplay=1; delay(schwelzeittaster);}
        
        break;  
      
    case 3: 
      while (digitalRead(RUNTER)==LOW) 
        {if (c>98)c=97; else if (c==0)c=1;   c--;achse3.write(c, speedmot,true); teachActDisplay=1;delay(schwelzeittaster);}
      
      while (digitalRead(HOCH)==LOW)
        {if (c>98)c=97; else if (c==0)c=1;   c++;achse3.write(c, speedmot,true); teachActDisplay=1;delay(schwelzeittaster);}
        break;  

    case 4: 
    digitalWrite(ENABLE,   LOW);                       //Aktiviere die Motorsteuerung
	
      	do {delay(100);}
      	while (digitalRead(RUNTER)==LOW && digitalRead(HOCH)==LOW); // WENN BEIDE DRÜCKT WERDEN
	
      while(digitalRead(RUNTER)==LOW && digitalRead(HOCH)== HIGH)      
        {    teachActDisplay=1;		//Aktualisieren das Display
        goaxis = true;
        if (!digitalRead(ENDSCHALTER_LINKS)) break;
        realLinearAxis--;
        digitalWrite(DIRECTION,LOW);                       //ROBOTER BEWEGT SICH NACH LINKS
              digitalWrite(PULS,HIGH);
              delayMicroseconds(LINEAR_ACHSE_SPEED);
              digitalWrite(PULS,LOW);
              delayMicroseconds(LINEAR_ACHSE_SPEED);
              notausabfrage ();
        }
          
      while(digitalRead(HOCH)==LOW && digitalRead(RUNTER)== HIGH) 
        {    teachActDisplay=1;		//Aktualisieren das Display
        goaxis = true;
        if (!digitalRead(ENDSCHALTER_RECHTS)) break;
        realLinearAxis++;
        digitalWrite(DIRECTION,HIGH);                       //ROBOTER BEWEGT SICH NACH RECHTS
              digitalWrite(PULS,HIGH);
              delayMicroseconds(LINEAR_ACHSE_SPEED);
              digitalWrite(PULS,LOW);
              delayMicroseconds(LINEAR_ACHSE_SPEED);
              notausabfrage ();
        }

      if (goaxis == true) {goaxis = false; break;} //BREAKT DAMIT KEIN CASE 5 EINTRITT
    
    case 5: 
          if  (digitalRead(HOCH)==LOW) 
        {
		   soundalive ();                         //HALLO
		   prozessledon ();               //PROZESSLED AN
		   magneticGripperOn();             //MAGNETGREIFER SCHLIESSEN
		   gripper = true;        // WRITE STATUS
		   teachActDisplay=1;	//Aktualisieren das Display
		   break;
        }
          if(digitalRead(RUNTER)==LOW) 
        {
		   soundalive ();                         //HALLO
		   prozessledoff ();               //PROZESSLED AUS
		   magneticGripperOff();             //MAGNETGREIFER ÖFFNEN
		   gripper = false;         // WRITE STATUS
		   teachActDisplay=1;	//Aktualisieren das Display
		   break;
        }
        
     case 6: 
          if  (digitalRead(HOCH)==LOW) 
        {
		  soundalive ();                         //HALLO
		  beleuchtungsledon();
		  led1 = true;       // WRITE STATUS
		  teachActDisplay=1;	//Aktualisieren das Display
		  break;
        }
          if(digitalRead(RUNTER)==LOW) 
        {
		  soundalive ();                         //HALLO
		  beleuchtungsledoff();
		  led1 = false;          // WRITE STATUS
		  teachActDisplay=1;	//Aktualisieren das Display
		  break;
        }
    
  case 7: 
        if  (digitalRead(HOCH)==LOW) 
        {
			soundalive ();                         //HALLO
			if (speedmot<30) speedmot +=5;      
			teachActDisplay=1; //Aktualisieren das Display
			break;
        }
        
		if(digitalRead(RUNTER)==LOW) 
        {
		   soundalive ();                         //HALLO
		   if (speedmot>15) speedmot -=5;    
		   teachActDisplay=1;	//Aktualisieren das Display
		   break;
        }

  case 8: 
          if  (digitalRead(HOCH)==LOW) 
        {
       soundalive ();                         //HALLO
       motion = true;
       teachActDisplay=1;	//Aktualisieren das Display
       break;
        }
          if(digitalRead(RUNTER)==LOW) 
        {
       soundalive ();                         //HALLO
       motion = false;
       teachActDisplay=1;	//Aktualisieren das Display
       break;
        }
      break;          
	}
	}
	beleuchtungsledoff ();             //Programmbeleuchtung beenden
    prozessledoff ();        //Prozessled aus
	tasterwahllcdloop =0;
}
///////////////////////////////////////////////////////////////////////////
void checkForPrograms()
{   
    prgcot=0;
    for (int i=0;i<9;++i) show[i] =  "";
    for (int j=0; j<9; j++)      // Search for programs on sd card
    {
        // IF EXIST WRITE NAME IN MEMORY
    if (SD.exists(prog[j].mem))
      {
       show[prgcot] = prog[j].mem;  // SAVE TO ARRAY
     
        if(debugger){Serial.println(F("SHOW MEMORY::")); Serial.println(show[prgcot]);  Serial.println(F("################ Programm auf SD gefunden !!! ###################"));} 
        prgcot++; // COUNT MEMORY UP
      }
       else if (debugger) {Serial.println(F("SHOW MEMORY::")); Serial.println(prog[j].mem); Serial.println(F("+++ nicht gefunden +++"));}
    }
      
    for (int i=0; i<prgcot; i++)
    {
      Serial.println(F(" ### Show finished memory slot:: ###"));
      Serial.println(i);    
      Serial.println(show[i]);
      Serial.println(F(" ###       ###        ###"));      
    }
  }
///////////////////////////////////////////////////////////////////////////
void programBuildingSettings()				//ADVANCED EDITOR
{ 
	newworld();
  delay(1000);
  Serial.println(F("###### EDITOR ######"));
	int ch=0;
	bool exit = false;
	/*
     "      choose...    " 
    ,"    save program   " 1
    ,"   choose program  " 2	
    ,"   delete program  " 3	
    ,"    delete single  " 4
    ,"    robotstatus    " 5
	,"     mainmenue     " 6
	*/
  while (ch==0)
    {
  	while (!exit)	// TASTERABFRAGE HAUPTMENUE
  	{
  		if (ch>=7)ch=1; // MENUEUMBRUCH
    
      lcd.setCursor(0, 0);
      lcd.print("<<<<<<<<< >>>>>>>>>");
      lcd.setCursor(0, 1);
      lcd.print(meop[ch]);
      lcd.setCursor(0, 2);    
      lcd.print("<<<<<<<<< >>>>>>>>>");
      lcd.setCursor(0, 3);
      lcd.print("switch       select"); 
      
  		if 		(!digitalRead(TASTER01)) {newworld();ch++;delay(250);} 
  		else if (!digitalRead(TASTER02)) {newworld();delay(250);exit = true;}
      
  		else {delay(250); if(debugger) Serial.println(meop[ch]);}	
  	}
  	exit = false;
   }
    ///////////////////////////////////////////////////////////////////////////
	if 		(ch==1) {  //name and save program

	checkForPrograms();

    int ch2 =0;
    
		while(!exit)
		{           if(debugger)Serial.println(F(" SAVE PROGRAM ")); 
                if(debugger)Serial.println(ch2); 
                if(debugger)Serial.println(prog[ch2].mem);
		  delay(1000);
			lcd.setCursor(0, 0);
			lcd.print("<<<   SAVE  UP >>>>");
			lcd.setCursor(0, 1);
			lcd.print(prog[ch2].mem);			
			lcd.setCursor(0, 2);
       if (!SD.exists(prog[ch2].mem))
			  lcd.print("<<<<<<<<< >>>>>>>>>");    
       else 
        lcd.print("<<<< is there  >>>");  
      lcd.setCursor(0, 3);
			lcd.print("next         select"); 

		if 		(!digitalRead(TASTER01)) {newworld();ch2++; if(ch2>8)ch2=0; Serial.println(F(" Show::")); Serial.println(prog[ch2].mem);}
		else if (!digitalRead(TASTER02)) 
		{
       if (!SD.exists(prog[ch2].mem))
      {
		  newworld();
			if(debugger)Serial.println(F(" SAVE!!! ")); Serial.println(prog[ch2].mem); 
		specifprog = true;
		whatprog = ch2;
    robotprogram = SD.open(prog[whatprog].mem, FILE_WRITE);
		for (uint8_t j=0;j<=checkint;++j)  saveProgtoSD (j);  // SAVE POINTS ONE BY ONE TO SD
		exit = true;
      }
		}
			delay(250);
		}
		exit = false;
	  }
	///////////////////////////////////////////////////////////////////////////
	else if (ch==2) { // search and show programs
 checkForPrograms();
    int ch2 =0;
    
		while(!exit)
		{           if(debugger)Serial.println(F(" SELECT PROGRAM ")); 
                if(debugger)Serial.println(ch2+1); 
		  delay(1000);
			lcd.setCursor(0, 0);
			lcd.print("<<<  SELECT UP >>>>");
			lcd.setCursor(0, 1);
			lcd.print(show[ch2]);			
			lcd.setCursor(0, 2);
			lcd.print("<<<<<<<<< >>>>>>>>>");    
			lcd.setCursor(0, 3);
			lcd.print("next         select"); 
		if 		  (!digitalRead(TASTER01)) {newworld();ch2++; if(ch2>=prgcot)ch2=0; Serial.println(F(" Show::")); Serial.println(show[ch2]);}
		else if (!digitalRead(TASTER02)) {newworld();if(debugger)Serial.println(F(" SELECT!!! "));Serial.println(show[ch2]); specifprog= true; whatprog=ch2; readProgfromSD(0); exit = true;}
			delay(250);
		}
		exit = false;
	} 
	///////////////////////////////////////////////////////////////////////////
	else if (ch==3) { // delete specific program
    checkForPrograms();
    int ch2 =0;
    
    while(!exit)
    {           if(debugger)Serial.println(F(" DELETE PROGRAM ")); 
                if(debugger)Serial.println(ch2+1); 
      delay(1000);
      lcd.setCursor(0, 0);
      lcd.print("<<<  DELETE UP >>>>");
      lcd.setCursor(0, 1);
      lcd.print(show[ch2]);     
      lcd.setCursor(0, 2);
      lcd.print("<<<<<<<<< >>>>>>>>>");    
      lcd.setCursor(0, 3);
      lcd.print("next         select"); 
    if      (!digitalRead(TASTER01)) {newworld();ch2++; if(ch2>prgcot)ch2=0; Serial.println(F(" Show::")); Serial.println(show[ch2]);}
    else if (!digitalRead(TASTER02)) 
    {
      newworld();
      if(debugger){Serial.println(F(" DELETE!!! ")); Serial.println(show[ch2]);} 
      SD.remove(show[ch2]); 
      exit = true;
       lcd.setCursor(0, 0);
      lcd.print("<<<            >>>>");
      lcd.setCursor(0, 1);
      lcd.print("      DELETED      ");     
      lcd.setCursor(0, 2);
      lcd.print("<<<<<<<<< >>>>>>>>>");     
      }
      delay(500);
    }
    exit = false;
	}
	///////////////////////////////////////////////////////////////////////////
	else if (ch==4) { // delete single point
	int ch3=0;
		if(debugger)Serial.println(F(" DELETE SINGLE POINT ")); 
      delay(1000);
      lcd.setCursor(0, 0);
      lcd.print("<<DELETE Si-Point>>");
      lcd.setCursor(4, 1);
      lcd.print(ch3);     
      lcd.setCursor(0, 2);
      lcd.print("<<<<<<<<< >>>>>>>>>");    
      lcd.setCursor(0, 3);
      lcd.print("next         select"); 
    if (!digitalRead(TASTER01)) {newworld();ch3++; if(ch3>checkint)ch3=0; Serial.println(F(" Show::"));}
	
		if  (!digitalRead(TASTER02)) 
		{	
			int pos(ch3);
			newworld();
			for (int i=0; i<checkint; ++i) // COPY MEMORY SLOTS
			{
				prs[pos] = prs[pos+1];
				pos++;
			}
			String text = prog[whatprog].mem;
			
			if (!text == "robottempfile.txt") robotprogram = SD.open(text, FILE_WRITE); // WENN DAS TEMP PROGRAMM GEÖFFNET WURDE
			for (uint8_t j=0;j<=checkint;++j)  saveProgtoSD (j);  // SAVE POINTS ONE BY ONE TO SD
		}
	}
	///////////////////////////////////////////////////////////////////////////
	else if (ch==5) {}
	///////////////////////////////////////////////////////////////////////////
	else if (ch==6) {exit = true; return;}
}
///////////////////////////////////////////////////////////////////////////
//////////////////////PROGRAMMFUNKTIONEN///////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void driveRobot() 							//DRIVE ROBOT FROM LOADED PROGRAM
{
	if (checkint == 0) checkint = (prgpoi-1); //Fahre auch nach dem Teachen
	
	if(checkint!=0) // Nur wenn auch geteacht wurde
	{
		for (int i=0; i<=checkint;i++)
		{	
			if(dtp>0)i=dtp; 	// WENN die Letzte Positon angefahren werden soll NEXT POSITON ABFRAGE IM TEACHMODUS
		    else if(dtp==(-1))i=0; // WENN die erste Programm Position angefahren werden soll

			printSerialValues(i); // SHOW DRIVING PARAMETERS
			translateMotionValue(i); // Translate in BOOL VALUES
			translateGripperValue(i);
			
      if  (prs[i].led1==true && checkLastled==false)      {beleuchtungsledon ();checkLastled=true; }   //SWITCH LED ON
	  else if  (prs[i].led1==false && checkLastled==true)  {beleuchtungsledoff();checkLastled=false;}  //SWITCH LED OFF digitalWrite(MAGNETGREIFER, HIGH); 
  
	  if  (prs[i].gripper==true && checkLastMagnet==false) prs[i].speedmot = 15; //CHANGE SPEED MOVEMENT IF MAGNETIC GRIPPER IS POWER ON 
		
		showRunningPath(i,prs[i].a1,prs[i].a2,prs[i].a3,prs[i].a4,prs[i].speedmot); //LCD ANZEIGE
		
		if (dtp==0)  
		{
			driveLinearAxis(prs[i].a4);
		notausabfrage ();
			achse1.write(prs[i].a1, prs[i].speedmot, prs[i].motion);
        notausabfrage ();
			achse2.write(prs[i].a2, prs[i].speedmot, prs[i].motion);               
        notausabfrage ();  
			achse3.write(prs[i].a3, prs[i].speedmot, prs[i].motion);
		delay(100);

		  if  (prs[i].gripper==true && checkLastMagnet==false)        {prozessledon(); delay(500);  for(int m=0;m<99;m++)digitalWrite(MAGNETGREIFER, HIGH); delay(1000); checkLastMagnet=true;} //SWITCH GRIPPER ON
		  else if  (prs[i].gripper==false && checkLastMagnet==true)   {prozessledoff(); delay(500); for(int m=0;m<99;m++)digitalWrite(MAGNETGREIFER, LOW); delay(1000); checkLastMagnet=false;}//SWITCH GRIPPER OFF
  		}
		
		else if (dtp>0||dtp==(-1))	// BEWEGE DICH BEIM RÜCKWÄRTSFAHREN AUCH RÜCKWÄRTS
		{ 

		  if  (prs[i].gripper==true && checkLastMagnet==false)        {prozessledon(); delay(500); for(int m=0;m<99;m++)digitalWrite(MAGNETGREIFER, HIGH);delay(1000); checkLastMagnet=true;} //SWITCH GRIPPER ON
		  else if  (prs[i].gripper==false && checkLastMagnet==true)   {prozessledoff();delay(500); for(int m=0;m<99;m++)digitalWrite(MAGNETGREIFER, LOW); delay(1000); checkLastMagnet=false;}//SWITCH GRIPPER OFF
  		
		notausabfrage ();  
			achse3.write(prs[i].a3, prs[i].speedmot, prs[i].motion);
        notausabfrage ();
			achse2.write(prs[i].a2, prs[i].speedmot, prs[i].motion);               
        notausabfrage (); 
			achse1.write(prs[i].a1, prs[i].speedmot, prs[i].motion);		
		notausabfrage ();		
			driveLinearAxis(prs[i].a4);
			dtp = 0; // SETZE RÜCKWÄRTSFAHRT ZURÜCK
		}
		
			if (digitalRead(AUTOMANUEL) == HIGH)  // GO TO TEACHMODUS
			{   //STARTWERTE prjpoi,int a,int b, int c, bool motion, int speed, bool gripper, bool led1, bool led2
				teachInPosition = true; //GLOBAL VARIABLE FOR STATUS
				actualPos = i; //GLOBAL VARIABLE FOR TEACHMOD
				teachmodus(prs[i].a1,prs[i].a2,prs[i].a3,prs[i].a4,prs[i].motion,prs[i].speedmot,prs[i].gripper,prs[i].led1,prs[i].led2);
				teachInPosition = false; //GLOBAL VARIABLE FOR STATUS
			}
			Serial.println("##############-ROBOT-REACHED-THE-POSITION-##############\n\n\n");
		}
	}
}
///////////////////////////////////////////////////////////////////////////
uint16_t driveLinearAxis(uint16_t sollwert) //DRIVE LINEARAXIS 
{ 
if (sollwert<=linearlaenge+15) // SICHERHEIT
 {
	Serial.println(F("##### Istwert Linearachse: #####"));
    Serial.println(realLinearAxis);
	Serial.println(F("##### Neuer Sollwert:      #####"));    
    Serial.println(sollwert);
	
    if (realLinearAxis<sollwert)
    {
    Serial.println(F("##### Verfahre nach Rechts #####"));        
      digitalWrite(ENABLE,   LOW);                       //Aktiviere die Motorsteuerung
      digitalWrite(DIRECTION,HIGH);                       //ROBOTER BEWEGT SICH NACH RECHTS 
      while (realLinearAxis<sollwert)
      {   
        if (!digitalRead(ENDSCHALTER_RECHTS)) break;
			realLinearAxis++;
            digitalWrite(PULS,HIGH);
            delayMicroseconds(LINEAR_ACHSE_SPEED);
            digitalWrite(PULS,LOW);
            delayMicroseconds(LINEAR_ACHSE_SPEED);
            notausabfrage ();
      }
          digitalWrite(DIRECTION,LOW);//Deaktiviere die 
    Serial.println(F("##### Istwert: #####"));
    Serial.println(realLinearAxis);   
  }
      
      
  else if (realLinearAxis>sollwert)
  {         Serial.println(F("##### Verfahre nach Links #####")); 
            digitalWrite(ENABLE,   LOW);                       //Aktiviere die Motorsteuerung
            digitalWrite(DIRECTION,LOW);                       //ROBOTER BEWEGT SICH NACH LINKS
      while (realLinearAxis>sollwert)
      {   
        if (!digitalRead(ENDSCHALTER_LINKS)) break;
			realLinearAxis--; 
            digitalWrite(PULS,HIGH);
            delayMicroseconds(LINEAR_ACHSE_SPEED);
            digitalWrite(PULS,LOW);
            delayMicroseconds(LINEAR_ACHSE_SPEED);
            notausabfrage ();
      }
          digitalWrite(DIRECTION,LOW);//Deaktiviere die Motorsteuerung
    Serial.println(F("###### Istwert: ######"));
    Serial.println(realLinearAxis); 
  }
 }
return realLinearAxis;
}
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void writePoiToMemory(uint8_t whatpos, uint8_t a1,uint8_t a2, uint8_t a3, uint16_t a4,bool motion,uint8_t speedmot, bool gripper, bool led1,bool led2) 	//Schreibe neuen Punkt in Speicher
{
	prs[whatpos].a1 = a1;
	prs[whatpos].a2 = a2;
	prs[whatpos].a3 = a3;
	prs[whatpos].a4 = a4;
	prs[whatpos].motion = motion;
	prs[whatpos].speedmot = speedmot;
	prs[whatpos].gripper = gripper; 
	prs[whatpos].led1 = led1;   
	prs[whatpos].led2 = led2;  
}
///////////////////////////////////////////////////////////////////////////
void showJumpOptionsLcd()				//LCD ANZEIGE
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("<<<<<<<<< >>>>>>>>>");
    lcd.setCursor(0, 1);
    lcd.print("<<<<<Drive to?>>>>>");
    lcd.setCursor(0, 2);
    lcd.print("<<<<<<<<< >>>>>>>>>");  
    lcd.setCursor(0, 3);
    lcd.print("Back<<<<< >>>>>Next"); 
}
///////////////////////////////////////////////////////////////////////////
void pointTeachedlcd()                	//LCD ANZEIGE
{
    lcd.clear();
if (!saveModusChoose)
  {
    lcd.setCursor(0, 0);
    lcd.print("<<<<<<<<< >>>>>>>>>");   
    lcd.setCursor(0, 1);
    lcd.print("<<<<<<<Punkt>>>>>>>");     
    lcd.setCursor(0, 2);
    lcd.print("<<<<gespeichert!>>>");  
    lcd.setCursor(0, 3);
    lcd.print("<<<<<<<<< >>>>>>>>>");
  }
  
if (saveModusChoose)
  {
    lcd.setCursor(0, 0);
    lcd.print("<<<<<<<<< >>>>>>>>>");  
    lcd.setCursor(0, 1);
    lcd.print("<<<<<<<Punkt>>>>>>>");     
    lcd.setCursor(0, 2);
    lcd.print("<<<<<erstellt!>>>>>");  
    lcd.setCursor(0, 3);
    lcd.print("<<<<<<<<< >>>>>>>>>");
  }
    delay(200);
    saveModusChoose = false;
}
///////////////////////////////////////////////////////////////////////////
void showRunningPath(uint8_t whatpos, uint8_t a1,uint8_t a2, uint8_t a3, uint16_t a4,uint8_t speedmot)		//TRANSLATION
{
	
	lcd.clear();
	
	lcd.setCursor(0,0);
	lcd.print("Position ");
	lcd.setCursor(9,0);
	lcd.print(whatpos+1);	
	
    lcd.setCursor(0, 1);
    lcd.print("A1=");
    lcd.setCursor(3,1);
    lcd.print(a1) ;
     
    lcd.setCursor(8, 1);
    lcd.print("A2=");
    lcd.setCursor(11,1);
    lcd.print(a2) ;
    
    lcd.setCursor(15, 1);
    lcd.print("A3=");
    lcd.setCursor(18,1);
    lcd.print(a3) ;     
	
	lcd.setCursor(0,2);
	lcd.print("A4=");
	lcd.setCursor(3,2);
	lcd.print(a4);

	lcd.setCursor(0,3);
	lcd.print("Speed=");
	lcd.setCursor(7,3);
	lcd.print(speedmot);		
}
///////////////////////////////////////////////////////////////////////////
void robotteachsetuplcd(uint8_t prgpoi, uint8_t a1,uint8_t a2, uint8_t a3, uint16_t a4,bool motion,uint8_t speedmot, bool gripper, bool led1,bool led2)  //TEACHMENUE
{

    lcd.clear();
      
    lcd.setCursor(0, 0);
    lcd.print("A1=");
    lcd.setCursor(3,0);
    lcd.print(a1) ;
    
    lcd.setCursor(8, 0);
    lcd.print("A2=");
    lcd.setCursor(11,0);
    lcd.print(a2) ;
    
    lcd.setCursor(15, 0);
    lcd.print("A3=");
    lcd.setCursor(18,0);
    lcd.print(a3) ;  
 

	if (a4<0||a4>28000) // PRÜEFE AXE 4
		{
			lcd.setCursor(0,1);
			lcd.print("A4="); 
			lcd.setCursor(3,1);
			lcd.print("Fehler!");
		}
	else 
		{
			lcd.setCursor(0,1);
			lcd.print("A4=");
			lcd.setCursor(3,1);
			lcd.print(a4);
		}
  
    lcd.setCursor(9,1);
    lcd.print("Gr=");
    lcd.setCursor(12,1);
    lcd.print(gripper);

    lcd.setCursor(15,1);
    lcd.print("Le=");
    lcd.setCursor(18,1);
    lcd.print(led1);
}
///////////////////////////////////////////////////////////////////////////
void printSerialValues(uint8_t i)			//TRANSLATION
{ 
if(debugger)
{
    Serial.println("##############-ROBOT-MOVE-TO-POSITION-###################");
	Serial.println(" Punkt = ");     
    Serial.println(i);	 
	Serial.println("###############");
	Serial.println(" Drive Achse 1 ");     
    Serial.println(prs[i].a1);
    Serial.println(" 	Drive Achse 2 ");             
    Serial.println(prs[i].a2);
    Serial.println(" 		Drive Achse 3 ");       
    Serial.println(prs[i].a3);
    Serial.println(" 			Drive Achse 4 ");       
    Serial.println(prs[i].a4);
    Serial.println(" 				Drive Motion ");  	
    Serial.println(prs[i].motion);
    Serial.println(" 					Speed     ");       
    Serial.println(prs[i].speedmot);
    Serial.println(" 						Greifer "); 
    Serial.println(prs[i].gripper);
    Serial.println(" 							Led 1");      
    Serial.println(prs[i].led1);
}
} 
///////////////////////////////////////////////////////////////////////////
void translateMotionValue(uint8_t i)		//TRANSLATION
{
	if (prs[i].motion == 0) prs[i].motion = false;
	else if (prs[i].motion == 1) prs[i].motion = true;
}
///////////////////////////////////////////////////////////////////////////
void translateGripperValue(uint8_t i)		//TRANSLATION
{
	if (prs[i].gripper == 0) prs[i].motion = false;
	else if (prs[i].gripper == 1) prs[i].motion = true;
}
///////////////////////////////////////////////////////////////////////////
void createWhatPosChars(uint8_t whatpos) 	// ERSTELLE AUS INT EIN CHAR ARRAY
{
		char copy [3];
	
	itoa(whatpos, axisposchars, 10);
	
	for (int i=0;i<3;i++)
	{
	copy[i] = axisposchars[i];
	}
	
	if (whatpos<10)   // BEFORE 5X \0 	AFTER X5 \0
	{
	axisposchars[0] = '0';
	axisposchars[1] = copy [0];
	}
	
	axisposchars[2] = '\0';
}
///////////////////////////////////////////////////////////////////////////
void createAxis4Chars(uint16_t a4) 			// ERSTELLE AUS INT EIN CHAR ARRAY
{
		char copy [6];
		
	itoa(a4, axis4chars, 10);
	
	for (int i=0;i<6;i++)
	{
	copy[i] = axis4chars[i];
	}
	
	if (a4<10)   // BEFORE 5XXXX \0 	AFTER XXXX5 \0
	{
	axis4chars[0] = '0';
	axis4chars[1] = '0';
	axis4chars[2] = '0';
	axis4chars[3] = '0';
	axis4chars[4] = copy [0];
	}		
	else if (a4<100)   // BEFORE 53XXX \0 	AFTER XXX53 \0
	{
	axis4chars[0] = '0';
	axis4chars[1] = '0';
	axis4chars[2] = '0';
	axis4chars[3] = copy [0];
	axis4chars[4] = copy [1];
	}		
	else if (a4<1000)   // BEFORE 537XX \0 	AFTER XX537 \0
	{
	axis4chars[0] = '0';
	axis4chars[1] = '0';
	axis4chars[2] = copy [0];
	axis4chars[3] = copy [1];
	axis4chars[4] = copy [2];
	}	
	else if (a4<10000)   // BEFORE 5374X \0 	AFTER X5374 \0
	{
	axis4chars[0] = '0';
	axis4chars[1] = copy [0];
	axis4chars[2] = copy [1];
	axis4chars[3] = copy [2];
	axis4chars[4] = copy [3];
	}
  else if (a4<=10000)   // BEFORE 5374X \0   AFTER X5374 \0
  {
  axis4chars[0] = copy [0];
  axis4chars[1] = copy [1];
  axis4chars[2] = copy [2];
  axis4chars[3] = copy [3];
  axis4chars[4] = copy [4];
  }	
	axis4chars[5] = '\0';	
}
///////////////////////////////////////////////////////////////////////////
void createAxis3Chars(uint8_t a3) 			// ERSTELLE AUS INT EIN CHAR ARRAY
{
	char copy [3];
	
	itoa(a3, axis3chars, 10);
	
	for (int i=0;i<3;i++)
	{
	copy[i] = axis3chars[i];
	}
	
	if (a3<10)   // BEFORE 5X \0 	AFTER X5 \0
	{
    axis3chars[0] = '0';
  	axis3chars[1] = copy [0];
	}
	
	axis3chars[2] = '\0';	
}
///////////////////////////////////////////////////////////////////////////
void createAxis2Chars(uint8_t a2) 			// ERSTELLE AUS INT EIN CHAR ARRAY
{
	char copy [3];
	
	itoa(a2, axis2chars, 10);
	
	for (int i=0;i<3;i++)
	{
	copy[i] = axis2chars[i];
	}
	
	if (a2<10)   // BEFORE 5X \0 	AFTER X5 \0
	{
  axis2chars[0] = '0';
	axis2chars[1] = copy [0];
	}
	
	axis2chars[2] = '\0';
}
///////////////////////////////////////////////////////////////////////////
void createAxis1Chars(uint8_t a1) 			// ERSTELLE AUS INTEGRATION EIN CHAR ARRAY
{
	char copy [3];
	
	itoa(a1, axis1chars, 10);
	
	for (int i=0;i<3;i++)
	{
	copy[i] = axis1chars[i];
	}
	
	if (a1<10)   // BEFORE 5X \0 	AFTER X5 \0
	{
  axis1chars[0] = '0';
	axis1chars[1] = copy [0];
	}
	
	axis1chars[2] = '\0';
}
///////////////////////////////////////////////////////////////////////////
void greifersuchlauf (uint8_t a3ist, uint8_t a2ist)    //Suchlauf fur Kugel
{ 	
 uint8_t suchlauf;

//LOCATE    
	suchlauf = a3ist; 
    while  (digitalRead(MAGNETSCHALTER_ROBOTER) == HIGH) //Erst wenn INI LOW ist wird Magnetgreifer geschlossen
      {
        if (suchlauf < 28) suchlauf = a3ist-20; 
		    suchlauf--;
        achse3.write(suchlauf,15,true);
        delay (500);
        Serial.println (F(" A3 und Ini-Greifer: "));
        Serial.println (digitalRead(MAGNETSCHALTER_ROBOTER));
     for (int settime=0; settime<200; settime++)
        {        
          if (digitalRead(MAGNETSCHALTER_ROBOTER) == LOW) 
          {
            a3ist = suchlauf;
            break; 
          }
        }
      }
    breakout:
/*
    // FINE TUNING
    uint8_t i=0;
  while  (digitalRead(MAGNETSCHALTER_ROBOTER) == LOW) 
      { 
       i++;
       achse2.write((a2ist++),10,true);
       delay (300); 
       Serial.println (F(" A2 und Ini-Greifer: "));
       Serial.println (digitalRead(MAGNETSCHALTER_ROBOTER));       
       if (digitalRead(MAGNETSCHALTER_ROBOTER) == HIGH) 
           {break;}
      }   
    i=i/2;   
    achse2.write((a2ist-i),5,true);
    
    i=0;
  while  (digitalRead(MAGNETSCHALTER_ROBOTER) == LOW) 
      {
       i++; 
       achse3.write((a3ist++),15,true);
       delay (300);
       Serial.println (F(" A3 und Ini-Greifer: "));
       Serial.println (digitalRead(MAGNETSCHALTER_ROBOTER));  
       if (digitalRead(MAGNETSCHALTER_ROBOTER) == HIGH) 
            {break;}       
      }
    breakout3:   
    i=i/2;   
    achse3.write((a3ist-i),5,true);    
*/
    
    axis3save = suchlauf;
  }
///////////////////////////////////////////////////////////////////////////
void magneticGripperOn()           			//MAGNETGREIFER SCHLIESSEN
{
	while (digitalRead(MAGNETSCHALTER_ROBOTER) == HIGH)
	{
     for (uint8_t i=0;i<200;i++)
     {digitalWrite(MAGNETGREIFER, HIGH); 	 Serial.println(F("  ++++  "));}                 //Magnetgreifer Einschalten   
    // kugelgefundenlcd ();
	 Serial.println(F(" Magnetgreifer wird EINGESCHALTET!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
	 Serial.println(digitalRead(MAGNETSCHALTER_ROBOTER));
	}
}
///////////////////////////////////////////////////////////////////////////
void magneticGripperOff()          			//MAGNETGREIFER OEFFNEN
{
	while (digitalRead(MAGNETSCHALTER_ROBOTER) == LOW) // SOLANGE KUGEL IM GREIFER
	{
		for  (uint8_t i=0; i<200;i++) 
		{digitalWrite(MAGNETGREIFER, LOW); Serial.println(F("  ----  "));}            //Magnetgreifer Ausschalten	
	}
	//if (digitalRead(MAGNETSCHALTER_ROBOTER) == HIGH) kugelablegenlcd();  
  
	 Serial.println(F(" Magnetgreifer wird AUSGESCHALTET!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
	 Serial.println(digitalRead(MAGNETSCHALTER_ROBOTER));
}
///////////////////////////////////////////////////////////////////////////
void saveAxisFriend() 						//CHECK FOR BAD AXIS PARAMETER
{
  if (realLinearAxis < 0 || !digitalRead(ENDSCHALTER_LINKS) )
  {
      realLinearAxis = 0;
  }     
}
///////////////////////////////////////////////////////////////////////////
void testsdkarte ()							//Testprogamm für SD Karte
{               
       robotprogram = SD.open("robottempfile.txt", FILE_WRITE); 
  if (robotprogram) 
{                                                                                 
    robotprogram.close();                                
    Serial.println(F("Temoräres Roboterporgamm vorhanden!"));                    
} 
  else 
{                                                      
    Serial.println(F("Roboterprogramm konnte nicht geoeffnet werden!"));  
}
  
  robotprogram = SD.open("robottempfile.txt");                           
  if (robotprogram) 
{                                            
    Serial.println(F("Versuche zu lesen!"));                            

   
  robotprogram.close();                                    
} 

  else 
{                                                      
    Serial.println(F("Roboterprogramm konnte nicht gelesen werden!"));  
}
    
}
///////////////////////////////////////////////////////////////////////////
void initzalisierung ()             		//Initzalisierung
{
    newworld ();                      //NewWorld SOUND
    beleuchtungsledon ();             //Programmbeleuchtung starten

//////////////AUFRUFE//REFERENZIERUNG//LINEARACHSE/////////////////////////
    
    fahreachseninitzialisierung();    //Fahre Roboter zum Endschalter
    delay(100);
    fahreachsenlaengenberechnung();   //Messe die Länge der Achse
    delay(100);

if (digitalRead(TASTER01))
{
/////////////////////KALIBRIERUNG ROBOTER////////////////////
    achse3.write(60, robotspeed); 
    achse1.write(4, robotspeed,true);
    achse2.write(68, 10);         
    achse3.write(40, robotspeed,true);     
   
    soundalive ();                         //HALLO
    prozessledon ();               //PROZESSLED AN
	tcpreferenzlcd ();        //Kontrolle der TCP Spitze      
    
	greifersuchlauf(40,70); // SEARCH THE ARTEFAKT

 	  achse3.write(axis3save,10,true);
    magneticGripperOn();             //MAGNETGREIFER SCHLIESSEN          
    magneticGripperOff();             //MAGNETGREIFER Öffnen    
 
	prozessledoff ();               //PROZESSLED AN 
    achse2.write(90, 10,true);  
    achse3.write(90, 20);    
    achse1.write(90, 20);       //START
}
    roboterinitfertiglcd ();    //Fertigmeldung
  
	fahreachsenachlinks_half ();	
	fahreachsenachlinks_half ();     		
    
    prozessledoff ();       //Prozessled aus

    tasterwahllcdloop       = 0;  //ruecksetzen
    TASTER02WERT            = HIGH;
    delay (1000);

}
///////////////////////////////////////////////////////////////////////////
void startstellung ()               		//Startstellung Roboter
{
       achse1.write(90, 10, true);        //HOME
       achse2.write(90, 10, true);      //HOME
       achse3.write(60, 10, true);      //HOME
}
///////////////////////////////////////////////////////////////////////////
void endstellung ()             			//Endstellung Roboter power off
{
       achse1.write(90, 10, true);    	  //HOME
       achse2.write(90, 10, true);        //HOME
       achse3.write(90, 10, true);        //HOME
}
///////////////////////////////////////////////////////////////////////////
void roboterprogramm3 ()           			//Ruft das Roboterprogramm auf
{
 ///////////////////////////////////////////////////////////////////////////
//////////////////ROBOTER _BEWEGUNGS_PROGRAMM//////////////////////////////
///////////////////////////////////////////////////////////////////////////
        notausabfrage ();   
		    kugelsuchelcd ();
	     	newworld ();               			  //HALLO SOUND
		    ledtest ();                           //LED Lichttest
        beleuchtungsledon ();      		      //Programmbeleuchtung starten        
/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////NEHME DAS BAUTEIL//////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
       achse1.write(24, robotspeed);          //DREHE A1 nach RECHTS
       notausabfrage ();   
       fahreachsenachrechts_half ();          //LINEAREINHEIT FÄHRT NACH RECHTS
       notausabfrage ();          
       
       achse3.write(22, 20);                //KIPPE A3 nach UNTEN
       
       notausabfrage ();          
       achse2.write(62, 15, true);      //KIPPE A2 nach UNTEN
       notausabfrage ();       
      
////////////PROZESSBEFEHL////////////////////////////////////////////////////////////
       soundalive ();                         //HALLO
       prozessledon ();               //PROZESSLED AN
       magneticGripperOn();             //MAGNETGREIFER SCHLIESSEN

       achse3.write(0, 20,true);                //KIPPE A3 nach UNTEN            //SUCHLAUF FUER KUGEL
            delay(300);
       kugelgefundenlcd ();             //LCD 
/////////////////////////////////////////////////////////////////////////////////////
       notausabfrage (); 
        achse3.write(40, robotspeed);           //HEBE A3 nach OBEN
         achse2.write(90, robotspeed,true);            //KIPPE A2 nach UNTEN

             
         achse2.write(105, robotspeed,true);            //KIPPE A2 nach UNTEN
       notausabfrage ();   
         achse3.write(60, robotspeed,true);           //HEBE A3 nach OBEN
       notausabfrage ();      
       achse1.write(90, robotspeed,true);          //DREHE A1 nach LINKS
       notausabfrage ();   
       notausabfrage ();       
/////////////////////////////////////////////////////////////////////////       
       achse2.write(90, 15);           //HEBE A2 nach OBEN
/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////FAHRE IN GRUNDSTELLUNG/////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////                       
 fahreachsenachlinks_half ();             //LINEAREINHEIT FÄHRT NACH LINKS

  fahreachsenachlinks_half ();         //LINEAREINHEIT FÄHRT NACH LINKS
/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////LEGE DAS BAUTEIL AB////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
      

       achse1.write(94, robotspeed);          //DREHE A1 nach LINKS      
            notausabfrage ();     
       achse2.write(50, 15,true);           //KIPPE A2 nach UNTEN
       notausabfrage ();   
       achse3.write(22,  10,true);        //KIPPE A3 nach UNTEN

     notausabfrage (); 
   
       achse3.write(29,  10);        //KIPPE A3 nach UNTEN    
            notausabfrage (); 



            //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      
       achse2.write(50, 10,true);           //KIPPE A2 nach UNTEN 
     
     
  //  if  (digitalRead(AUTOMANUEL)==HIGH) teachmodus(94,60,10);//Aktueller wert
  
         delay(2000);


   
         
          notausabfrage ();   
       achse3.write(53,10,true);        //KIPPE A3 nach oben
     
     
     notausabfrage (); 
    
    newtonpendel();   //Aufpruf netwonpednel

    
       achse3.write(29,  10,true);        //KIPPE A3 nach UNTEN        

         delay(1000);

   // if  (digitalRead(AUTOMANUEL)==HIGH) teachmodus(94,50,29);//Aktueller wert
  
////////////PROZESSBEFEHL////////////////////////////////////////////////////////////
       magneticGripperOff();                  //MAGNETGREIFER OEFFEN
       prozessledoff();               //PROZESSLED AUS
       sounddown();
/////////////////////////////////////////////////////////////////////////////////////       

         delay(2000);

   // if  (digitalRead(AUTOMANUEL)==HIGH) teachmodus(94,60,29);//Aktueller wert
  
        achse2.write(60, 20,true);           //KIPPE A2 nach UNTEN 
    
       delay(1000);
           kugelfertiglcd ();
           notausabfrage (); 
///////////////////////////////////////////////////////////////////////////////////// 
       achse2.write(90, 20,true);           //KIPPE A2 nach UNTEN
       notausabfrage (); 
       achse1.write(90, robotspeed);            
       notausabfrage ();   
       achse3.write(90, robotspeed);     //KIPPE A2 nach UNTEN
       fahreachsenachrechts_half ();         //LINEAREINHEIT FÄHRT NACH RECHTS
      fahreachsenachrechts_half ();         //LINEAREINHEIT FÄHRT NACH RECHTS

 achse1.write(14, 20,true);
 achse2.write(58, 20,true);
 achse3.write(45, 20,true);




 achse1.write(52, 20,true);
 achse3.write(42, 5,true);
 achse2.write(29, 10,true);
 achse3.write(40, 5,true);



  //  if  (digitalRead(AUTOMANUEL)==HIGH) teachmodus(52,29,40);//Aktueller wert
  
         delay(2000);


////////////PROZESSBEFEHL////////////////////////////////////////////////////////////
       soundalive ();                         //HALLO
       prozessledon ();               //PROZESSLED AN
       magneticGripperOn();             //MAGNETGREIFER SCHLIESSEN
            delay(300);
       kugelgefundenlcd ();             //LCD 
/////////////////////////////////////////////////////////////////////////////////////
 achse2.write(58, 10,true);
 
 
 achse1.write(14, 20,true);
 achse2.write(90, 20,true);
 achse3.write(90, 20,true);
 
 
       achse1.write(90, robotspeed);          //DREHE A1 nach LINKS   
       notausabfrage ();       
       kugelinsnestlcd ();
       notausabfrage (); 
       achse3.write(84, 15);           //HEBE A3 nach OBEN
       notausabfrage ();          
       achse2.write(72, 15,true);           //HEBE A3 nach OBEN    
/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////LEGE DAS BAUTEIL AB////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
      delay(300);
       notausabfrage ();       
       kugelablegenlcd ();  
////////////PROZESSBEFEHL////////////////////////////////////////////////////////////
       magneticGripperOff();                  //MAGNETGREIFER OEFFEN
       prozessledoff();               //PROZESSLED AUS
       sounddown();
/////////////////////////////////////////////////////////////////////////////////////       
    kugelfertiglcd ();
       achse2.write(90, 15,true);           //HEBE A2 nach OBEN
/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////FAHRE IN GRUNDSTELLUNG/////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////                       
       fahreachsenachlinks_half ();             //LINEAREINHEIT FÄHRT NACH LINKS
 
  }
///////////////////////////////////////////////////////////////////////////
void optionsmenue ()            			//Optionsmenue
{  
  if (optionenlcdweiter == 0)
  {
    optionenlcd ();
    Serial.println(F("Schreibe auf Display")); 
    optionenlcdweiter = 1;
    delay (1000); 
  }
  
  while (optionenlcdweiter == 1)
  { 
    delay (500);
    Serial.println(F("Warte auf Tasterauswahl"));  
    TASTER01WERT = digitalRead(TASTER01);
    if (TASTER01WERT == LOW)
    {
    optionenlcdweiter = 3;
    Serial.println(F("Taste verlasse Optionsmenue"));  
    } 
    TASTER02WERT = digitalRead(TASTER02);
    if (TASTER02WERT == LOW)
    {
    optionenlcdweiter = 2;  
    Serial.println(F("Taste für Robotspeedsetup"));
    }   
    
  }
  
  if (optionenlcdweiter == 2)        //AUSWAHLMENUE STATUSUEBERSICHT
  { 
    robotspeedsetuplcd ();
    Serial.println(F("Robotspeedsetup LCD"));
  }
  
  while (robotspeedsetuplcdweiter == 0 && optionenlcdweiter == 2) //AUSWAHLMENUE ROBOTSPEEDSETUP
  { 
    delay (500);  
    Serial.println(F("Warte auf Tasterauswahl"));
        
    TASTER01WERT = digitalRead(TASTER01);
    if (TASTER01WERT == LOW)
    {
    optionenlcdweiter = 4;  
    robotspeedsetuplcdweiter = 2;
    Serial.println(F("Taste für Weiter mit Endstops"));  
    } 
    
    TASTER02WERT = digitalRead(TASTER02);
    if (TASTER02WERT == LOW)
    {
    robotspeedsetuplcdweiter = 1;
    Serial.println(F("Taste für Robotspeedsetup hochsetzen"));
    } 
    
    if  (robotspeedsetuplcdweiter == 1)   
    {
      Serial.println(F("Robotspeedsetup ="));
      Serial.println(robotspeed);
      Serial.println(F("################")); 
      
      Serial.println(F("Robotspeedsetup 10 hochgezaehlt"));      
      robotspeed = robotspeed + 10;
      newworld ();
    if (robotspeed > 30)
      {
        robotspeed = 10;
        Serial.println(F("Robotspeedsetup auf 10 gesetzt"));
      } 
      
      lcd.setCursor(11, 0);
      lcd.print(robotspeed);
      robotspeedsetuplcdweiter =0;
      delay (1000);
     }
          
  }   
  
    if (robotspeedsetuplcdweiter == 2 && endstopsetuplcdweiter == 0)  
    {
    endstopsetuplcd ();
    endstopsetuplcdweiter++;
    }
    
  while (endstopsetuplcdweiter == 1)        //AUSWAHLMENUE ENDSCHALTER
  {
    delay (500);  
    Serial.println(F("Warte auf Tasterauswahl"));    
    TASTER01WERT = digitalRead(TASTER01);
    if (TASTER01WERT == LOW)
    {
    endstopsetuplcdweiter = 4;  
    Serial.println(F("Taste für Animationen"));  
    } 
    
    TASTER02WERT = digitalRead(TASTER02);
    if (TASTER02WERT == LOW)
    {
    endstopsetuplcdweiter = 2;
    Serial.println(F("Taste für Teststart"));
    } 
        
if (endstopsetuplcdweiter == 4)
  { 
    animationensetuplcd ();
    delay (500);
    while (endstopsetuplcdweiter == 4)                    //AUSWAHLMENUE ANIMATIONEN
    { 
      TASTER01WERT = digitalRead(TASTER01);
      if (TASTER01WERT == LOW)
      {
        endstopsetuplcdweiter = 5;
        Serial.println(F("Taste für HOME"));
        delay (500);  
      } 
    
      TASTER02WERT = digitalRead(TASTER02);
      if (TASTER02WERT == LOW)
      {
        Serial.println(F("Taste Animationen ausschalten"));
        delay (50); 
        animationensetup = 0; //Animationen werden ausgeschaltet
        animationensetuplcd ();
      } 
    }
  }   
}

  if (endstopsetuplcdweiter == 2)     //ENDSCHALTERTEST
    {    
    while (endschalterlinkstest == 0)
    {
    delay (500);  
    endstoplinkslcd ();
    ENDSCHALTER_LINKSWERT = digitalRead(ENDSCHALTER_LINKS);
    
    if (ENDSCHALTER_LINKSWERT == LOW)
      {
      endschalterlinkstest++;
      newworld ();
      }
    }
 
    while (endschalterrechtstest == 0)
    {
    delay (500);  
    endstoprechtslcd ();
    ENDSCHALTER_RECHTSWERT = digitalRead(ENDSCHALTER_RECHTS);
    if (ENDSCHALTER_RECHTSWERT == LOW)
      {
        endschalterrechtstest++;
        newworld ();
      }
    }
  }
  robotspeedsetuplcdweiter  = 0;  //ruecksetzen
    optionenlcdweiter               = 0;  //ruecksetzen
    tasterwahllcdloop     = 0;  //ruecksetzen
  endstopsetuplcdweiter     = 0;  //ruecksetzen 
  }
///////////////////////////////////////////////////////////////////////////
void notausabfrage ()     		  			//Abfrage fuer NOTAUS  
{
  notaus = digitalRead(NOTAUSSCHALTER);
  if (notaus == HIGH)   // WENN NOTAUS BETÄTIGT
  {
    notausquittierung = LOW;
while  (notausquittierung == LOW)   // SOLANGE NOTAUS BETÄTIGT IST
{
  roboterimnotauslcd ();delay (100);
  Serial.println(F("notauswert = "));
  Serial.println(notaus);
  
  TASTER01WERT = digitalRead(TASTER01);TASTER02WERT = digitalRead(TASTER02);notaus = digitalRead(NOTAUSSCHALTER); 
  
  if (TASTER01WERT == LOW && TASTER02WERT == LOW && notaus == LOW)
    { 
    notausquittierung = HIGH;robotspeed = 20;delay (100);
  }
}
  lcd.clear();
  }
  }
///////////////////////////////////////////////////////////////////////////
void newtonpendel() 						//Messvorgang Newtonpendel
{
    for  (magnetspiel_wert=0;magnetspiel_wert<10;) // WENN DAS MAGNETSPIEL DEN WERT 10 ERRIEHCT HAT
    {   
      if (digitalRead(INITZIATOR_MAGNETSPIEL) == LOW)
      {
        magnetspiel_wert++;
        Serial.println(F("Newtonpendel: "));
        Serial.println(magnetspiel_wert);  
        delay (200); 
        
 
    lcd.setCursor(0, 0);
    lcd.print(">>>>>>>>>><<<<<<<<<<");
    lcd.setCursor(0, 1);      
    lcd.print("Counter:   ");      
    lcd.setCursor(0, 2);
    lcd.print(magnetspiel_wert);    lcd.print("       ");
    lcd.setCursor(0, 3);
    lcd.print(">>>>>>>>>><<<<<<<<<<");
    }     
    }
 }
///////////////////////////////////////////////////////////////////////////
void fahreachseninitzialisierung ()         //Lineareinheit initzalisieren
{
///////////////////////////////////////////////////////////////////////////
//////////////////LINEARACHSE_BEWEGUNGS_PROGRAMM///////////////////////////
///////////////////////////////////////////////////////////////////////////

    sucheendschalterlcd ();
        digitalWrite(ENABLE,   LOW);                       //Aktiviere die Motorsteuerung
        digitalWrite(DIRECTION,LOW);
        ENDSCHALTER_LINKSWERT = digitalRead(ENDSCHALTER_LINKS);   
        
        ENDSCHALTER_LINKSWERT = digitalRead(ENDSCHALTER_LINKS);
        while (ENDSCHALTER_LINKSWERT == HIGH)              // Bewege IR solange nach Links bis Endschalter gedrückt wurde = LOW
        {
      digitalWrite(PULS,HIGH);
      delayMicroseconds(LINEAR_ACHSE_SPEED);
      digitalWrite(PULS,LOW);
      delayMicroseconds(LINEAR_ACHSE_SPEED);
      notausabfrage ();
      ENDSCHALTER_LINKSWERT = digitalRead(ENDSCHALTER_LINKS);   
        }

  }
///////////////////////////////////////////////////////////////////////////
void fahreachsenlaengenberechnung ()        //Lineareinheit referenzieren
{
    scannelaengelcd (); // AUFRUF LCD
  
        digitalWrite(ENABLE,   LOW);
        digitalWrite(DIRECTION,HIGH);
        linearlaenge =0;
        ENDSCHALTER_RECHTSWERT = digitalRead(ENDSCHALTER_RECHTS);

        while (ENDSCHALTER_RECHTSWERT == HIGH)              // Bewege IR solange nach Rechts bis Endschalter gedrückt wurde = LOW
        {
      digitalWrite(PULS,HIGH);
      delayMicroseconds(LINEAR_ACHSE_SPEED);
      digitalWrite(PULS,LOW);
      delayMicroseconds(LINEAR_ACHSE_SPEED);
        notausabfrage ();
        linearlaenge ++;                                    // Bestimme die Länge der Achse durch Addieren von 1
      ENDSCHALTER_RECHTSWERT = digitalRead(ENDSCHALTER_RECHTS);
        }

     
        Serial.println(F("###############################"));
        Serial.println(linearlaenge);
        Serial.println(F("=Schritte relative Achsenlänge"));
        Serial.println(F("###############################"));
        zeigelaengelcd (); // AUFRUF LCD
      
  }
///////////////////////////////////////////////////////////////////////////
void fahreachsenachlinks_half ()      		//Lineareinheit fahren Motorrichtung
{
    digitalWrite(ENABLE,   LOW);                       //Aktiviere die Motorsteuerung
        digitalWrite(DIRECTION,LOW);                       //ROBOTER BEWEGT SICH NACH LINKS

        for(SCHRITTE = 0; SCHRITTE < linearlaenge / 2 || ENDSCHALTER_LINKSWERT == HIGH; SCHRITTE++)     // Singlesteps in X RICHTUNG ------14900
    {
      digitalWrite(PULS,HIGH);
      delayMicroseconds(LINEAR_ACHSE_SPEED);
      digitalWrite(PULS,LOW);
      delayMicroseconds(LINEAR_ACHSE_SPEED);
      notausabfrage ();
    }

        digitalWrite(DIRECTION,LOW);//Deaktiviere die Motorsteuerung

}
///////////////////////////////////////////////////////////////////////////
void fahreachsenachlinks_quater ()      	//Lineareinheit fahren Motorrichtung
{
    digitalWrite(ENABLE,   LOW);                       //Aktiviere die Motorsteuerung
        digitalWrite(DIRECTION,LOW);                       //ROBOTER BEWEGT SICH NACH LINKS

        for(SCHRITTE = 0; SCHRITTE < linearlaenge / 4 || ENDSCHALTER_LINKSWERT == HIGH; SCHRITTE++)     // Singlesteps in X RICHTUNG
{
        digitalWrite(PULS,HIGH);
        delayMicroseconds(LINEAR_ACHSE_SPEED);
        digitalWrite(PULS,LOW);
        delayMicroseconds(LINEAR_ACHSE_SPEED);
      notausabfrage ();
}

        digitalWrite(DIRECTION,LOW);//Deaktiviere die Motorsteuerung

}
///////////////////////////////////////////////////////////////////////////
void fahreachsenachrechts_half ()     		//Lineareinheit fahren
{
    digitalWrite(ENABLE,   LOW);                       //Aktiviere die Motorsteuerung
        digitalWrite(DIRECTION,HIGH);                      //ROBOTER BEWEGT SICH NACH RECHTS
        for(SCHRITTE = 0; SCHRITTE < linearlaenge / 2 || ENDSCHALTER_RECHTSWERT == HIGH; SCHRITTE++)     // Singlesteps in X RICHTUNG
{
        digitalWrite(PULS,HIGH);
        delayMicroseconds(LINEAR_ACHSE_SPEED);
        digitalWrite(PULS,LOW);
        delayMicroseconds(LINEAR_ACHSE_SPEED);
      notausabfrage ();
}
        digitalWrite(DIRECTION,LOW);//Deaktiviere die Motorsteuerung

}
///////////////////////////////////////////////////////////////////////////
void fahreachsenachrechts_quater ()     	//Lineareinheit starten
{
    digitalWrite(ENABLE,   LOW);                       //Aktiviere die Motorsteuerung
        digitalWrite(DIRECTION,HIGH);                      //ROBOTER BEWEGT SICH NACH RECHTS
        for(SCHRITTE = 0; SCHRITTE < linearlaenge / 4 || ENDSCHALTER_RECHTSWERT == HIGH; SCHRITTE++)     // Singlesteps in X RICHTUNG
{
        digitalWrite(PULS,HIGH);
        delayMicroseconds(LINEAR_ACHSE_SPEED);
        digitalWrite(PULS,LOW);
        delayMicroseconds(LINEAR_ACHSE_SPEED);
      notausabfrage ();
}
        digitalWrite(DIRECTION,LOW);//Deaktiviere die Motorsteuerung

}
///////////////////////////////////////////////////////////////////////////
void ledtest()                      		//LED TEST
{
  
   if (animationensetup == 1)
{
        digitalWrite(STATUSANZEIGE_PROZLICHT, HIGH);  //Initzialisierungs Anzeige
            delay(zeitledtest);
        digitalWrite(STATUSANZEIGE_PROZLICHT, LOW);  //Initzialisierungs Anzeige
            delay(zeitledtest);
        digitalWrite(STATUSANZEIGE_ROTLICHT, HIGH);  //Initzialisierungs Anzeige
            delay(zeitledtest);
        digitalWrite(STATUSANZEIGE_ROTLICHT, LOW);  //Initzialisierungs Anzeige
            delay(zeitledtest);
}
}
///////////////////////////////////////////////////////////////////////////
void prozessledon()             			//PROZESSLED AN
{
            delay(100);
        digitalWrite(STATUSANZEIGE_PROZLICHT, HIGH);  //Initzialisierungs Anzeige
            delay(200);
        digitalWrite(STATUSANZEIGE_PROZLICHT, LOW);  //Initzialisierungs Anzeige
            delay(100);
        digitalWrite(STATUSANZEIGE_PROZLICHT, HIGH);  //Initzialisierungs Anzeige
            delay(100);
        digitalWrite(STATUSANZEIGE_PROZLICHT, LOW);  //Initzialisierungs Anzeige
            delay(100);
        digitalWrite(STATUSANZEIGE_PROZLICHT, HIGH);  //Initzialisierungs Anzeige
            delay(200);
}
///////////////////////////////////////////////////////////////////////////
void prozessledoff()            			//PROZESSLED AUS
{
        digitalWrite(STATUSANZEIGE_PROZLICHT, LOW);  //Initzialisierungs Anzeige
            delay(100);
        digitalWrite(STATUSANZEIGE_PROZLICHT, HIGH);  //Initzialisierungs Anzeige
            delay(100);
        digitalWrite(STATUSANZEIGE_PROZLICHT, LOW);  //Initzialisierungs Anzeige
            delay(100);
        digitalWrite(STATUSANZEIGE_PROZLICHT, HIGH);  //Initzialisierungs Anzeige
            delay(100);
        digitalWrite(STATUSANZEIGE_PROZLICHT, LOW);  //Initzialisierungs Anzeige
            delay(100);
}
///////////////////////////////////////////////////////////////////////////
void beleuchtungsledon()          			//ROTE LEDS AN
{	
            delay(100);
        digitalWrite(STATUSANZEIGE_ROTLICHT, HIGH);  //Initzialisierungs Anzeige
            delay(100);
        digitalWrite(STATUSANZEIGE_ROTLICHT, LOW);  //Initzialisierungs Anzeige
            delay(100);
        digitalWrite(STATUSANZEIGE_ROTLICHT, HIGH);  //Initzialisierungs Anzeige
            delay(100);            
}
///////////////////////////////////////////////////////////////////////////
void beleuchtungsledoff()         			//ROTE LEDS AUS
{
            delay(100);
        digitalWrite(STATUSANZEIGE_ROTLICHT, LOW);  //Initzialisierungs Anzeige
            delay(100);
        digitalWrite(STATUSANZEIGE_ROTLICHT, HIGH);  //Initzialisierungs Anzeige
            delay(100);
        digitalWrite(STATUSANZEIGE_ROTLICHT, LOW);  //Initzialisierungs Anzeige
            delay(100);       
}
///////////////////////////////////////////////////////////////////////////
void newworld()               				//SOUND
{
   if (animationensetup == 1)
{
  
        tone      (2, 500);
        delay     (100);
        noTone    (2);
        tone      (2, 430);
        delay     (100);
        noTone    (2);
        tone      (2, 500);
        delay     (100);
        noTone    (2);
        tone      (2, 800);
        delay     (200);
        noTone    (2);
}
}
///////////////////////////////////////////////////////////////////////////
void soundalive()             				//SOUND
{
        tone      (2, 400);
        delay     (100);
        noTone    (2);
        tone      (2, 600);
        delay     (200);
        noTone    (2);
}
///////////////////////////////////////////////////////////////////////////
void sounddown()              				//SOUND
{
        tone      (2, 600);
        delay     (100);
        noTone    (2);
        tone      (2, 400);
        delay     (200);
        noTone    (2);
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

void noSdCardlcd()           				//LCD ANZEIGE
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" ");   
    lcd.setCursor(0, 1);
    lcd.print("      SD Karte      ");     
    lcd.setCursor(0, 2);
    lcd.print("   nicht gefunden!  ");  
    lcd.setCursor(0, 3);
    lcd.print("");  
}
///////////////////////////////////////////////////////////////////////////
void noPrgFound()           				//LCD ANZEIGE
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("                    ");   
    lcd.setCursor(0, 1);
    lcd.print("     Teachmodus     ");     
    lcd.setCursor(0, 2);
    lcd.print("    aktivieren!     ");  
    lcd.setCursor(0, 3);
    lcd.print("");  
	delay (1000);
}
///////////////////////////////////////////////////////////////////////////
void foundSdCardlcd()           			//LCD ANZEIGE
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" ");   
    lcd.setCursor(0, 1);
    lcd.print("      SD Karte      ");     
    lcd.setCursor(0, 2);
    lcd.print("     gefunden!     ");  
    lcd.setCursor(0, 3);
    lcd.print("");  
}
///////////////////////////////////////////////////////////////////////////
void checkSdProgramlcd()          			//LCD ANZEIGE
{
	  lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("   START Optionen   ");     

    lcd.setCursor(0, 2);  
    lcd.print("-S1         ...Laden");

    lcd.setCursor(0, 3);
    lcd.print("-S2       Neuer Pfad");    

	
	while (checksd == 0)
	{
	 if(!digitalRead(TASTER02)) // WENN 1 GEDRÜCKT WURDE
      {
		checksd = 1;		  
	  lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("      Erstelle      ");  
    lcd.setCursor(0, 2);
    lcd.print("     neuen Pfad     ");
	  newworld();
	  }
	 if(!digitalRead(TASTER01)) // WENN 2 GEDRÜCKT WURDE
    {
    lcd.clear();
    lcd.setCursor(0, 1); 
    lcd.print("        LADE      ");
    lcd.setCursor(0, 2);          
    lcd.print("    IN SPEICHER   "); 
		checksd = 2;
	  newworld();
	  }
	  delay(1000);
	}
	
}
///////////////////////////////////////////////////////////////////////////
void hint1lcd()								//LCD ANZEIGE
{   
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("    WAHLSCHALTER    ");  
  lcd.setCursor(0, 2);
  lcd.print("     AUF TEACH      ");  
  delay(1000);  
}
///////////////////////////////////////////////////////////////////////////
void createNewPath()          				//LCD ANZEIGE
{
	  lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("      Erstelle      ");  
    lcd.setCursor(0, 2);
    lcd.print("     neuen Pfad     ");	
		  delay(1000);
}
///////////////////////////////////////////////////////////////////////////
void selectMemorySocket()           		//LCD ANZEIGE
{
/*	lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" ");   
    lcd.setCursor(0, 1);
    lcd.print("        Punkt      ");     
    lcd.setCursor(0, 2);
    lcd.print("      erstellt!     ");  
    lcd.setCursor(0, 3);
    lcd.print("");  
*/	delay(500);
}
///////////////////////////////////////////////////////////////////////////
void showErroronlcd ()           			//LCD ANZEIGE
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("                    ");   
    lcd.setCursor(0, 1);
    lcd.print("   Kein Programm    ");     
    lcd.setCursor(0, 2);
    lcd.print("     gefunden!      ");  
    lcd.setCursor(0, 3);
    lcd.print("                    ");  
    delay(1500);
}
///////////////////////////////////////////////////////////////////////////
void showMemoryonlcd()          			//LCD ANZEIGE
{    
 
for (int i=0; i<=checkint;i++)
{

	lcd.clear();
    lcd.setCursor(0, 0);	  
	lcd.print("Punkt=");
   
   lcd.setCursor(6, 0);

    lcd.print(i+1);

	if (prs[i].a4<0 || prs[i].a4>28000) {lcd.setCursor(12, 0);   lcd.print("FEHLER");   } // ZEIGE BEI FALSCHEN WERTEN EINEN FEHLER
	 else // CHECKE NACH KORREKTEN WERTEN
	{
		lcd.setCursor(12, 0);   
		lcd.print("A4=");      
		lcd.setCursor(15, 0);
		lcd.print(prs[i].a4);
	}
 
    lcd.setCursor(0, 2);	  
	lcd.print("A1="); 	
    lcd.setCursor(3, 2);	  
    lcd.print(prs[i].a1);
    	
    lcd.setCursor(8, 2);    
	lcd.print("A2=");   
    lcd.setCursor(11, 2);    
    lcd.print(prs[i].a2); 

    lcd.setCursor(15, 2);    
	lcd.print("A3="); 
    lcd.setCursor(18, 2);
    lcd.print(prs[i].a3);
   
   delay (300);
}
    lcd.clear();
    lcd.setCursor(0, 1); 
    lcd.print("    ERFOLGREICH   ");
    lcd.setCursor(0, 2);          
    lcd.print("   ABGESCHLOSSEN  ");     

   delay (1000);    
}
///////////////////////////////////////////////////////////////////////////
void tasterwahllcd ()          				//LCD ANZEIGE
{
      lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print(" Pluna Programmwahl");   

    delay (100);
    lcd.setCursor(0, 2);
    lcd.print("                  <=");  
    lcd.setCursor(0, 3);
    lcd.print("                  <=");

    delay (100);
    
    lcd.setCursor(0, 2);
    if (referenzierungsstatus == 0)
         lcd.print("               <=  R");
    else lcd.print("               <=  S");  

    lcd.setCursor(0, 3);
    lcd.print("               <=  O");

                delay (100);
    
    lcd.setCursor(0, 2);
    if (referenzierungsstatus == 0)
         lcd.print("            <=  Ref.");
    else lcd.print("            <=  Star");
  
    lcd.setCursor(0, 3);
    lcd.print("            <=  Opti");

                delay (100);

    lcd.setCursor(0, 2);
    if (referenzierungsstatus == 0)
         lcd.print("          <=  Ref. F");
    else lcd.print("          <=  Start ");  
    lcd.setCursor(0, 3);
    lcd.print("          <=  Option");

                 delay (100);

    lcd.setCursor(0, 2);
    if (referenzierungsstatus == 0)
         lcd.print("1       <=  Ref. Fah");
    else lcd.print("1       <=  Start Ro");  
    
    lcd.setCursor(0, 3);
    lcd.print("2       <=  Optionen");
 
                delay (100);
    lcd.setCursor(0, 2);
    if (referenzierungsstatus == 0)
         lcd.print("1     <=  Ref. Fahrt");
    else lcd.print("1     <=  Start Robo"); 

     
    lcd.setCursor(0, 3);
    lcd.print("2     <=  Optionen  ");

                delay (100);
      
    lcd.setCursor(0, 2);
    if (referenzierungsstatus == 0)
         lcd.print(" 1  <=  Ref. Fahrt  ");
    else lcd.print(" 1  <=  Start Robot ");  
    lcd.setCursor(0, 3);
         lcd.print(" 2  <=  Optionen    ");

}
///////////////////////////////////////////////////////////////////////////
void kugelsuchelcd ()           			//LCD ANZEIGE
{
Serial.println(F("####################################"));
Serial.println(F("### Suche eine Kugel   ?<(^.^ )>P ##"));
Serial.println(F("####################################"));
      lcd.clear();

 if (animationensetup == 1)
{
    lcd.setCursor(0, 0);
    lcd.print("");
    lcd.setCursor(0, 1);
    lcd.print("Okay suche       ");
    lcd.setCursor(0, 2);      
    lcd.print("eine Kugel! (-.o )  ");

                delay (100);
    lcd.setCursor(0, 3);
    lcd.print("o");
                delay (100);
    lcd.setCursor(0, 3);
    lcd.print("  O");
                delay (100);
    lcd.setCursor(0, 3);
    lcd.print("     o");
                delay (100);
    lcd.setCursor(0, 3);
    lcd.print("        O");
                delay (100);
    lcd.setCursor(0, 3);
    lcd.print("           o");
                delay (100);
    lcd.setCursor(0, 3);
    lcd.print("              O");
                delay (100);
    lcd.setCursor(0, 3);
    lcd.print("               o");
                delay (100);
    lcd.setCursor(0, 3);
    lcd.print("                 o");
                delay (100);
    lcd.setCursor(0, 3);
    lcd.print("                  O");
                delay (100);
    lcd.setCursor(0, 3);
    lcd.print("                   o");

                delay (100);
    lcd.setCursor(0, 2);      
    lcd.print("eine Kugel! ( -.-) ?");
                delay (100);
    lcd.setCursor(0, 2);      
    lcd.print("eine Kugel! ( -.o) ?");
  }
}
///////////////////////////////////////////////////////////////////////////
void kugelgefundenlcd ()          			//LCD ANZEIGE
{
  
Serial.println(F("############################----<(o_o<)--##"));
Serial.println(F("### Habe eine Kugel gefunden    +!o!++ #####"));
Serial.println(F("###############################--++++--#####"));
    lcd.clear();
 if (animationensetup == 1)
{     
    lcd.setCursor(0, 0);
    lcd.print("              !!!");
    lcd.setCursor(0, 1);
    lcd.print("Habe eine    (O.O)  ");  
    lcd.setCursor(0, 2);
    lcd.print("Kugel               ");
                delay (flytime);
    lcd.setCursor(0, 3);
    lcd.print("gefunden!       o   ");
                delay (300);
    lcd.setCursor(0, 3);
    lcd.print("gefunden!    +*!o!*+"); 
                delay (300);
    lcd.setCursor(0, 3);
    lcd.print("gefunden!       o   ");
                delay (300);
    lcd.setCursor(0, 3); 
    
    lcd.setCursor(0, 2);
    lcd.print("Kugel               ");    
    
                delay (300);
    lcd.setCursor(0, 3); 
    lcd.print("gefunden!    +*!o!*+"); 
                delay (300);
    lcd.setCursor(0, 3);
    lcd.print("gefunden!      >o<  ");
}
}
///////////////////////////////////////////////////////////////////////////
void kugelinsnestlcd ()           			//LCD ANZEIGE
{       
Serial.println                (F("#########################################"));
Serial.println                (F("### Lege die Kugel ins Nest  o<(^_^<) ###"));
Serial.println                (F("#########################################"));
      lcd.clear();
  if (animationensetup == 1)
{     
      
    lcd.setCursor(0, 0);
    lcd.print("                    ");
    lcd.setCursor(0, 1);
    lcd.print("Lege die Kugel  /o/ ");
    lcd.setCursor(0, 2);
    lcd.print("ins Nest      (^-^) ");
    delay (flytime);
    lcd.setCursor(0, 1);
    lcd.print("Lege die Kugel /O/  ");
    lcd.setCursor(0, 2);
    lcd.print("ins Nest      (^o^) ");
    delay (flytime);
    lcd.setCursor(0, 1);
    lcd.print("Lege die Kugel  /o/ ");
    lcd.setCursor(0, 2);
    lcd.print("ins Nest      (^-^) ");
    delay (flytime);
    lcd.setCursor(0, 1);
    lcd.print("Lege die Kugel /O/  ");
    lcd.setCursor(0, 2);
    lcd.print("ins Nest      (^o^) ");
    delay (flytime);
    lcd.setCursor(0, 1);
    lcd.print("Lege die Kugel  /o/ ");
    lcd.setCursor(0, 2);
    lcd.print("ins Nest      (^-^) ");    
    
    
    lcd.setCursor(0, 3);
    lcd.print("                    ");
  }
}
///////////////////////////////////////////////////////////////////////////
void kugelablegenlcd ()           			//LCD ANZEIGE
{      
Serial.println(F("###########################----I-I---###"));
Serial.println(F("### Lege die Kugel wieder ab---I-I---###"));
Serial.println(F("##########################---( '.' )-###"));

 if (animationensetup == 1)
{
  
      delay (flytime);    
    lcd.setCursor(0, 0);
    lcd.print("                    ");
    lcd.setCursor(0, 1);
    lcd.print("Lege die Kugel      ");  
    lcd.setCursor(0, 2);
    lcd.print("wieder ab!          ");
    lcd.setCursor(0, 3);
    lcd.print("                    ");
  
    delay (flytime);    
    lcd.setCursor(0, 0);
    lcd.print("             ( '.' )");
    lcd.setCursor(0, 1);
    lcd.print("Lege die Kugel      ");  
    lcd.setCursor(0, 2);
    lcd.print("wieder ab!          ");
    lcd.setCursor(0, 3);
    lcd.print("                    ");
    
    delay (flytime);    
    lcd.setCursor(0, 0);
    lcd.print("                 I-I");
    lcd.setCursor(0, 1);
    lcd.print("Lege die Kuge( '.' )");  
    lcd.setCursor(0, 2);
    lcd.print("wieder ab!          ");
    lcd.setCursor(0, 3);
    lcd.print("                    ");
    
    delay (flytime);    
    lcd.setCursor(0, 0);
    lcd.print("                I-I ");
    lcd.setCursor(0, 1);
    lcd.print("Lege die Kugel I-I  ");  
    lcd.setCursor(0, 2);
    lcd.print("wieder ab!  ( '.' ) ");
    lcd.setCursor(0, 3);
    lcd.print("                    ");  
      
    delay (flytime);      
    lcd.setCursor(0, 0);
    lcd.print("                 I-I");
    lcd.setCursor(0, 1);
    lcd.print("Lege die Kugel  I-I ");  
    lcd.setCursor(0, 2);
    lcd.print("wieder ab!     I-I  ");
    lcd.setCursor(0, 3);
    lcd.print("            ( '.' ) ");
  
  
    delay (flytime);      
    lcd.setCursor(0, 0);
    lcd.print("                 I-I");
    lcd.setCursor(0, 1);
    lcd.print("Lege die Kugel  I-I ");  
    lcd.setCursor(0, 2);
    lcd.print("wieder ab!          ");
    lcd.setCursor(0, 3);
    lcd.print("            ( '.' ) ");  
    
    delay (flytime);      
    lcd.setCursor(0, 0);
    lcd.print("                 I-I");
    lcd.setCursor(0, 1);
    lcd.print("Lege die Kugel      ");
        
    delay (flytime);      
    lcd.setCursor(0, 0);
    lcd.print("                    ");

} 
}
///////////////////////////////////////////////////////////////////////////
void kugelfertiglcd ()						//LCD ANZEIGE
{   
Serial.println(F("###################################"));
Serial.println(F("### Fertig! Nochmal? <('.'-^)######"));
Serial.println(F("###################################"));
      lcd.clear();
 if (animationensetup == 1)
{   
    lcd.setCursor(0, 0);
    lcd.print("                    ");
    lcd.setCursor(0, 1);
    lcd.print("Fertig!          O  "); 
    lcd.setCursor(0, 2); 
    lcd.print("Nochmal?   <=('-'^) ");
    lcd.setCursor(0, 3);
    lcd.print("                    ");
    delay (flytime);
    lcd.setCursor(0, 0);
    lcd.print("                    ");
    lcd.setCursor(0, 1);
    lcd.print("Fertig!         o   "); 
    lcd.setCursor(0, 2); 
    lcd.print("Nochmal?   <=('-' ) ");
    lcd.setCursor(0, 3);
    lcd.print("                    ");
    delay (flytime);    
    lcd.setCursor(0, 0);
    lcd.print("                    ");
    lcd.setCursor(0, 1);
    lcd.print("Fertig!          O  "); 
    lcd.setCursor(0, 2); 
    lcd.print("Nochmal?   <=('-'^) ");
    lcd.setCursor(0, 3);
    lcd.print("                    ");
}
}
///////////////////////////////////////////////////////////////////////////
void sucheendschalterlcd ()         		//LCD ANZEIGE
{    
Serial.println(F("#########################I<<<<++++"));
Serial.println(F("###Suche den Endschalter I t(-_-t)"));
Serial.println(F("#########################I<<<<++++"));
 if (animationensetup == 1)
{ 
 
                delay (150);
    lcd.setCursor(0, 0);
    lcd.print("#########I         <");
    lcd.setCursor(0, 1);
    lcd.print("Suche    I         <"); 
    lcd.setCursor(0, 2); 
    lcd.print("Endstop  I         <");
    lcd.setCursor(0, 3);
    lcd.print("#########I         <"); 

                delay (150);
    lcd.setCursor(0, 0);
    lcd.print("#########I       <  ");
    lcd.setCursor(0, 1);
    lcd.print("Suche    I       <  "); 
    lcd.setCursor(0, 2); 
    lcd.print("Endstop  I       <t(");
    lcd.setCursor(0, 3);
    lcd.print("#########I       <  "); 

                delay (150);
    lcd.setCursor(0, 0);
    lcd.print("#########I     <    ");
    lcd.setCursor(0, 1);
    lcd.print("Suche    I     <    "); 
    lcd.setCursor(0, 2); 
    lcd.print("Endstop  I     <t('.");
    lcd.setCursor(0, 3);
    lcd.print("#########I     <    "); 
        delay (150);
    lcd.setCursor(0, 0);
    lcd.print("#########I    <     ");
    lcd.setCursor(0, 1);
    lcd.print("Suche    I    <     "); 
    lcd.setCursor(0, 2); 
    lcd.print("Endstop  I    <t('.'");
    lcd.setCursor(0, 3);
    lcd.print("#########I    <     "); 
    delay (150);
    lcd.setCursor(0, 0);
    lcd.print("#########I  <       ");
    lcd.setCursor(0, 1);
    lcd.print("Suche    I  <       "); 
    lcd.setCursor(0, 2); 
    lcd.print("Endstop  I  <t('.'t)");
    lcd.setCursor(0, 3);
    lcd.print("#########I  <       "); 
    delay (150);
    lcd.setCursor(0, 0);
    lcd.print("#########I <        ");
    lcd.setCursor(0, 1);
    lcd.print("Suche    I <        "); 
    lcd.setCursor(0, 2); 
    lcd.print("Endstop  I <t('.'t) ");
    lcd.setCursor(0, 3);
    lcd.print("#########I <        "); 
}
}
///////////////////////////////////////////////////////////////////////////
void scannelaengelcd ()           			//LCD ANZEIGE
{
Serial.println(F("#############################+-*:####"));
Serial.println(F("##Berechne die Achsenlänge  (-_-)###"));
Serial.println(F("#############################+-*:####"));

          lcd.clear();
 if (animationensetup == 1)
{
                delay (100);
    lcd.setCursor(0, 0);
    lcd.print("                   I");
     lcd.setCursor(0, 1);
    lcd.print("Scanne die         I");
    lcd.setCursor(0, 2);
    lcd.print("Achsenlaenge       I");
    lcd.setCursor(0, 3);
    lcd.print("                  I ");
                delay (100);
    lcd.setCursor(0, 0);
    lcd.print("                 I  ");
    lcd.setCursor(0, 1);
    lcd.print("Scanne die      I   ");
    lcd.setCursor(0, 2);
    lcd.print("Achsenlaenge     I  ");
    lcd.setCursor(0, 3);
    lcd.print("                I   ");
                delay (100);
    lcd.setCursor(0, 0);
    lcd.print("               I    ");
    lcd.setCursor(0, 1);
    lcd.print("Scanne die       I  ");
    lcd.setCursor(0, 2);
    lcd.print("Achsenlaenge    I   ");
    lcd.setCursor(0, 3);
    lcd.print("                I   ");
    
                delay (100);
    lcd.setCursor(0, 0);
    lcd.print("              I     ");
    lcd.setCursor(0, 1);
    lcd.print("Scanne die     I    ");
    lcd.setCursor(0, 2);
    lcd.print("Achsenlaenge  I     ");
    lcd.setCursor(0, 3);
    lcd.print("             I      ");
        delay (100);
    lcd.setCursor(0, 0);
    lcd.print("              I     ");
    lcd.setCursor(0, 1);
    lcd.print("Scanne die      I   ");
    lcd.setCursor(0, 2);
    lcd.print("Achsenlaenge   I    ");
    lcd.setCursor(0, 3);
    lcd.print("               I    ");
        delay (100);
    lcd.setCursor(0, 0);
    lcd.print("              (-_o) ");
    lcd.setCursor(0, 1);
    lcd.print("Scanne die      I   "); 
    lcd.setCursor(0, 2);
    lcd.print("Achsenlaenge    I   ");
    lcd.setCursor(0, 3);
    lcd.print("                I   ");
        delay (100);
    lcd.setCursor(0, 0);
    lcd.print("                I   ");
    lcd.setCursor(0, 1);
    lcd.print("Scanne die    (o_o) "); 
    lcd.setCursor(0, 2);
    lcd.print("Achsenlaenge    I   ");
    lcd.setCursor(0, 3);
    lcd.print("                I   ");  
    delay (100);
    lcd.setCursor(0, 0);
    lcd.print("                I   ");
    lcd.setCursor(0, 1);
    lcd.print("Scanne die      I   "); 
    lcd.setCursor(0, 2);
    lcd.print("Achsenlaenge  (o_o) ");
    lcd.setCursor(0, 3);
    lcd.print("                I   ");
        delay (100);
    lcd.setCursor(0, 0);
    lcd.print("                I   ");
    lcd.setCursor(0, 1);
    lcd.print("Scanne die      I   "); 
    lcd.setCursor(0, 2);
    lcd.print("Achsenlaenge    I   ");
    lcd.setCursor(0, 3);
    lcd.print("              (*_*) ");
    
}}    
///////////////////////////////////////////////////////////////////////////
void zeigelaengelcd ()            			//LCD ANZEIGE     
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("                    ");
    lcd.setCursor(0, 1);
    lcd.print("Laenge der Achse    "); 
    
    lcd.setCursor(0, 2);
    lcd.print("Steps =");
    lcd.setCursor(8, 2);
    lcd.print(linearlaenge);

    float show = ((float)linearlaenge / 1000);

    lcd.setCursor(0, 3);
    lcd.print("    cm=");
    lcd.setCursor(8, 3);
    lcd.print(show); 
}
///////////////////////////////////////////////////////////////////////////     
void optionenlcd ()             			//LCD ANZEIGE
{   
  lcd.clear();
      
    lcd.setCursor(0, 0);
    lcd.print("Ref.status = ");
    lcd.setCursor(13, 0);
    lcd.print(referenzierungsstatus);
    
    lcd.setCursor(0, 1);
    lcd.print("Achslaenge = ");
    lcd.setCursor(13, 1);
    lcd.print(linearlaenge);
    
    lcd.setCursor(0, 2);
    lcd.print("Robotspeed = ");
    lcd.setCursor(13, 2);
    lcd.print(robotspeed);
    

    lcd.setCursor(0, 3);
    lcd.print("1=HOME      2=Weiter");
}
///////////////////////////////////////////////////////////////////////////     
void robotspeedsetuplcd()        			//LCD ANZEIGE
{
    lcd.clear();
      
    lcd.setCursor(0, 0);
    lcd.print("Robspeed =");
    lcd.setCursor(11,0);
    lcd.print(robotspeed) ;
    
    lcd.setCursor(0, 1);
    lcd.print("s10 m20 f30"); 
    
    lcd.setCursor(0, 2);
    lcd.print("               Speed");
    lcd.setCursor(0, 3);
    lcd.print("1=Weiter    2=Change");
}
///////////////////////////////////////////////////////////////////////////     
void endstopsetuplcd ()           			//LCD ANZEIGE
{
    lcd.clear();      
    lcd.setCursor(0, 0);
    lcd.print(">>>>>>>>>><<<<<<<<<<");
    lcd.setCursor(0, 1);
    lcd.print(">>Endschalter Test<<");
    lcd.setCursor(0, 2);
    lcd.print(">>>>>>>>>><<<<<<<<<<");
    lcd.setCursor(0, 3);
    lcd.print("1=Weiter     2=Start");    
}
///////////////////////////////////////////////////////////////////////////   
void animationensetuplcd ()         		//LCD ANZEIGE
{
  
    lcd.clear();      
    lcd.setCursor(0, 0);
    lcd.print(">>>>>>>>>><<<<<<<<<<");
    lcd.setCursor(0, 1);
    lcd.print(">>Display =       <<");
    lcd.setCursor(12, 1);
    lcd.print (animationensetup);
    lcd.setCursor(0, 2);
    lcd.print(">>Animationen ??? <<");
    lcd.setCursor(0, 3);
    lcd.print("1=Home         2=AUS");    
}
///////////////////////////////////////////////////////////////////////////   
void chooseSaveOption ()           			//LCD ANZEIGE
{ 
  newworld();           //HELLO SOUND
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("   Punkt speichern  ");
	lcd.setCursor(0, 1);
	lcd.print("<<<<<<< (-.-) >>>>>>");
	lcd.setCursor(0, 2);
  lcd.print("<<<<<<<  ???  >>>>>>");
	lcd.setCursor(0, 3);
	lcd.print("Overwrite    Add-New");	
} 
///////////////////////////////////////////////////////////////////////////  
void beforeOrAfterlcd ()                	//LCD ANZEIGE
{ 
  newworld();           //HELLO SOUND
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   Punkt einfuegen  ");
  lcd.setCursor(0, 1);
  lcd.print("<<<<<<< (-.-) >>>>>>");
  lcd.setCursor(0, 2);
  lcd.print("<<<<<<<  ???  >>>>>>");
  lcd.setCursor(0, 3);
  lcd.print("Befor?        After?"); 
}  
///////////////////////////////////////////////////////////////////////////   
void endstoplinkslcd ()           			//LCD ANZEIGE
{   
    lcd.setCursor(0, 0);
    lcd.print(">>>>>>>>>><<<<<<<<<<");
    lcd.setCursor(0, 1);      
    lcd.print("Endschalter Links   ");      
    lcd.setCursor(0, 2);      
    lcd.print("betaetigen          ");
    lcd.setCursor(0, 3);
    lcd.print(">>>>>>>>>><<<<<<<<<<");
}
///////////////////////////////////////////////////////////////////////////     
void endstoprechtslcd ()          			//LCD ANZEIGE
{   
    lcd.setCursor(0, 0);
    lcd.print(">>>>>>>>>><<<<<<<<<<");
    lcd.setCursor(0, 1);      
    lcd.print("Endschalter Rechts  ");      
    lcd.setCursor(0, 2);      
    lcd.print("betaetigen          ");
    lcd.setCursor(0, 3);
    lcd.print(">>>>>>>>>><<<<<<<<<<");
  }
///////////////////////////////////////////////////////////////////////////     
void tcpreferenzlcd ()            			//LCD ANZEIGE
{
    lcd.clear();

    delay (100);     
    lcd.setCursor(0, 3);
    lcd.print("   TCP-Kontrolle   "); 

    delay (100);     
    lcd.setCursor(0, 2);
    lcd.print("   TCP-Kontrolle   ");
    delay (100);     
    lcd.setCursor(0, 3);
    lcd.print("                   ");

    delay (100);     
    lcd.setCursor(0, 1);
    lcd.print("   TCP-Kontrolle   ");
    delay (100);     
    lcd.setCursor(0, 2);
    lcd.print("                   ");
    delay (100);     
    lcd.setCursor(0, 3);
    lcd.print("   Automatischer   ");

    delay (100);     
    lcd.setCursor(0, 0);
    lcd.print("   TCP-Kontrolle   ");
    delay (100);     
    lcd.setCursor(0, 1);
    lcd.print("                   ");
    delay (100);     
    lcd.setCursor(0, 2);    
    lcd.print("   Automatischer   ");
    delay (100);     
    lcd.setCursor(0, 3);
    lcd.print("     Suchlauf      ");     

      delay (1000);
}
///////////////////////////////////////////////////////////////////////////     
void roboterinitfertiglcd()         		//LCD ANZEIGE
{
    lcd.clear();
      
    lcd.setCursor(0, 0);
    lcd.print("                    ");
  
    lcd.setCursor(0, 1);
    lcd.print("  Initzalisierung   ");
      
    lcd.setCursor(0, 2);
    lcd.print("    erfolgreich!    ");
          
    lcd.setCursor(0, 3);
    lcd.print("       (^.^)/       ");
}
///////////////////////////////////////////////////////////////////////////
void roboterimnotauslcd()         			//LCD ANZEIGE
{
                lcd.clear();

                lcd.setCursor(0, 0);
                lcd.print("  !!!   NOTAUS  !!!  ");

                lcd.setCursor(0, 1);
                lcd.print("  !!!   NOTAUS  !!!  ");

                lcd.setCursor(0, 2);
                lcd.print("  !!!   NOTAUS  !!!  ");

                lcd.setCursor(0, 3);
                lcd.print("QUITIERUNG MIT S1^S2 ");
}
///////////////////////////////////////////////////////////////////////////
void welcomeLcd()                 			//LCD ANZEIGE
{
    lcd.clear();
    lcd.setCursor(0, 0);  
    lcd.print("   ");
            
    delay (flytime); 
    lcd.setCursor(0, 2);  
    lcd.print(" =>        ");  
    delay (flytime); 
    lcd.setCursor(0, 2);  
    lcd.print("-')=>               ");  
    delay (flytime); 
    lcd.setCursor(0, 2);  
    lcd.print("<=('-')======>      ");    
    delay (flytime); 
    lcd.setCursor(0, 2);  
    lcd.print("  ('-')<========>   ");  
    delay (flytime); 
    lcd.setCursor(0, 2);  
    lcd.print("    ('-') <========>");  
    delay (flytime); 
    lcd.setCursor(0, 2);  
    lcd.print("    ('-')   <=======");  
    delay (flytime); 
    lcd.setCursor(0, 2);  
    lcd.print("    ('-')       <===");  
    delay (flytime); 
    lcd.setCursor(0, 2);  
    lcd.print("    ('-')         <=");  
    delay (flytime); 
    lcd.setCursor(0, 2);  
    lcd.print("    ('-')       <===");      
    delay (flytime); 
    lcd.setCursor(0, 2);  
    lcd.print("    ('-')     <=====");    
    delay (flytime); 
    lcd.setCursor(0, 2);  
    lcd.print("    ('-')   <=======");  
    delay (flytime); 
    lcd.setCursor(0, 2);  
    lcd.print("    ('-')  <========");  
    delay (flytime); 
    lcd.setCursor(0, 2);  
    lcd.print("    ('-')<==========");
    delay (flytime); 
    lcd.setCursor(0, 2);  
    lcd.print("     ('-')<=========");
    delay (flytime); 
    lcd.setCursor(0, 2);  
    lcd.print("      ('-')<========");      
    delay (flytime); 
    lcd.setCursor(0, 2);  
    lcd.print("       ('-')<=======");    
    delay (flytime); 
    lcd.setCursor(0, 2);  
    lcd.print(">       ('-') <=====");
    delay (flytime); 
    lcd.setCursor(0, 2);  
    lcd.print("==>     ('-')  <====");        
    delay (flytime); 
    lcd.setCursor(0, 2);  
    lcd.print("===>    ('-')  <====");  
      delay (flytime); 
    lcd.setCursor(0, 2);  
    lcd.print("=====>  ('-')   <===");        
      delay (flytime); 
    lcd.setCursor(0, 2);  
    lcd.print("======> ('-')    <==");          
      delay (flytime); 
    lcd.setCursor(0, 2);  
    lcd.print("========('-')      <");          
      delay (flytime); 
    lcd.setCursor(0, 2);  
    lcd.print(" <======('-')=>     ");      
      delay (flytime);    
    lcd.setCursor(0, 2);  
    lcd.print("     <==('-')=>     ");        
    delay (flytime); 
    lcd.setCursor(0, 2);  
    lcd.print("      <=('-')=>     ");  
        delay (flytime); 
    lcd.setCursor(0, 2);  
    lcd.print("    <== ('-')==>    ");
        delay (flytime); 
    lcd.setCursor(0, 2);  
    lcd.print("   <==  ('-') ==>   ");
        delay (flytime); 
    lcd.setCursor(0, 2);  
    lcd.print("  <==   ('-')  ==>  ");
        delay (flytime); 
    lcd.setCursor(0, 2);  
    lcd.print(" <==    ('-')   ==> ");
        delay (flytime); 
    lcd.setCursor(0, 2);  
    lcd.print("<==     ('-')    ==>");
        delay (flytime); 
    lcd.setCursor(0, 2);  
    lcd.print("==      ('-')     ==");
        delay (flytime); 
    lcd.setCursor(0, 2);  
    lcd.print("=       ('-')      =");
    delay (flytime); 
    lcd.setCursor(0, 2);  
    lcd.print("        ('-')       ");    
        
    delay (flytime);
    lcd.setCursor(0, 1);
        lcd.print("ics ");
        delay (flytime);
    lcd.setCursor(0, 1);
        lcd.print("Robotics ");
        delay (flytime);
        lcd.setCursor(0, 1);
        lcd.print("na-Robotics ");
        delay (flytime); 
        lcd.setCursor(0, 1);
        lcd.print("Pluna-Robotics "); 
        delay (flytime); 
        lcd.setCursor(0, 1);
        lcd.print("   Pluna-Robotics "); 
    
    delay (250);
    lcd.setCursor(0, 3);
    lcd.print("                   V"); 
    delay (flytime); 
    lcd.setCursor(0, 3);
    lcd.print("                  V0"); 
    delay (flytime); 
    lcd.setCursor(0, 3);
    lcd.print("                 V03"); 
    
    delay (500);
    lcd.setCursor(0, 2);  
    lcd.print("        ('-*)       ");
    delay (flytime); 
    lcd.setCursor(0, 2);  
    lcd.print("        ('-')       ");    
    delay (250);    
}
///////////////////////////////////////////////////////////////////////////
void robotactivaxislcd(uint8_t d,bool motion,uint8_t speedmot, uint8_t whatpos)	//LCD ANZEIGE
{
    lcd.setCursor(0, 2);
    lcd.print("Activ ");

  if (d<=4)
  {
     lcd.setCursor(6, 2);   
     lcd.print(d) ;
    lcd.print("     ");  
  }
  else if (d==5)
  {
     lcd.setCursor(6, 2);   
     lcd.print("Gr") ;
  }
  else if (d==6)
  {
     lcd.setCursor(6, 2);   
     lcd.print("Le") ;
  }  
  else if (d==7)
  {
     lcd.setCursor(6, 2);   
     lcd.print("Sp") ;
  }    
  else if (d==8)
  {
     lcd.setCursor(6, 2);   
     lcd.print("Mo") ;
  }
    lcd.setCursor(9,2);
    lcd.print("Sp=");
    lcd.setCursor(12,2);
    lcd.print(speedmot);

    lcd.setCursor(15,2);
    lcd.print("Mo=");
    lcd.setCursor(18,2);
    lcd.print(motion);  

   lcd.setCursor(0, 3);
   lcd.print("Teach");
      
   lcd.setCursor(8, 3);
   lcd.print("Pos");

  if(teachInPosition)
  {
  lcd.setCursor(12, 3);
  lcd.print(actualPos);
    }
  else { 
  lcd.setCursor(12, 3);
  lcd.print(whatpos);
   }
   
   if (teachInPosition)
   {
   lcd.setCursor(15, 3);
   lcd.print("Drive");
   }
}
///////////////////////////////////////////////////////////////////////////
