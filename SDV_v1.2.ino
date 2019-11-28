#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Keyboard.h"
#include <EEPROM.h>
#include "RTClib.h"
#include <OneWire.h>
#include <DallasTemperature.h>

// bill define..................................................................
#define ret 176    //return
#define cont 128   // ball control bill
#define upp 218    // fel nyil
#define down 217   //le nyil
#define left 216   //ball nyil
#define right 215  // jobb nyil
#define alt 130    // ALT ball bill
#define back 178   //blackspace
#define TAB 179    //TAB

//Zene hang define.......................................................................
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

//... zene data.................................................................
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};
int thisNote;
int noteDuration;
int pauseBetweenNotes;
//..sound data...............................
#define bipp 50
#define rovid 200
#define kozepes 500
#define hosszu 1000
#define nagyonhosszu 2000

#define alacson 100
#define normal 500
#define nagas 1000
#define csip 2000


int hossz;
int hz;
byte hang=1; // hang enged ha 1 akkor van 

//...........................................................................
RTC_DS1307 rtc;
//DS1307 rtc;
LiquidCrystal_I2C lcd(0x27, 2,  1, 0, 4, 5, 6, 7, 3, POSITIVE);//lcd setup
OneWire oneWire(11);
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer;

//...........................................................................

unsigned long ti; //segéd változo
unsigned long tii;
unsigned long k_ti=0; // sdv kapcsolo segéd az idözitéshez 
float tempC;
int x; //álltalános segéd változok
int y=20;
int older;
int timer;
int T;
int TT;
byte h;
int L=1;// led pwm
long LL; 
byte CX; //culsor X
byte CY; //culsor Y
byte ent=0; // enter állapot ha 0 akkor nincs ha 1 akkor van enter a mentésnél
byte user=0; // ha 0 akkor USER1 ha 1 akkor BBALA BALA1
int delai; // delay értek
byte del=2;// delay segéd vátozo 1  2 3 4 az értéke   kezdö 500 mili sec
byte blue=1;// kék funcioja a hosszu mod a normal =1
byte kapcsolo=0;
byte hosz; //ez a kék gomb utáni idözités másodpercben az eprombol 500-ik olvassa


int f; //álltalános for váltotozo
byte SDV1[5];
byte SDV2[5];


//----------------------------------------------------------------------------------------------- SETUP ---------------------------------------

void setup() {
delai=del*100;  //delay set up =50
//Serial.begin(9600);
  
//pin setup  
pinMode(0,INPUT); digitalWrite(0,1); //  kék
pinMode(1,INPUT); digitalWrite(1,1); //  sárga le nyil
pinMode(4,INPUT); digitalWrite(4,1); //  svd2
pinMode(5,INPUT); digitalWrite(5,1); //  sdv1
pinMode(6,INPUT); digitalWrite(6,1); //  sdv3
pinMode(7,INPUT); digitalWrite(7,1); //  piros
pinMode(8,INPUT); digitalWrite(8,1); //  fehér
pinMode(9,INPUT); digitalWrite(9,1); //  fekete
pinMode(10,INPUT); digitalWrite(10,1); //zöld
//pinMode(11,INPUT); digitalWrite(11,1); //
pinMode(13,OUTPUT);//LED
// analog A0=sárga jobbra
// analog A1=sárga fel                     ezek az analogok mert nemvolt több digital PIN
// analog A3=sárga balra
//...............................................................................................................


//tone(12,1000,100);
hz=normal; hossz=bipp; sound();

//Basic EEPROM setup
//EEPROM.update(00,56);EEPROM.update(01,);EEPROM.update(02,52);EEPROM.update(13,48); // 8260 FIGYELEM a NULLA eng=48 magy=96
//EEPROM.update(10,56);EEPROM.update(11,51);EEPROM.update(12,53);EEPROM.update(13,48); //8260 // 826
for (x=0;x<=3;x++){SDV1[x]=EEPROM.read(x);SDV2[x]=EEPROM.read(x+10);} //beolvassa az sdv-t 

//EEPROM.update(500,15); 
hosz=EEPROM.read(500);


//............................................................................................................   
//delay(10000); // biztonsági várakozás
Keyboard.begin();
lcd.begin (16,2);// LCD size setup end start
rtc.begin();

//...........................................................................................................
sensors.begin();
sensors.getAddress(insideThermometer,0);
sensors.setResolution(insideThermometer,12);
//...........................................................................................................
//rtc.adjust(DateTime(2019, 10, 02, 19, 00, 00)); //Idö SETUP xxxxxxxxxxxxxxxxx      IDÖ      xxxxxxxxxxxxxx

//...........................................................................................................
lcd.clear();
lcd.print("    SDV Data");
lcd.setCursor (0,1);lcd.print("2019.10.13. v1.2"); //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX  VERZIO XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
delay(1000);
//..........................................................................................................

lcd.clear();
for(x=1;x<30;x++){
  //lcd.setCursor (0,0);
  switch (random(4)){
  case 0:lcd.print("0");break;
  case 1:lcd.print("1");break;
  case 2:lcd.print(" ");break;
  case 3:lcd.print("  ");break;
  }
  lcd.scrollDisplayRight();
  delay(100);}

lcd.clear();
lcd.print(" Clifford James");
lcd.setCursor (0,1);

// kezdö menü érzékelés
x=1;
for (f=0;f<16;f++){
  lcd.print(".");
  if (digitalRead(7) ==0) {x=0;}// piros gomb megnyomva gotobig setup
  if (digitalRead(10) ==0) {user=1;} //zöld USER = BBALA BALA1
  if (digitalRead(0) ==0) {x=2;}// kék gomb megnyomva goto ora beállitás

  delay(100);}
if (x==2) tset(); 
if (x==0) set_up();
if (user==1){lcd.clear();lcd.print("Hello Kisgazdam!");delay(2000);}


//...........................................................................................................
lcd.clear();
PR();
termo(insideThermometer);
//goto teszt;
 }

 
//------------------------------------------------------------------------------------------ LOOP START---------------------------------

void loop() {
  //idözités a kék gomb után és hang
  
if(millis()>(k_ti+(hosz*1000))&&kapcsolo==1){
  kapcsolo=0;
  tone(12,1000,100);
  delay(150);
  tone(12,800,150);
  delay(200);
  tone(12,1000,100);
  lcd.clear();
  }  
  
T++;if(T>1000){T=0; PR();TT++;} // : pont villogtatás
if(TT==20){TT=0;termo(insideThermometer);} // hömérözés

input();
//Serial.println(analogRead(A0));

if(L==1) {LL++;if(LL>=2550){L=0;}} else {LL--;if(LL<=100){L=1;}} // LED PWM    matek
analogWrite(13, LL/10);

//...........RÖVID NYOMÁSOK.............................................................
switch (y) {
case 0:  if(blue==0){kek();} else {kekH();}    break; //kék contV itt furdul a  kék 
case 1:  le();     break; // sárga le
case 4:  sdv2();   break; //sdv2
case 5:  sdv1();   break; //sdv1
case 6:  flip();   break; //svd 3

case 7:  zoldH();  break; // ebben a verzioban megeggyezik a hosszu zöldel vagy csere
 //case 7:  piros();  break; //piros mentés kulzor mugatással ablka vátás NÉLKÜL

case 8:  feher();  break; //fehér
case 9:  fekete(); break; //fekete
case 10: zold();   break; //zöld
case 15: bal();    break;
case 16: job();    break;
case 17: fel();    break;
case 20:           break; // nemtörtént semmi!!! üres

//hosszu nyomások.......................................................
case 100: if(blue==0){kekH();}else{kek();}  break; // itt vátozik a kék funkcioja a seup tol föggöen 
case 101: leH();   break; // contZ
case 105: sdv1H(); break; //sdv1H setup sdv1
case 104: sdv2H(); break; //sdv2H setup svd2
case 106: flipH(); break; // flipp contA contC RET + ablakváltás
case 107: pirosH(); break; // login 
case 108: Dolder(); break; // older print
case 109:feketeH(); break; // sima contA contC return nélkül

case 110: piros(); break; //  ebben a verzioban megeggyezik a hosszu zöldel vagy csere
//case 110: zoldH(); break; // UK mentés kulzor mozgatással és ablak vátással 


case 150:  break;
default: break;  
}
  
//DateTime now = rtc.now();lcd.setCursor (0,0);lcd.print(now.hour(), DEC);  lcd.print(":");  lcd.print(now.minute(), DEC);  lcd.print(":"); lcd.print(now.second(), DEC);lcd.print("   ");lcd.setCursor (0,1);lcd.print(now.unixtime());delay(1000);

}
//------------------------------------------------------------------------------------------------- LOOP END-----------------------------------



//---------------------------------------------------------------------------------------------------- INPUT -----------------------------
void input(){
y=20;   
for (x=0;x<11;x++){
if(x==2){x=4;} //ez az I2C miatt kell mer foglalt a 2 3 pin 
if(digitalRead(x)==0){y=x;} //itt nyerei ki a h melyik gommb van lennyomva. y=gombPIN
}

//....................................................................................
ti=millis();
if(y>10) {goto mark2; y=100;}  // ha nincs nyomás akkor goto mark2
tone(12,4000,10);

//......................................................................................
mark1: 

if(digitalRead(y)==0 ){goto mark1;} // a gomb nyomva van és elindult a mérés goto mark1

tii=millis(); // lemérve

//........................................................................................
if(tii-ti<50) {y=20;} // tul rövid a nyomáés nincs esemény
if(tii-ti>700) {y=y+100;} // hosszu megnyomás történt

mark2:
delay(0); //ez a mark miatt kell!!!! ha nincs itt  nem forditja le rendesen

// analog input balra jobra fel gombok.......................................................
if(analogRead(A3)<700) {tone(12,4000,10);y=15;} //bal
if(analogRead(A0)<700) {tone(12,4000,10);y=16;} //jobb
if(analogRead(A1)<700) {tone(12,4000,10);y=17;} // fel
}

//---------------------------------------------------------------------------------------- SDV1 -------------------
void sdv1(){
  if(kapcsolo==0){
lcd.clear(); lcd.setCursor (2,0); lcd.print("Typed:");
lcd.setCursor (8,1);lcd.print("SDV"); for(x=0;x<=2;x++) {lcd.write(SDV1[x]);delay(100);}
  
Keyboard.print("SDV");
for(x=0;x<=2;x++) {Keyboard.write(SDV1[x]);}
older++;
tone(12,500,200); // rövid nyomás hang

delay(delai);
lcd.clear();}

else
{lcd.clear(); lcd.setCursor (2,0); lcd.print("Labels 1");kapcsolo=0;
Keyboard.print("1");
tone(12,500,200); // rövid nyomás hang

delay(delai);
lcd.clear();
  
  }

} 

//---------------------------------------------------------------------------------------- SDV1 H setup ---------------
void sdv1H(){
tone(12,1000,500);
CX=5; CY=0;lcd.setCursor (CX,CY); lcd.blink();

mark3:

if(analogRead(A3)<700){
 CX--;tone(12,300,100); delay(300);
 if(CX<3)CX=3;}
 
 if(analogRead(A0)<700){
 CX++;tone(12,300,100);delay(300); 
 if(CX>5)CX=5;}
 
 if(analogRead(A1)<700){
 SDV1[CX-3]++; tone(12,300,100);delay(300); 
 if(SDV1[CX-3]>57)SDV1[CX-3]=57;PR();}

 if(digitalRead(1)==0){
 SDV1[CX-3]--; tone(12,300,100);delay(300); 
 if(SDV1[CX-3]<48)SDV1[CX-3]=48;PR();}
 
 lcd.setCursor (CX,CY);
 
if(digitalRead(6)==1){goto mark3;} // hurok meg meg nem nyom

tone(12,800,1000); // hosszu nyomás hang
lcd.noBlink();
lcd.clear();
lcd.print("-- SAVE eRom --");
EEPROM.update(00,SDV1[0]);EEPROM.update(01,SDV1[1]);EEPROM.update(02,SDV1[2]);
delay(3000);
lcd.clear();
}


//--------------------------------------------------------------------------------------- SDV2 ---------------------
void sdv2(){
if(kapcsolo==0){  

lcd.clear(); lcd.setCursor (2,0); lcd.print("Typed:");
lcd.setCursor (8,1);lcd.print("SDV"); for(x=0;x<=2;x++) {lcd.write(SDV2[x]);delay(100);}

Keyboard.print("SDV");
for(x=0;x<=2;x++) {Keyboard.write(SDV2[x]);}
older++;
tone(12,500,200); // rövid nyomás hang
delay(delai);lcd.clear();}
else
{lcd.clear(); lcd.setCursor (2,0); lcd.print("Labels 2");kapcsolo=0;
Keyboard.print("2");
tone(12,600,200); // rövid nyomás hang

delay(delai);
lcd.clear();
}}


// -------------------------------------------------------------------------------------- SDV2H set up ---------------------
void sdv2H(){
tone(12,1000,500); // hosszu nyomás hang   
CX=5; CY=1;lcd.setCursor (CX,CY); lcd.blink();

mark4:

if(analogRead(A3)<700){
 CX--;tone(12,300,100); delay(300);
 if(CX<3)CX=3;}
 
 if(analogRead(A0)<700){
 CX++;tone(12,300,100);delay(300); 
 if(CX>5)CX=5;}
 
 if(analogRead(A1)<700){
 SDV2[CX-3]++; tone(12,300,100);delay(300); 
 if(SDV2[CX-3]>57)SDV2[CX-3]=57;PR();}

 if(digitalRead(1)==0){
 SDV2[CX-3]--; tone(12,300,100);delay(300); 
 if(SDV2[CX-3]<48)SDV2[CX-3]=48;PR();}
 
 lcd.setCursor (CX,CY);
 

 
if(digitalRead(6)==1){goto mark4;} // hurok meg meg nem nyom

tone(12,800,1000); // hosszu nyomás hang
lcd.noBlink();
lcd.clear();
lcd.print("-- SAVE eRom --");
EEPROM.update(10,SDV2[0]);EEPROM.update(11,SDV2[1]);EEPROM.update(12,SDV2[2]);
delay(3000);
lcd.clear();
}




//----------------------------------------------------------------------------------------------- FLIP cont A: cont C: RET:---------------------------
void flip(){
if(kapcsolo==0){
lcd.clear(); lcd.setCursor (0,0); lcd.print("Copy SDV");
lcd.setCursor (8,1); lcd.print("& :ENTER");  
Keyboard.press(128); //cont
Keyboard.press('a');
delay(100);
Keyboard.press('c');
delay(100);Keyboard.releaseAll();
Keyboard.press(ret);Keyboard.releaseAll(); //return
tone(12,500,200); // rövid nyomás hang
delay(delai);
lcd.clear();}
else
{lcd.clear(); lcd.setCursor (2,0); lcd.print("Labels 3");kapcsolo=0;
Keyboard.print("3");
tone(12,500,200); // rövid nyomás hang

delay(delai);
lcd.clear();}

}

//------------------------------------------------------------------------------------------------ FLIP H cont A: cont C: RET: ALT: TAB:----------------------
void flipH(){

lcd.clear(); lcd.setCursor (0,0); lcd.print("Copy SDV");
lcd.setCursor (2,1); lcd.print("& :ENTER :Flip"); 
Keyboard.press(cont);Keyboard.press('a');delay(100);Keyboard.press('c');
delay(100);Keyboard.releaseAll();
Keyboard.press(ret);delay(100);Keyboard.releaseAll();tone(12,400,200); delay(300);
Keyboard.press(alt);delay(100);Keyboard.press(TAB);delay(50);Keyboard.release(TAB);delay(100);Keyboard.releaseAll();
tone(12,500,200); // rövid nyomás hang 
delay(delai);
lcd.clear();
}

//----------------------------------------------------------------------------------------------- ZÖLD fel: jobra: ------------------
void zold(){ 
kapcsolo=1; // lelzi hogy az svd gombok megváltoznak 
k_ti=millis();
lcd.clear(); lcd.setCursor (2,0); lcd.print("Move Cursor"); 
lcd.setCursor (3,1); lcd.print(":Up :Right "); 
Keyboard.press(upp);
delay(100);
Keyboard.releaseAll();
tone(12,500,20); // rövid nyomás hang
delay(200);
Keyboard.press(right);
delay(100);
Keyboard.releaseAll();
tone(12,500,200); // rövid nyomás hang
delay(delai);lcd.clear();
}

//---------------------------------------------------------------------------------------- ZÖLD H le: balra: balra: conts: ret: ALT: TAB:------------------
void zoldH(){
  kapcsolo=0;
lcd.clear(); lcd.setCursor (1,0); lcd.print("Save UK. Data");
lcd.setCursor (0,1); lcd.print(":Save :Ent :Flip");  
Keyboard.press(down);delay(50);Keyboard.releaseAll();tone(12,500,20); delay(200);
Keyboard.press(left);delay(50);Keyboard.releaseAll();tone(12,500,20); delay(200);
Keyboard.press(left);delay(50);Keyboard.releaseAll();tone(12,500,20); delay(200);
Keyboard.press(cont);Keyboard.press('s');delay(1000);Keyboard.releaseAll();tone(12,500,200); 
if(ent==1){Keyboard.press(ret);delay(100);Keyboard.releaseAll();tone(12,300,200);delay(300);} //ha van enter

delay(100);
Keyboard.press(alt);
delay(100);
Keyboard.press(TAB);
delay(50);
Keyboard.release(TAB);
delay(500);
Keyboard.releaseAll();
tone(12,1000,500);
delay(3000);
lcd.clear();
}

//----------------------------------------------------------------------------------------------- FEL fel: ---------------------------------
void fel(){
lcd.clear(); lcd.setCursor (2,0); lcd.print("Move Cursor"); 
lcd.setCursor (7,1); lcd.print(":Up"); 
Keyboard.press(upp);
delay(100);
Keyboard.releaseAll();
tone(12,500,200); // rövid nyomás hang
delay(500);lcd.clear();
}


//--------------------------------------------------------------------------------------- KÉK cont V: -----------------------------------------------
void kek(){
lcd.clear(); lcd.setCursor (2,0); lcd.print("PASTE"); 
lcd.setCursor (7,1); lcd.print("SDV Data");  
Keyboard.press(cont); //cont bill
Keyboard.press('v');
delay(100);
Keyboard.releaseAll();
tone(12,500,200); // rövid nyomás hang
delay(delai);lcd.clear();
}

//-------------------------------------------------------------------------------------- Kék H ---------------------
void kekH(){
kapcsolo=1; // lelzi hogy az svd gombok megváltoznak 
k_ti=millis();
lcd.clear(); lcd.setCursor (0,0); lcd.print("Full");
lcd.setCursor (2,1); lcd.print("& :ENTER :Flip"); 
Keyboard.press(cont);Keyboard.press('a');delay(100);Keyboard.press('c');
delay(100);Keyboard.releaseAll();
Keyboard.press(ret);delay(100);Keyboard.releaseAll();tone(12,400,200); delay(300);
Keyboard.press(alt);delay(100);Keyboard.press(TAB);delay(50);Keyboard.release(TAB);delay(100);Keyboard.releaseAll();
delay(100);Keyboard.press(cont);Keyboard.press('v');delay(100);Keyboard.releaseAll();
Keyboard.press(right);delay(50);Keyboard.releaseAll();


tone(12,500,200); // rövid nyomás hang 
delay(delai);
lcd.clear();

 

tone(12,800,1000); // rövid nyomás hang

}

//------------------------------------------------------------------------------ PIROS  le: ball: ball: cont S: Return: --------------------
void piros(){
kapcsolo=0;
lcd.clear(); lcd.setCursor (1,0); lcd.print("Save UK. Data");
lcd.setCursor (3,1); lcd.print(":Save :Ent");  
Keyboard.press(down);delay(50);Keyboard.releaseAll();tone(12,500,20); delay(200);
Keyboard.press(left);delay(50);Keyboard.releaseAll();tone(12,500,20); delay(200);
Keyboard.press(left);delay(50);Keyboard.releaseAll();tone(12,500,20); delay(200);
Keyboard.press(cont);Keyboard.press('s');delay(1000);Keyboard.releaseAll();tone(12,500,200); 
if(ent==1){Keyboard.press(ret);delay(100);Keyboard.releaseAll();tone(12,600,500);delay(500);} // ha van entter

delay(3000);
lcd.clear();

  }

//------------------------------------------------------------------------------ PIROS H  "BBALA" TAB: BALA1: TAB: TAB: RET:----------
void pirosH(){
if(user==1){lcd.clear(); lcd.setCursor (5,0); lcd.print("BBALA");
lcd.setCursor (5,1); lcd.print("Login");
  
Keyboard.print("BBALA");
delay(100);
Keyboard.press(TAB);
delay(100);
Keyboard.releaseAll();
Keyboard.print("BALA1");
Keyboard.press(TAB);
delay(100);
Keyboard.releaseAll();
Keyboard.press(TAB);
delay(100);
Keyboard.releaseAll();
Keyboard.press(ret);
delay(100);
Keyboard.releaseAll();
tone(12,1000,500); // hosszu nyomás hang
delay(2000);lcd.clear();
}


if(user==0){lcd.clear(); lcd.setCursor (5,0); lcd.print("USER1");
lcd.setCursor (5,1); lcd.print("Login");
  
Keyboard.print("USER1");
delay(100);
Keyboard.press(TAB);
delay(100);
Keyboard.releaseAll();
Keyboard.print("Password1");
Keyboard.press(TAB);
delay(100);
Keyboard.releaseAll();
Keyboard.press(TAB);
delay(100);
Keyboard.releaseAll();
Keyboard.press(ret);
delay(100);
Keyboard.releaseAll();
tone(12,1000,500); // hosszu nyomás hang
delay(2000);lcd.clear();}

  

}

//----------------------------------------------------------------------------------------------- FEKETE  alt: TAB: ---------------
void fekete(){
 kapcsolo=0; 
lcd.clear(); lcd.setCursor (5,0); lcd.print("Flip");
lcd.setCursor (4,1); lcd.print("Windows");  
Keyboard.press(alt);
delay(100);
Keyboard.press(TAB);
delay(50);
Keyboard.release(TAB);
delay(200);
Keyboard.releaseAll();
tone(12,500,200); // rövid nyomás hang 
delay(delai);lcd.clear();
}

//------------------------------------------------------------------------------------------------- FEKETE H cont A: cont C: ------------------------
void feketeH(){
lcd.clear(); lcd.setCursor (3,0); lcd.print("Copy Data");
lcd.setCursor (3,1); lcd.print(":No Enter");  
Keyboard.press(cont); //cont
Keyboard.press('a');
delay(100);
Keyboard.press('c');
delay(100);Keyboard.releaseAll();
tone(12,500,200); // rövid nyomás hang
delay(delai);lcd.clear();
}


//---------------------------------------------------------------------------------------------------- FEHÉR balack x6:  ----------------
void feher(){
  kapcsolo=0;
lcd.clear(); lcd.setCursor (3,0); lcd.print("Blackspace");
 lcd.setCursor (3,1); lcd.print("Delete SDV"); 
for(f=1;f<=6;f++){
Keyboard.press(back);
delay(50);
Keyboard.releaseAll();
}

tone(12,500,200); // rövid nyomás hang  
delay(delai);
lcd.clear();
}

//----------------------------------------------------------------------------------------------------- LE nyil ----------
void le(){
lcd.clear(); lcd.setCursor (2,0); lcd.print("Move Cursor"); 
lcd.setCursor (6,1); lcd.print(":Down");  
Keyboard.press(down);
delay(50);
Keyboard.releaseAll();
tone(12,500,200); // rövid nyomás hang
delay(500);lcd.clear();
  }

//-------------------------------------------------------------------------------------------------------LE H--------------
void leH(){

lcd.clear(); lcd.setCursor (4,0); lcd.print("R E D O"); 
lcd.setCursor (0,1); lcd.print("XXXXXXXXXXXXXXXX");   
Keyboard.press(cont); //cont bill
delay(100);
Keyboard.press('z');
delay(100);
Keyboard.releaseAll();
tone(12,900,1000); // rövid nyomás hang  
delay(3000);lcd.clear();
}

//------------------------------------------------------------------------------------------------- Ball Nyil --------
void bal(){
lcd.clear(); lcd.setCursor (2,0); lcd.print("Move Cursor"); 
lcd.setCursor (5,1); lcd.print(":Left");  
Keyboard.press(left);
delay(50);
Keyboard.releaseAll();
tone(12,500,200); // rövid nyomás hang
delay(500);lcd.clear();
  }

//----------------------------------------------------------------------------------------------------Jobra Nyil ---------
void job(){
lcd.clear(); lcd.setCursor (2,0); lcd.print("Move Cursor"); 
lcd.setCursor (5,1); lcd.print(":Right");  
Keyboard.press(right);
delay(50);
Keyboard.releaseAll();
tone(12,500,200); // rövid nyomás hang
delay(500);lcd.clear();
  }

//--------------------------------------------------------------------------------------------------- SETUP ----------------------------
void set_up() {
lcd.clear();
lcd.print("Enter "); if(ent==0) lcd.print("No     "); else lcd.print("Yes   ");
tone(12,1000,500); 
CX=6; CY=0;lcd.setCursor (CX,CY);
mark5:
if(analogRead(A1)<700){ent=1; tone(12,300,100);delay(400);lcd.setCursor (CX,CY);lcd.print("Yes   ");}
if(digitalRead(1)==0){ent=0; tone(12,300,100);delay(400);lcd.setCursor (CX,CY);lcd.print("No     ");}
if(digitalRead(6)==1){goto mark5;} // hurok meg meg nem nyom
tone(12,800,1000); // hosszu nyomás hang
lcd.noBlink();
lcd.clear();
lcd.print("  --- SAVE ---");
delay(1500);
//------------------------------------------------------------------------------------------
lcd.clear();
lcd.print("USER "); if(user==0) lcd.print(" USER1  "); else lcd.print("BBALA");
tone(12,1000,500); 
CX=6; CY=0;lcd.setCursor (CX,CY);
mark15:
if(analogRead(A1)<700){user=0; tone(12,300,100);delay(400);lcd.setCursor (CX,CY);lcd.print("USER1   ");}
if(digitalRead(1)==0){user=1; tone(12,300,100);delay(400);lcd.setCursor (CX,CY);lcd.print("BBALA   ");}
if(digitalRead(6)==1){goto mark15;} // hurok meg meg nem nyom
tone(12,800,1000); // hosszu nyomás hang
lcd.noBlink();
lcd.clear();
lcd.print("  --- SAVE ---");
delay(1500);
lcd.clear();
//------------------------------------------------------------------------------------------
lcd.clear();
lcd.print("Delay "); lcd.print(delai);
tone(12,1000,500); 
CX=6; CY=0;lcd.setCursor (CX,CY);
mark25:
if(analogRead(A1)<700){del++;
if(del==11) del=10;
delai=100*del;
tone(12,300,100);delay(400);lcd.setCursor (CX,CY);lcd.print(delai);lcd.print("  ");}
if(digitalRead(1)==0){del--;
if(del==0) del=1;
delai=100*del;
tone(12,300,100);delay(400);lcd.setCursor (CX,CY);lcd.print(delai);lcd.print("  ");}
if(digitalRead(6)==1){goto mark25;} // hurok meg meg nem nyom
tone(12,800,1000); // hosszu nyomás hang
lcd.noBlink();
lcd.clear();
lcd.print("  --- SAVE ---");
delay(1500);
lcd.clear();
//---------------------------------------------------------
lcd.clear();
lcd.print("Blue ");
CX=6; CY=0;lcd.setCursor (CX,CY);
if(blue==0) lcd.print("Normal   "); else lcd.print("Advanced");
tone(12,1000,500); 
CX=6; CY=0;lcd.setCursor (CX,CY);
mark110:
if(analogRead(A1)<700){blue=1; tone(12,300,100);delay(400);lcd.setCursor (CX,CY);lcd.print("Advanced");}
if(digitalRead(1)==0){blue=0; tone(12,300,100);delay(400);lcd.setCursor (CX,CY);lcd.print("Normal   ");}
if(digitalRead(6)==1){goto mark110;} // hurok meg meg nem nyom
tone(12,800,1000); // hosszu nyomás hang
lcd.noBlink();
lcd.clear();
lcd.print("  --- SAVE ---");
delay(1500);
//---------------------------------------------------------
lcd.clear();
lcd.print("Bulu Time ");
CX=11; CY=0;lcd.setCursor (CX,CY);
lcd.print(hosz);
tone(12,1000,500); 
CX=11; CY=0;lcd.setCursor (CX,CY);
mark111:
if(analogRead(A1)<700){hosz++; tone(12,300,100);delay(400);lcd.setCursor (CX,CY);lcd.print(hosz);lcd.print("   ");}
if(digitalRead(1)==0){hosz--; tone(12,300,100);delay(400);lcd.setCursor (CX,CY);lcd.print(hosz);lcd.print("   ");}
if(digitalRead(6)==1){goto mark111;} // hurok meg meg nem nyom
tone(12,800,1000); // hosszu nyomás hang
lcd.noBlink();
lcd.clear();
lcd.print("-- SAVE eRom --");
delay(1500); 
EEPROM.update(500,hosz); 
  }


//-----------------------------------------------------------------------------------------------------  PRINT   ---------------------
void PR() {
if(kapcsolo==0){
lcd.setCursor (0,0);lcd.print("SDV");for(x=0;x<=2;x++) {
  if(SDV1[x]==96){lcd.write(48);} else {lcd.write(SDV1[x]);}}  // karakter csere mert a "0" más az lcd-n
lcd.setCursor(8,0);lcd.write(255);
lcd.setCursor (0,1);lcd.print("SDV");for(x=0;x<=2;x++) {
  if(SDV2[x]==96){lcd.write(48);} else {lcd.write(SDV2[x]);}}
lcd.setCursor(8,1);lcd.write(255);
DateTime now = rtc.now();
lcd.setCursor (10,0);
if(now.hour()<10) lcd.print("0");
lcd.print(now.hour());
if(timer==1) {lcd.print(":");timer=0; } else {lcd.print(" ");timer=1; }

if(now.minute()<10) lcd.print("0");
lcd.print(now.minute());lcd.print(" ");
lcd.setCursor (9,1);lcd.print(" ");lcd.print(tempC);lcd.print("C");


if(now.minute()==0 and now.second()<1){zene();}}
else
{
 lcd.setCursor (0,0);lcd.print("Labels Number"); 
}
  }


//---------------------------------------------------------------------------------------------------- TERMO -------------------------  
void termo(DeviceAddress deviceAddress)
{

  sensors.requestTemperatures();
  tempC = sensors.getTempC(deviceAddress);
  }

//---------------------------------------------------------------------------------------------------- ZENE ---------------------

 void zene() {
for (thisNote = 0; thisNote < 8; thisNote++) {
noteDuration = 1000 / noteDurations[thisNote];
tone(12, melody[thisNote], noteDuration);
pauseBetweenNotes = noteDuration * 1.30;
delay(pauseBetweenNotes);
noTone(12);}}

//------------------------------------------------------------------------------------------------- FEHÉR H --------------
void Dolder(){
lcd.clear();lcd.setCursor (0,0);lcd.print("BVG user ");
lcd.setCursor (5,1); lcd.print("Login");
tone(12,200,100);
delay(500);
lcd.setCursor (5,1); lcd.print("BVGSCANNER");
tone(12,600,300);
delay(1000);

  
Keyboard.print("BVGSCANNER");
tone(12,300,100);
delay(500);
tone(12,100,200);
Keyboard.releaseAll();
Keyboard.press(TAB);
Keyboard.releaseAll();
delay(100);
tone(12,1000,1000);
lcd.clear();lcd.setCursor (0,0);lcd.print("PASSWORD   ");
lcd.setCursor (1,1); lcd.print("Password123");
Keyboard.print("Password123");
delay(100);
Keyboard.releaseAll();
delay(100);
Keyboard.press(ret);
delay(100);
Keyboard.releaseAll();

delay(100);



tone(12,1000,500); // hosszu nyomás hang
delay(2000);lcd.clear();
tone(12,200,1000);
lcd.println("SkyNet ACTIVATED");
lcd.setCursor (0,1);
lcd.print("Countdown Starts");
  delay(5000);
  lcd.clear();
}

//------------------------------------------------------------------------------------------------ SOUND ------------------

void sound(){
if(hang==1){
  tone(12,hz,hossz);}  
}

void tset(){ //ora beállitás bocs de nem volt idöm a szofisztikáltságra :)
int ev;
byte ho;
byte nap;
int ora;
int perc;
int szek;

lcd.clear();// idö adatok beolvasása
DateTime now = rtc.now();
lcd.setCursor (0,0);
lcd.print(now.year());lcd.print(" ");ev=now.year();if(ev<2019) {ev=2019;}
lcd.print(now.month());lcd.print(" ");ho=now.month();
lcd.print(now.day());lcd.print(" "); nap=now.day();nap=now.day();
lcd.setCursor (0,1);lcd.print("SetUp Time ");
lcd.print(now.hour());lcd.print(":");lcd.print(now.minute());ora=now.hour();perc=now.minute();
szek=0; 

delay(2000);
//év set 
lcd.clear();
lcd.print("Year");
CX=6; CY=0;lcd.setCursor (CX,CY);
lcd.print(ev);
tone(12,1000,500); 
CX=6; CY=0;lcd.setCursor (CX,CY);
mark100:
if(analogRead(A1)<700){ev++; tone(12,300,100);delay(400);lcd.setCursor (CX,CY);lcd.print(ev);}
if(digitalRead(1)==0){ev--; tone(12,300,100);delay(400);lcd.setCursor (CX,CY);lcd.print(ev);}
if(digitalRead(6)==1){goto mark100;} // hurok meg meg nem nyom
tone(12,800,1000); // hosszu nyomás hang
lcd.noBlink();
lcd.clear();
lcd.print("  --- Next ---");
delay(1000);

//ho set 
lcd.clear();
lcd.print("Month ");
CX=6; CY=0;lcd.setCursor (CX,CY);
lcd.print(ho);
tone(12,1000,500); 
CX=6; CY=0;lcd.setCursor (CX,CY);
mark101:
if(analogRead(A1)<700){ho++; if(ho>=13) ho=12;tone(12,300,100);delay(400);lcd.setCursor (CX,CY);lcd.print(ho);lcd.print("  ");}
if(digitalRead(1)==0){ho--; if(ho<=0) ho=1;tone(12,300,100);delay(400);lcd.setCursor (CX,CY);lcd.print(ho);lcd.print("  ");}
if(digitalRead(6)==1){goto mark101;} // hurok meg meg nem nyom
tone(12,800,1000); // hosszu nyomás hang
lcd.noBlink();
lcd.clear();
lcd.print("  --- Next ---");
delay(1000);
 
 //nap set 
lcd.clear();
lcd.print("Day ");
CX=6; CY=0;lcd.setCursor (CX,CY);
lcd.print(nap);
tone(12,1000,500); 
CX=6; CY=0;lcd.setCursor (CX,CY);
mark102:
if(analogRead(A1)<700){nap++; if(nap>=32) nap=31;tone(12,300,100);delay(400);lcd.setCursor (CX,CY);lcd.print(nap);lcd.print("  ");}
if(digitalRead(1)==0){nap--; if(nap<=0) nap=1;tone(12,300,100);delay(400);lcd.setCursor (CX,CY);lcd.print(nap);lcd.print("  ");}
if(digitalRead(6)==1){goto mark102;} // hurok meg meg nem nyom
tone(12,800,1000); // hosszu nyomás hang
lcd.noBlink();
lcd.clear();
lcd.print("  --- Next ---");
delay(1000); 

 //ora set 
lcd.clear();
lcd.print("Hour ");
CX=6; CY=0;lcd.setCursor (CX,CY);
lcd.print(ora);
tone(12,1000,500); 
CX=6; CY=0;lcd.setCursor (CX,CY);
mark103:
if(analogRead(A1)<700){ora++; if(ora>=24) ora=23;tone(12,300,100);delay(400);lcd.setCursor (CX,CY);lcd.print(ora);lcd.print("  ");}
if(digitalRead(1)==0){ora--; if(ora<=-1) ora=0;tone(12,300,100);delay(400);lcd.setCursor (CX,CY);lcd.print(ora);lcd.print("  ");}
if(digitalRead(6)==1){goto mark103;} // hurok meg meg nem nyom
tone(12,800,1000); // hosszu nyomás hang
lcd.noBlink();
lcd.clear();
lcd.print("  --- Next ---");
delay(1000);

 //perc set 
lcd.clear();
lcd.print("Minute ");
CX=7; CY=0;lcd.setCursor (CX,CY);
lcd.print(perc);
tone(12,1000,500); 
CX=7; CY=0;lcd.setCursor (CX,CY);
mark104:
if(analogRead(A1)<700){perc++; if(perc>=60) perc=59;tone(12,300,100);delay(400);lcd.setCursor (CX,CY);lcd.print(perc);lcd.print("  ");}
if(digitalRead(1)==0){perc--; if(perc<=-1) perc=0;tone(12,300,100);delay(400);lcd.setCursor (CX,CY);lcd.print(perc);lcd.print("  ");}
if(digitalRead(6)==1){goto mark104;} // hurok meg meg nem nyom
tone(12,800,1000); // hosszu nyomás hang
lcd.noBlink();
lcd.clear();
lcd.print("  --- Next ---");
delay(1000);

 //másosperc "szek" set 
lcd.clear();
lcd.print("Second ");
CX=7; CY=0;lcd.setCursor (CX,CY);
lcd.print(szek);
tone(12,1000,500); 
CX=7; CY=0;lcd.setCursor (CX,CY);
mark105:
if(analogRead(A1)<700){szek++; if(szek>=60) szek=59;tone(12,300,100);delay(400);lcd.setCursor (CX,CY);lcd.print(szek);lcd.print("  ");}
if(digitalRead(1)==0){szek--; if(szek<=-1) szek=0;tone(12,300,100);delay(400);lcd.setCursor (CX,CY);lcd.print(szek);lcd.print("  ");}
if(digitalRead(6)==1){goto mark105;} // hurok meg meg nem nyom
tone(12,800,1000); // hosszu nyomás hang
lcd.noBlink();
lcd.clear();
lcd.print("  --- SAVE ---");

rtc.adjust(DateTime(ev, nap, ho, ora, perc, szek));
delay(1500);
  
  

}
