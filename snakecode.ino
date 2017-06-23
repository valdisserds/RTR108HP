int PIN_SCE = 6;        // SS
int PIN_RESET = 7;
int PIN_DC = 5;         // data
int PIN_SDIN = 4;       // MOSI SIMO
int PIN_SCLK = 3;       // CLK SCLK
int PIN_BL = 9;         // BL LED
int PIN_VCC = 2;        // Vcc +-

#define BUTTON_D 8            // pinu pielāgošana pogām
#define BUTTON_U 9
#define BUTTON_L 10
#define BUTTON_R 11
#define LCD_C LOW             // command
#define LCD_D HIGH            // data high command low.
#define LCD_X 84              // /character area //
#define LCD_Y 48              // consists of banks of 7 by eight pixels.//
#define LCD_DATA 1

void posmarker(){
  LcdWrite(0, 0x80 | 0);
  LcdWrite(0, 0x40 | 0);
}

void LcdClear(void){
  for (int index = 0; index < 504; index++) LcdWrite(LCD_D, 0x00);
}

void LcdWrite(byte dc, byte data){
  digitalWrite(PIN_DC, dc);
  digitalWrite(PIN_SCE, LOW);
  shiftOut(PIN_SDIN, PIN_SCLK, MSBFIRST, data);
  digitalWrite(PIN_SCE, HIGH);
}

void initialise(){
  pinMode(PIN_VCC, OUTPUT);
  pinMode(PIN_SCE, OUTPUT);
  pinMode(PIN_RESET, OUTPUT);
  pinMode(PIN_DC, OUTPUT);
  pinMode(PIN_SDIN, OUTPUT);
  pinMode(PIN_SCLK, OUTPUT);
  pinMode(PIN_BL, OUTPUT);
  digitalWrite(PIN_VCC, HIGH);
  digitalWrite(PIN_RESET, LOW);
  digitalWrite(PIN_RESET, HIGH);
  LcdWrite(0, 0x21);      // Tell LCD that extended commands follow
  LcdWrite(0, 0xB1);      // Set LCD Vop (Contrast): Try 0xB1(good @3.3V) or0xBF if your display is too dark 
  LcdWrite(0, 0x04);      // Set Temp coefficent
  LcdWrite(0, 0x13);      // LCD bias mode 1:48: Try 0x13 or 0x14
  LcdWrite(0, 0x20);      // We must send 0x20 before modifying the display control mode 
  LcdWrite(0, 0x0C);      // Set display control, normal mode. 0x0D for inverse.
}

int lenght, i, k, dtime;
char d, foodx, foody, nx, ny, snake_x[200], snake_y[200], tempx, tempy; //maksimālais čūskas garums ir 200
byte map_array[42][24]; // karte(masīvs), kur pārvietojas čūsku

// čūskas izveide - čūskas garums; virziens, kurā sākumā pārvietojas; divi masīvi, kas uzglabā x un y koordinātes
void create_snake(){
  d = 'R';
  lenght = 8;
  for(i = 0; i < lenght; i++){
    snake_x[i] = lenght - (i+1);
    snake_y[i] = 0;
  }
}

// ēdiena izveide - x un y koordinātes vienai vienībai
void create_food(){
  foodx = random(42);
  foody = random(24);
}

// funkcija, kas pārbauda, vai čūsku saduras ar sevi, un izvada attiecīgi - true vai false
bool check_collision(char x_, char y_, char arrx[], char arry[]) {
  for(i = 0; i < lenght; i++){
    if(arrx[i] == x_ && arry[i] == y_) return true;
  }
  return false;
}

void setup(){
  initialise();
  LcdClear();
  posmarker();
  pinMode(BUTTON_L, INPUT);
  pinMode(BUTTON_R, INPUT);
  pinMode(BUTTON_U, INPUT);
  pinMode(BUTTON_D, INPUT);
  digitalWrite(BUTTON_L, HIGH);
  digitalWrite(BUTTON_R, HIGH);
  digitalWrite(BUTTON_U, HIGH);
  digitalWrite(BUTTON_D, HIGH);
  create_snake();
  create_food();
}

void loop() {
  dtime = 200;
  for(i = 0;i < 42; i++){
	for(k = 0; k < 24; k++) map_array[i][k] = 0;
  } // aizpilda kartes masīvu ar 0 vērtību
  
  map_array[foodx][foody] = 1; // kartes masīva elementu (adrese ir ēdiena koordinātes) pielīdzina 1
  
  if(!digitalRead(BUTTON_R) && d != 'R') d = 'L'; // čūskas kontroles
  else if(!digitalRead(BUTTON_U) && d != 'D') d = 'U';
  else if(!digitalRead(BUTTON_L) && d != 'L') d = 'R';
  else if(!digitalRead(BUTTON_D) && d != 'U') d = 'D';	
  
  nx = snake_x[0]; // x un y koordināšu masīvu apstrāde - princips vienkāršs - lai čūsku pārvietotos, astes pēdējais
  ny = snake_y[0]; // gabals (koordinātes) cikliski tiek pielikts priekšā galvai
  
  if(d == 'R') nx++;
  else if(d == 'L') nx--;
  else if(d == 'U') ny--;
  else if(d == 'D') ny++;
  
  // pārbauda, vai ir sadursme ar sienām vai ķermeni, ja ir, tad uzsāk spēli no jauna
  if(nx == -1 || nx == 42 || ny == -1 || ny == 24 || check_collision(nx, ny, snake_x, snake_y)) { 
        create_snake();
        create_food();
        return;
  }

  if(nx == foodx && ny == foody){ //ja gadījumā, čūska appēd ēdienu, tad tā vietā, lai pārliktu asti, ēdiens kļūst par jauno čūskas galvu
	snake_x[lenght] = nx;
	snake_y[lenght] = ny;
	lenght++;      // pagarina čūsku
	create_food(); //izveido jaunu ēdienu
	dtime = 0;
  }
  else {
	snake_x[lenght-1] = nx;
        snake_y[lenght-1] = ny;
  }
  
  tempx = snake_x[lenght-1];
  tempy = snake_y[lenght-1]; 
  for(i = lenght-1; i > 0; i--){
	snake_x[i] = snake_x[i-1];
	snake_y[i] = snake_y[i-1];
  }
  snake_x[0] = tempx;
  snake_y[0] = tempy; // x,y masīva apstrādes beigas
  
  for(i = 0; i < lenght; i++) map_array[snake_x[i]][snake_y[i]] = 1; // kartes masīvā 1 vērtība tiek piešķirta adresēm ar čūskas koordinātēm
  
  unsigned char hexc, a, b, f, g, c = 0;  // kartes masīvs tiek interpretēts, lai to varētu attēlot uz LCD ekrāna 
  while(c < 24){
    for(i = 0; i < 42; i++){
      g = map_array[i][0+c]; 
      f = map_array[i][1+c]; 
      b = map_array[i][2+c]; 
      a = map_array[i][3+c];
      hexc = a * 192 + b * 48 + f * 12 + g * 3;
      LcdWrite(1, hexc);
      LcdWrite(1, hexc);
    }
    c = c + 4;
  }
  delay(dtime); // kustības ātrums: jo lielāks dtime, jo lēnāk kustas čūska
  posmarker(); // uzstāda attēlošanas koordinātes uz 0:0
}
