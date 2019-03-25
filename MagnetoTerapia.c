#INCLUDE <16f88.h>
#device adc=10
#USE DELAY(CLOCK=4000000)
#FUSES XT,NOPROTECT,NOWDT,NOBROWNOUT,NOPUT,PUT,NOLVP,WRT

#include <string.h>
#include <math.h>

                           
//////////////////////////////////PUERTOS///////////////////////////////////////////////////////////////////////////////
#BYTE PORTA= 5
#BYTE PORTB= 6

//////////////////////////////////////////variables//////////////////////////////////////////////////////////////////
long bits;     //Variable almacena los bits
float tem;     //Almacena la temperatura;



int cont;  //se utiliza en timer1
volatile int timeraux;    //se utiliza en timer0 para cambiar frecuencias

/////////////////////////////////////////////////LCD//////////////////////////////////////////////////////////////
//#define USE_PORTB_LCD TRUE //El LCD usa por defecto al puerto D, con esto
                           //definimos al puerto B para el LCD
                           
#define lcd_rs_pin pin_b0
#define lcd_rw_pin pin_b1
#define lcd_enable_pin pin_b2
#define lcd_data4_pin pin_b4
#define lcd_data5_pin pin_b5
#define lcd_data6_pin pin_b6
#define lcd_data7_pin pin_b7

#include <lcd.c> //LIBRERIA DE MANEJO LCD 4 BITS

const char opcion1[]={"1)20MHz"}; // para el LCD
const char opcion2[]={"2)40MHz"};
const char opcion3[]={"3)60MHz"};
const char opcion4[]={"4)80MHz"};

///////////////////////////////////////////////FUNCIONES///////////////////////////////////////////////////////////////////

////////////////////////////////////////TEMP////////////////////////////////////////////////////////////////////////

void TEMP()
{
      set_adc_channel(0);          //Selecciono el canal 0 (RA0)
      delay_ms(1);                 //llamo retardo de 1 ms
      bits=read_adc();  //Guarde el dato del LM en tempe
      delay_ms(1);
      tem=5*bits*100/1023,0; //Conversion de bits a temperatura
      
      
      
    
      if(tem>40)                    //40 ES LA TEMP MAX QUE SELECCIONAMOS
         {
           
            bit_set(PORTA,3);      //PRENDO LED DE ADVERTENCIA 
            disable_interrupts(INT_TIMER1);
            disable_interrupts(INT_TIMER0);
            bit_clear(PORTB, 3);  //DEJO DE MANDAR FRECUENCIA
                                   
         }
}
////////////////////////////////////////////////TMR0/////////////////////////////////////////////////////////////////////

#int_timer0
void timer0() 
{
         output_toggle(pin_b3); //toggle=cambia de estado
         set_timer0(timeraux);
}

//////////////////////////////////////////////TMR1/////////////////////////////////////////////////////////////////////

#int_timer1
void timer1() 
{
         
         
         if(cont == 15) // 7,5 segundos
          {         
             bit_clear(PORTA, 3);             //APAGO LED
             disable_interrupts(INT_TIMER1);
             disable_interrupts(INT_TIMER0);
             bit_clear(PORTB, 3);               //DEJO DE MANDAR FRECUENCIA
             cont=0;
             return;
          }
          else
          {
         enable_interrupts(INT_TIMER0);
         SET_TIMER0(timeraux);
         SET_TIMER1(3036);
         TEMP();
         cont++; 
          
          }
    
       
}

/////////////////////////////////////////MENU///////////////////////////////////////////////////////////////////////
void MENU()
{

      lcd_gotoxy(1,1);
      printf(lcd_putc,opcion1);
      
      lcd_gotoxy(1,2);
      printf(lcd_putc,opcion2);
      
      lcd_gotoxy(9,1);
      printf(lcd_putc,opcion3);
      
      lcd_gotoxy(9,2);
      printf(lcd_putc,opcion4);
}


/////////////////////////////////////////////////////MAIN/////////////////////////////////////////////////////////

void main()
{   

/////////////////////////////////////////CONFIG.ENTRADASYSALIDAS//////////////////////////////////////////////////      
   set_tris_a(0b00000111);          //Pongo el RA0 como entrada
   bit_set(PORTB, 3);               //Pongo la salida para el tren de pulsos
   enable_interrupts(GLOBAL);       //Habilito todas las interrupciones
   setup_adc(adc_clock_internal);   //Selecciono reloj interno para conversion
   setup_adc_ports(pin_a0);     //Pongo el PORTA 0 en analogico;

 
/////////////////////////////////////////LCD//////////////////////////////////////////////////////////////////////      
   lcd_init(); //Inicializa el LCD
///////////////////////////////////////TMR////////////////////////////////////////////////////////////////////////
   
   setup_timer_1(T1_INTERNAL|RTCC_DIV_8); //Seteo el TMR1: reloj interno y preecaler         
   setup_timer_0(T0_INTERNAL|RTCC_DIV_4); //Seteo el TMR0: reloj interno y preecaler
  

   
////////////////////////////////////////////////PULSADOR//////////////////////////////////////////////////////////
   int cant_pulsos=0;
   cont=0; 
   bit_clear(PORTA,3);  
      
      while(1)
      {
            
            MENU(); //Imprimo en el display el menu
            if(input(pin_a2))
              { 
                cant_pulsos=cant_pulsos%4+1;
                delay_ms(300);                    
              }else
            if(input(pin_a1))
            {
            switch(cant_pulsos)
               {
                  case 1:                                                                          
                        timeraux=0; 
                        enable_interrupts(INT_TIMER1);
                        SET_TIMER1(3036);  // 0,5 segundos                      
                        TEMP();
                        break;
                  case 2:
                        timeraux=120;
                        enable_interrupts(INT_TIMER1);
                        SET_TIMER1(3036); 
                        TEMP();
                        break;
                  case 3:              
                        timeraux=200;
                        enable_interrupts(INT_TIMER1);
                        SET_TIMER1(3036);
                        TEMP();
                        break;
                  case 4:                          
                        
                        timeraux=255;
                        enable_interrupts(INT_TIMER1);
                        SET_TIMER1(3036);
                        TEMP();
                        break;
               }
               

   }
}
} 




