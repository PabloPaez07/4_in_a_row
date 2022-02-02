#include <Adafruit_NeoPixel.h>

#define PIN      2
#define N_LEDS 144

Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, PIN, NEO_GRB + NEO_KHZ800);

//PIXEL_MAP //FILA
//                5  |05|  |11|  |17|  |23|  |29|  |35|  |41|   #   |83|  |77|  |71|  |65|  |59|  |53|  |47|
//                4  |04|  |10|  |16|  |22|  |28|  |34|  |40|   #   |82|  |76|  |70|  |64|  |58|  |52|  |46| 
//                3  |03|  |09|  |15|  |21|  |27|  |33|  |39|   #   |81|  |75|  |69|  |63|  |57|  |51|  |45|
//                2  |02|  |08|  |14|  |20|  |26|  |32|  |38|   #   |80|  |74|  |68|  |62|  |56|  |50|  |44|
//                1  |01|  |07|  |13|  |19|  |25|  |31|  |37|   #   |79|  |73|  |67|  |61|  |55|  |49|  |43|
//                0  |00|  |06|  |12|  |18|  |24|  |30|  |36|   #   |78|  |72|  |66|  |60|  |54|  |48|  |42|
//COLUMNA             0     1     2     3     4     5     6

int matriz[14][6];  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - matriz[columnas][filas]
int jugador=1;   //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - el 1 son las rojas, el 2 las amarillas
int n_columna;
uint8_t brillo=200;
long long frame;
int retardo=30;
long long cuenta_animacion=0;
int led1=0,led2=0,led3=0,led4=0;
boolean animacion_hecha=false;
boolean amontonado=true;
boolean boton_pulsado=false;
boolean inicializado=false;
int cont_animacion=0;
boolean win1=false,win2=false;
int r,g,b;

//########################################################### SET_ UP ###############################################################################################
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

//######################################################## LOOP #####################################################################################################
void loop()
{
  //encender_todo();
  frame=millis();
  if(!inicializado)
  {
      inicializar_leds();
  }
  presionar_boton();
  
  animacion_ficha(n_columna, frame);
  
  amontonar_fichas(n_columna, jugador);

  check_win();
}


//################################################### LED_SUP #########################################################################################################

//devuelve el numero del led que hay arriba del todo en la columna

void encender_todo()
{
  for(int i=0;i<84;i++)
  {
    strip.setBrightness(brillo);
    strip.setPixelColor(i,200,30,30);
    strip.show();
  }
}
int led_sup(int columna)
{
  int numero_pixel;
  numero_pixel = (((columna + 1) * 6) - 1);
  return numero_pixel;
}


//##################################################### ANIMACION_FICHA ##############################################################################################

//hace la animación de caída de las fichas
void animacion_ficha(int col, long long frame)
{
  long long resta=frame-cuenta_animacion;
  if(resta>retardo&&n_fichas(col)<6&&boton_pulsado&&total_fichas()<84) //si pasa el tiempo retardo
  {
    if(boton_pulsado)
    {
    cuenta_animacion=frame;
    strip.setBrightness(brillo);
    strip.setPixelColor(led_sup(col)-cont_animacion,r,g,b);
    strip.setPixelColor(led_sup(col)-cont_animacion+42,r,g,b);
    //Serial.print("enciendo ");
    //Serial.println((led_sup(col)-cont_animacion));
    strip.show();
    if(cont_animacion!=0&&cont_animacion<6-n_fichas(col))
    {
      strip.setPixelColor((led_sup(col)+1)-cont_animacion,0);
      strip.setPixelColor((led_sup(col)+1)-cont_animacion+42,0);
      //Serial.print("apago ");
      //Serial.println((led_sup(col)+1)-cont_animacion);
      strip.show();
    }
    cont_animacion++;
    if(cont_animacion==6-n_fichas(col))
    {
      //Serial.println("------------------reset_cont");
      animacion_hecha=true;
      cont_animacion=0;
    }
    }
   }else
   if(total_fichas()==42)
   {
    delay(3000);
    animacion_hecha=true;
    inicializado=false;
   }
   if(n_fichas(col)==6)
   {
    animacion_hecha=true;
   }
   
}
int total_fichas()
{
  int total=0;
  for(int c=0;c<7;c++)
  {
    total+=n_fichas(c);
  }
  return total;
}
//############################################################ SIGUIENTE_HUECO #####################################################################################

//devuelve el numero del led en el que se va a quedar la ficha en la columna

int siguiente_hueco(int columna)
{
  int n_hueco = 0;

  while (matriz[columna][n_hueco] != 0)
  {
    n_hueco++;
  }

  return n_hueco;
}

//########################################################### N_FICHAS ###############################################################################################

//devuelve el numero de fichas que hay en una columna
 
int n_fichas(int columna)
{
  int n_fichas = 0;

  for (int i = 0; i < 6; i++)
  {
    if (matriz[columna][i] != 0)
    {
      n_fichas++;
    }
  }
  return n_fichas;
}

//############################################################ COLOR #################################################################################################
//cambia el color segun el jugador que toque

void color(int &r, int &g, int &b)
{
  if (jugador == 2)
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

//############################################################## AMONTONAR FICHAS ####################################################################################
//amontona las fichas en la columna que se indique y con el colo del jugador que le toque

void amontonar_fichas(int col, int jugador)
{
    if(animacion_hecha)
    {
      animacion_hecha=false;
      if (n_fichas(col) == 6)
      {
    
      } 
      else
      {
        if (n_fichas(col) == 0)  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - si no hay fichas en la columna
        {
          if(jugador==1)
          {
            matriz[col][0] = 1;   // - - - - - - - - - - - - - - - - - - - - - - - - - - - pones ficha en la primera fila (matriz) 
            matriz[col][42] = 1;       
          }else
          {
            matriz[col][0] = 2;
            matriz[col][42]= 2; 
          }
          //imprime_matriz();
        }
        else   //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - si hay fichas en la columna
        {
          if(jugador==1)
          {
             matriz[col][siguiente_hueco(col)] = 1; // - - - - - - - - - - - - - - - - - - pones ficha en el hueco superior (matriz)
             matriz[col][siguiente_hueco(col)+41] = 1;
          }
          else
          {
            matriz[col][siguiente_hueco(col)] = 2; //- - - - - - - - - - - - - - - - - - - pones ficha en el hueco superior (matriz)
            matriz[col][siguiente_hueco(col)+41] = 2;
          }
          //imprime_matriz();
        }
      }
      amontonado=true;
      boton_pulsado=false;
    }
    
  }


//############################################################### IMPRIME_MATRIZ #####################################################################################
//imprime en el monitor serie una matriz de [0,1,2] representando el tablero

void imprime_matriz()
{
  for (int f = 5; f >=0; f--)
  {
    for (int c = 0; c < 14; c++)
    {
      Serial.print(" |");
      if(matriz[c][f]==1)
      {
        Serial.print("X");
      }else
      if(matriz[c][f]==2)
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
//################################################################# PRESIONAR_BOTON ###################################################################################
void presionar_boton()
{
  int sensor3 = digitalRead(3); //columna 1
  int sensor4 = digitalRead(4); //columna 2
  int sensor5 = digitalRead(5); //columna 3
  int sensor6 = digitalRead(6); //columna 4
  int sensor7 = digitalRead(7); //columna 5
  int sensor8 = digitalRead(8); //columna 6
  int sensor9 = digitalRead(9); //columna 7
 if(amontonado)
 {
  if(sensor3==0)
    {
    if(n_fichas(0)<6)
    {
    Serial.println("Boton 1");
    n_columna=0;
    conmutar();
    }
    boton_pulsado=true;
    amontonado=false;
    }
    
    if(sensor4==0)
    {
    if(n_fichas(1)<6)
    {
    Serial.println("Boton 2");
    n_columna=1;
    conmutar();
    }
    boton_pulsado=true;
    amontonado=false;
    }
    
    if(sensor5==0)
    {
    if(n_fichas(2)<6)
    {
    Serial.println("Boton 3");
    n_columna=2;
    conmutar();
    }
    boton_pulsado=true;
    amontonado=false;
    }
   
    if(sensor6==0)
     {
     if(n_fichas(3)<6)
    {
     Serial.println("Boton 4");
     n_columna=3;
     conmutar();
    }
     boton_pulsado=true;
     amontonado=false;
     }
    if(sensor7==0)
    {
    if(n_fichas(4)<6)
    {
    Serial.println("Boton 5");
    n_columna=4;
    conmutar();
    }
    boton_pulsado=true;
    amontonado=false;
    }
    
    if(sensor8==0)
    {
    if(n_fichas(5)<6)
    {
    Serial.println("Boton 6");
    n_columna=5;
    conmutar();
    }
    boton_pulsado=true;
    amontonado=false;
    }
    
    if(sensor9==0)
    {
    if(n_fichas(6)<6)
    {
    Serial.println("Boton 7");
    n_columna=6;
    conmutar();
    }
    boton_pulsado=true;
    amontonado=false;
   }
  }
  color(r,g,b);
}
//################################################################ NUM_LED ############################################################################################
int num_led(int colum, int fila)
{
  int num_led;

  num_led = colum*6 + fila;
  
  return num_led;
}
//################################################################ ANIMACION FINAL ####################################################################################
void animacion_final()
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
  inicializado=false;
}
//################################################################ CONDICION_DE_WIN ###################################################################################
void check_win()
{
  if(!win1&&!win2)
  {
  check_filas();
  check_columnas();
  check_derecha();
  check_izquierda();
  }else
  if(win1)
  {
    jugador=1;
    Serial.println("WIN1");
    animacion_final();
  }else
  if(win2)
  {
    jugador=2;
    animacion_final();
  }
}

void check_columnas()
{
  for(int c=0;c<7;c++)
  {
    for(int f=0;f<3;f++)
    {
      if(matriz[c][f]==1&&matriz[c][f+1]==1&&matriz[c][f+2]==1&&matriz[c][f+3]==1)
      {
        led1=num_led(c,f);
        led2=num_led(c,f+1);
        led3=num_led(c,f+2);
        led4=num_led(c,f+3);
        win1=true;
        break;
      }else
      if(matriz[c][f]==2&&matriz[c][f+1]==2&&matriz[c][f+2]==2&&matriz[c][f+3]==2)
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

void check_filas()
{
  for(int f=0;f<5;f++)
  {
    for(int c=0;c<4;c++)
    {
      if(matriz[c][f]==1&&matriz[c+1][f]==1&&matriz[c+2][f]==1&&matriz[c+3][f]==1)
      {
        led1=num_led(c,f);
        led2=num_led(c+1,f);
        led3=num_led(c+2,f);
        led4=num_led(c+3,f);
        win1=true;
        break;
      }else
      if(matriz[c][f]==2&&matriz[c+1][f]==2&&matriz[c+2][f]==2&&matriz[c+3][f]==2)
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

void check_derecha()
{
    for(int c=0;c<4;c++)
  {
    for(int f=0;f<3;f++)
    {
      if(matriz[c][f]==1&&matriz[c+1][f+1]==1&&matriz[c+2][f+2]==1&&matriz[c+3][f+3]==1)
      {
        led1=num_led(c,f);
        led2=num_led(c+1,f+1);
        led3=num_led(c+2,f+2);
        led4=num_led(c+3,f+3);
        win1=true;
        break;
      }else
      if(matriz[c][f]==2&&matriz[c+1][f+1]==2&&matriz[c+2][f+2]==2&&matriz[c+3][f+3]==2)
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

void check_izquierda()
{
  for(int c=0;c<4;c++)
  {
    for(int f=5;f>2;f--)
    {
      if(matriz[c][f]==1&&matriz[c+1][f-1]==1&&matriz[c+2][f-2]==1&&matriz[c+3][f-3]==1)
      {
        led1=num_led(c,f);
        led2=num_led(c+1,f-1);
        led3=num_led(c+2,f-2);
        led4=num_led(c+3,f-3);
        win1=true;
        break;
      }else
      if(matriz[c][f]==2&&matriz[c+1][f-1]==2&&matriz[c+2][f-2]==2&&matriz[c+3][f-3]==2)
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

//################################################################### RESET ###########################################################################################

//##################################################################### REPRODUCIR_SONIDOS ############################################################################

//##################################################################### CONMUTAR ######################################################################################
void conmutar()
{
        if(jugador==1)
        {
          jugador=2;
          digitalWrite(11,HIGH);
          digitalWrite(10,LOW);
        }else
        {
          jugador=1;
          digitalWrite(11,LOW);
          digitalWrite(10,HIGH);
        }
}
//################################################################## INICIALIZAR_LEDS #################################################################################
void inicializar_leds()
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
      matriz[c][f]=0;
    }
  }
  imprime_matriz();
  digitalWrite(11,HIGH);
  digitalWrite(10,LOW);
  jugador=2;
  inicializado=true;
}
