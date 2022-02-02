#include <Adafruit_NeoPixel.h>

#define PIN      2
#define N_LEDS 144

Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, PIN, NEO_GRB + NEO_KHZ800);

//PIXEL_MAP // ROW
//                5  |05|  |11|  |17|  |23|  |29|  |35|  |41|   #   |83|  |77|  |71|  |65|  |59|  |53|  |47|
//                4  |04|  |10|  |16|  |22|  |28|  |34|  |40|   #   |82|  |76|  |70|  |64|  |58|  |52|  |46| 
//                3  |03|  |09|  |15|  |21|  |27|  |33|  |39|   #   |81|  |75|  |69|  |63|  |57|  |51|  |45|
//                2  |02|  |08|  |14|  |20|  |26|  |32|  |38|   #   |80|  |74|  |68|  |62|  |56|  |50|  |44|
//                1  |01|  |07|  |13|  |19|  |25|  |31|  |37|   #   |79|  |73|  |67|  |61|  |55|  |49|  |43|
//                0  |00|  |06|  |12|  |18|  |24|  |30|  |36|   #   |78|  |72|  |66|  |60|  |54|  |48|  |42|
//COLUMN              0     1     2     3     4     5     6           0     1     2     3     4     5     6

int matrix[14][6];  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - matrix[column][row]
int player=1;   //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - number 1: red leds - - - number 2: yellow leds
int num_colum;
uint8_t brightless=200;
long long frame;
int animation_delay=30;
int animation_length=0;
long long animation_count=0;
int led1=0,led2=0,led3=0,led4=0;
boolean animation_done=false;
boolean stacked=true;
boolean button_pressed=false;
boolean initialized=false;
boolean win1=false,win2=false;
int r,g,b;

//########################################################### SET_ UP ##################################################################################################
void setup()
{
  strip.begin();
  strip.clear();
  Serial.begin(9600);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
}

//######################################################## LOOP #########################################################################################################
void loop()
{
  frame=millis();
  
  if(!initialized)
  {
      initializing_leds();
  }
  
  pushing_button();
  
  piece_animation(num_colum, frame);
  
  stacking_pieces(num_colum, player);

  check_win();
}

//################################################### TURN ON ALL LEDS #####################################################################################################
void turn_on_all_leds()
{
  for(int i=0;i<84;i++)
  {
    strip.setBrightness(brightless);
    strip.setPixelColor(i,200,30,30);
    strip.show();
  }
}

//################################################### SUPERIOR_LED #########################################################################################################
//it returns the numbre of most superior burning led of column
int superior_led(int column)
{
  int numero_pixel;
  numero_pixel = (((column + 1) * 6) - 1);
  return numero_pixel;
}


//##################################################### PIECE_ANIMATION ##############################################################################################
//It mades the piece animation
void piece_animation(int col, long long frame)
{
  long long subtraction = frame - animation_count;

  //if 
  if( subtraction > animation_delay &&     //if it pass the time of animation delay
      number_pieces(col) < 6             &&     //if number of pieces in column is less than 6 (6 is full of column)
      button_pressed                &&     //if the button is pressed
      total_of_pieces() < 84                  //if not full leds are switched on
    )
  {
    if(button_pressed)
    {
    animation_count=frame;                 //update the account
    strip.setBrightness(brightless);
    strip.setPixelColor(superior_led(col)-animation_length,r,g,b);
    strip.setPixelColor(superior_led(col)-animation_length+42,r,g,b);
    //Serial.print("turn on: ");
    //Serial.println((superior_led(col)-animation_length));
    strip.show();
    if( animation_length!=0 &&
        animation_length<6-number_pieces(col) //if itsn't the last or first positions of column
      )
    {
      strip.setPixelColor((superior_led(col)+1)-animation_length,0);
      strip.setPixelColor((superior_led(col)+1)-animation_length+42,0);
      //Serial.print("apago ");
      //Serial.println((superior_led(col)+1)-animation_length);
      strip.show();
    }
    animation_length++;
    if(animation_length==6-number_pieces(col)) //if it's the last position of column
    {
      animation_done=true;
      animation_length=0;
    }
    }
   }else
   if(total_of_pieces()==42)
   {
    delay(2000);
    animation_done=true;
    initialized=false;
   }
   if(number_pieces(col)==6)
   {
    animation_done=true;
   }
   
}
//############################################################ TOTAL_OF_PIECES ######################################################################################
//it returns the total of pieces on the game
int total_of_pieces()
{
  int total=0;
  for(int c=0;c<7;c++)
  {
    total += number_pieces(c);
  }
  return total;
}
//############################################################ NEXT_SPACE #####################################################################################
//its return the led number where the piece will be stay in the column
int next_space(int column)
{
  int number_space = 0;

  while (matrix[column][number_space] != 0)
  {
    number_space++;
  }

  return number_space;
}

//########################################################### NUMBER_PIECES ###############################################################################################
//it returns the number of pieces in a column
int number_pieces(int column)
{
  int number_pieces = 0;

  for (int i = 0; i < 6; i++)
  {
    if (matrix[column][i] != 0)
    {
      number_pieces++;
    }
  }
  return number_pieces;
}

//############################################################ COLOR #################################################################################################
//it changes the color depending on the player's number
void color(int &r, int &g, int &b)
{
  if (player == 2)
  {
    r = 255;
    g = 255;
    b = 0;
  }else
  {
    r = 255;
    g = 0;
    b = 30;
  }
}

//############################################################## STACKING PIECES ####################################################################################
//it stacks the piece in the matrix, putting on a number (1,2) according to the player
void stacking_pieces(int col, int player)
{
    if(animation_done)
    {
      animation_done=false;
      if (number_pieces(col) == 6)
      {
    
      } 
      else
      {
        if (number_pieces(col) == 0)  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - if there aren't pieces in column
        {
          if(player==1)
          {
            matrix[col][0] = 1;   // - - - - - - - - - - - - - - - - - - - - - - - - - - - put on the piece in the first column (matrix)
            matrix[col][42] = 1;       
          }else
          {
            matrix[col][0] = 2;
            matrix[col][42]= 2; 
          }
          //monitor_matrix();
        }
        else   //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - if there are pieces in the column
        {
          if(player==1)
          {
             matrix[col][next_space(col)] = 1; // - - - - - - - - - - - - - - - - - - put on the piece in the next space (matrix)
             matrix[col][next_space(col)+41] = 1;
          }
          else
          {
            matrix[col][next_space(col)] = 2; //- - - - - - - - - - - - - - - - - - - put on the piece in the next space (matrix)
            matrix[col][next_space(col)+41] = 2;
          }
          //monitor_matrix();
        }
      }
      stacked=true;
      button_pressed=false;
    }
    
}

//############################################################### MONITOR_MATRIX ################################################################################### 
//it shows on the Serial Monitor the matrix that represents the game
void monitor_matrix()
{
  for (int r = 5; r >=0; r--)
  {
    for (int c = 0; c < 14; c++)
    {
      Serial.print(" |");
      if(matrix[c][r]==1)
      {
        Serial.print("X");
      }else
      if(matrix[c][r]==2)
      {
        Serial.print("O");
      }else
      {
        Serial.print(" ");
      }
      Serial.print("| ");

    }
    Serial.println("");
  }
  Serial.println("-_________________________________-__________________________________-");
}

//################################################################# PUSHING_BUTTON ###################################################################################
void pushing_button()
{
  int sensor3 = digitalRead(3); //column 1
  int sensor4 = digitalRead(4); //column 2
  int sensor5 = digitalRead(5); //column 3
  int sensor6 = digitalRead(6); //column 4
  int sensor7 = digitalRead(7); //column 5
  int sensor8 = digitalRead(8); //column 6
  int sensor9 = digitalRead(9); //column 7
 if(stacked)
 {
  if(sensor3==0)
    {
    if(number_pieces(0)<6)
    {
    //Serial.println("Boton 1");
    num_colum=0;
    switch_player();
    }
    button_pressed=true;
    stacked=false;
    }
    
    if(sensor4==0)
    {
    if(number_pieces(1)<6)
    {
    //Serial.println("Boton 2");
    num_colum=1;
    switch_player();
    }
    button_pressed=true;
    stacked=false;
    }
    
    if(sensor5==0)
    {
    if(number_pieces(2)<6)
    {
    //Serial.println("Boton 3");
    num_colum=2;
    switch_player();
    }
    button_pressed=true;
    stacked=false;
    }
   
    if(sensor6==0)
     {
     if(number_pieces(3)<6)
    {
     //Serial.println("Boton 4");
     num_colum=3;
     switch_player();
    }
     button_pressed=true;
     stacked=false;
     }
    if(sensor7==0)
    {
    if(number_pieces(4)<6)
    {
    //Serial.println("Boton 5");
    num_colum=4;
    switch_player();
    }
    button_pressed=true;
    stacked=false;
    }
    
    if(sensor8==0)
    {
    if(number_pieces(5)<6)
    {
    //Serial.println("Boton 6");
    num_colum=5;
    switch_player();
    }
    button_pressed=true;
    stacked=false;
    }
    
    if(sensor9==0)
    {
    if(number_pieces(6)<6)
    {
    //Serial.println("Boton 7");
    num_colum=6;
    switch_player();
    }
    button_pressed=true;
    stacked=false;
   }
  }
  color(r,g,b);
}

//################################################################ NUM_LED ############################################################################################
//it returns the number of led in a column and a row
int num_led(int colum, int row)
{
  int num_led;

  num_led = colum*6 + row;
  
  return num_led;
}

//################################################################ FINAL ANIMATION ####################################################################################
void final_animation()
{
  color(r,g,b);
  for(int i=0;i<10;i++)
  {
    strip.setPixelColor(led1,r,g,b);
    strip.setPixelColor(led2,r,g,b);
    strip.setPixelColor(led3,r,g,b);
    strip.setPixelColor(led4,r,g,b);
    strip.setPixelColor(led1+42,r,g,b);
    strip.setPixelColor(led2+42,r,g,b);
    strip.setPixelColor(led3+42,r,g,b);
    strip.setPixelColor(led4+42,r,g,b);
    Serial.println("led1: ");
    Serial.print(led1);
    Serial.println("");
    Serial.println("led2: ");
    Serial.print(led2);
    Serial.println("");
    Serial.println("led3: ");
    Serial.print(led3);
    Serial.println("");
    Serial.println("led4: ");
    Serial.print(led4);
    Serial.println("");
    strip.show();
    delay(200);

    strip.setPixelColor(led1,0);
    strip.setPixelColor(led2,0);
    strip.setPixelColor(led3,0);
    strip.setPixelColor(led4,0);
    strip.setPixelColor(led1+42,0);
    strip.setPixelColor(led2+42,0);
    strip.setPixelColor(led3+42,0);
    strip.setPixelColor(led4+42,0);
    
    strip.show();

    delay(500);
    
  }
  initialized=false;
}
//################################################################ WIN_CONDITION ###################################################################################
void check_win()
{
  if(!win1&&!win2)
  {
  check_rows();
  check_columns();
  check_right();
  check_left();
  }else
  if(win1)
  {
    player=1;
    Serial.println("WIN1");
    final_animation();
  }else
  if(win2)
  {
    player=2;
    final_animation();
  }
}

//##################################################################### CHECK_COLUMNS #################################################################################
//it checks the columns
void check_columns()
{
  for(int c=0;c<7;c++)
  {
    for(int f=0;f<3;f++)
    {
      if(matrix[c][f]==1&&matrix[c][f+1]==1&&matrix[c][f+2]==1&&matrix[c][f+3]==1)
      {
        led1=num_led(c,f);
        led2=num_led(c,f+1);
        led3=num_led(c,f+2);
        led4=num_led(c,f+3);
        win1=true;
        break;
      }else
      if(matrix[c][f]==2&&matrix[c][f+1]==2&&matrix[c][f+2]==2&&matrix[c][f+3]==2)
      {
        led1=num_led(c,f);
        led2=num_led(c,f+1);
        led3=num_led(c,f+2);
        led4=num_led(c,f+3);
        win2=true;
        break;
      }
    }
  }
}

//##################################################################### CHECK_ROWS #####################################################################################
//it checks the rows
void check_rows()
{
  for(int f=0;f<5;f++)
  {
    for(int c=0;c<4;c++)
    {
      if(matrix[c][f]==1&&matrix[c+1][f]==1&&matrix[c+2][f]==1&&matrix[c+3][f]==1)
      {
        led1=num_led(c,f);
        led2=num_led(c+1,f);
        led3=num_led(c+2,f);
        led4=num_led(c+3,f);
        win1=true;
        break;
      }else
      if(matrix[c][f]==2&&matrix[c+1][f]==2&&matrix[c+2][f]==2&&matrix[c+3][f]==2)
      {
        led1=num_led(c,f);
        led2=num_led(c+1,f);
        led3=num_led(c+2,f);
        led4=num_led(c+3,f);
        win2=true;
        break;
      }
    }
  }
}

//##################################################################### CHECK_RIGHT ####################################################################################
//it checks the diagonal to right
void check_right()
{
    for(int c=0;c<4;c++)
  {
    for(int f=0;f<3;f++)
    {
      if(matrix[c][f]==1&&matrix[c+1][f+1]==1&&matrix[c+2][f+2]==1&&matrix[c+3][f+3]==1)
      {
        led1=num_led(c,f);
        led2=num_led(c+1,f+1);
        led3=num_led(c+2,f+2);
        led4=num_led(c+3,f+3);
        win1=true;
        break;
      }else
      if(matrix[c][f]==2&&matrix[c+1][f+1]==2&&matrix[c+2][f+2]==2&&matrix[c+3][f+3]==2)
      {
        led1=num_led(c,f);
        led2=num_led(c+1,f+1);
        led3=num_led(c+2,f+2);
        led4=num_led(c+3,f+3);
        win2=true;
        break;
      }
    }
  }
}

//##################################################################### CHECK_LEFT #####################################################################################
//it checks the diagonal to left
void check_left()
{
  for(int c=0;c<4;c++)
  {
    for(int f=5;f>2;f--)
    {
      if(matrix[c][f]==1&&matrix[c+1][f-1]==1&&matrix[c+2][f-2]==1&&matrix[c+3][f-3]==1)
      {
        led1=num_led(c,f);
        led2=num_led(c+1,f-1);
        led3=num_led(c+2,f-2);
        led4=num_led(c+3,f-3);
        win1=true;
        break;
      }else
      if(matrix[c][f]==2&&matrix[c+1][f-1]==2&&matrix[c+2][f-2]==2&&matrix[c+3][f-3]==2)
      {
        led1=num_led(c,f);
        led2=num_led(c+1,f-1);
        led3=num_led(c+2,f-2);
        led4=num_led(c+3,f-3);
        win2=true;
        break;
      }
    }
  }
}

//##################################################################### SWITCH_PLAYER ###################################################################################
void switch_player()
{
        if(player==1)
        {
          player=2;
          digitalWrite(11,HIGH);  //it changes the ground with transistors
          digitalWrite(10,LOW);   //it changes the ground with transistors
        }else
        {
          player=1;
          digitalWrite(11,LOW);
          digitalWrite(10,HIGH);
        }
}

//################################################################## INITIALIZING LEDS #################################################################################
//it initializes all the game with the defautl parameters
void initializing_leds()
{
  for(int i=0;i<84;i++)
  {
    strip.setPixelColor(i,0);
    strip.show();
  }
  win1=false;
  win2=false;
  strip.clear();
  for(int c=0;c<14;c++)
  {
    for(int f=0;f<6;f++)
    {
      matrix[c][f]=0;
    }
  }
  monitor_matrix();
  digitalWrite(11,HIGH);
  digitalWrite(10,LOW);
  player=2;
  initialized=true;
}
