#include "stm32f10x.h"
char B[100]="Brake high", A[100]="Brake low", C[100]="Forward" , D[100]="Reverse" ,E[100]="Kill pressed" ,F[100]="Kill released" ,G[100]="Nuetral",H[100]="Charging",I[100]=" Charge off";

char ts;

	void timer4(int x)
	{
	RCC->APB1ENR |= (1<<2); // TIMER 4 ke clock ko enable kia 
	
	//ARR ki value set karne ke liye ham 72Mhz ka clock frequency hai usko 1 Mhz se divide karenge agar hamko 1 micro sec ka delay chahiye 
	
	TIM4->PSC =7200;//7200 for 1ms //PRESCALER SET KIA 0 se start hoga clock esa
	TIM4->ARR =10*x;// auto reload register set kia 0-65535 tak jayega 
	// isse ab timer 72 tak jayega phir vapis 0 pe jana chahiye
	
	// ab timer enable karenge
	
	TIM4->CR1 |= (1<<0);//CEN ko 1 set kia ;counter enabled 
	
	//register update hua ki nai vo pata karne ke liye uif ko 1 set karenge 
	
	while(!(TIM4->SR & (1<<0))); // WAIT KARENGE dekhne ke liye ki UIF set hua ki nai
	TIM4->SR &= ~(1<<0);//UIF0
	
	TIM4->CR1 &= ~(1<<0);//TIMER OFF
	}

	void TX(char string[])
	{
		for(int i=0; string[i] != '\0'; i++)
			{
				USART1->DR = string[i];
				while(!(USART1->SR & (1<<7)));//wait for data to be transferer
				while(!(USART1->SR & (1<<6)));//tc transmission compelte
			}
	}
	
	void TP(char *str)
	{
		for(int j=0; str[j] != '\0'; j++)	
			{
				USART1->DR = str[j];
				while(!(USART1->SR & (1<<7)));
				while(!(USART1->SR & (1<<6)));
			}
	}
	
	void TI(int k)
	{
		USART1->DR = k;											
		while(!(USART1->SR & (1<<7)));				
		while(!(USART1->SR & (1<<6)));				
	}

void TC(char c)
	{
		USART1->DR = c;												
		while(!(USART1->SR & (1<<7)));				
		while(!(USART1->SR & (1<<6)));
	}
	
	void FNR(char st[])
	{
			TP("t0.txt=\":");
			TX(st);
			TP("\"");
			TI(0XFF);//last ke 3 ff
			TI(0XFF);
			TI(0XFF);
	}
	
	void brake(char st[])
	{
			TP("t1.txt=\":");
			TX(st);
			TP("\"");
			TI(0XFF);
			TI(0XFF);
			TI(0XFF);
		  //delay(20);
	}
	
	void kill(char st[])
	{
			TP("t2.txt=\":");
		  TX(st);
		  TP("\"");
			TI(0XFF);
			TI(0XFF);
			TI(0XFF);
	}
	
	void charging(char st[])
	{
			TP("t3.txt=\":");
			TX(st);
		  TP("\"");
			TI(0XFF);  
			TI(0XFF);
			TI(0XFF);
	}
	
void BH(void)//brake high
	{
		brake (B);
		//timer4(1000);
	//	GPIOB->ODR |=(1<<11);  //brake light on                       
	}

void BLOW(void)
	{
		brake (A);
	//	GPIOD->ODR &= ~(1<<11);                      
	}

void FH()//forward high kia
	{
			FNR(C);
	}

void FLOW()//forward low  matlab nuetral
	{
			FNR(G);
	}

void RH()//reverse high
	{
			FNR(D);
		  //GPIOA->ODR|=(1<<15);     //reverse light on                 
  }

void RLOW(S)//reverse low nuetral
	{
		 // GPIOA->ODR &= ~(1<<15);                    
			FNR(G);
	}
	
	void CH(void)
	{
		charging(H);
	}
	
	void CL(void)
	{
		charging(I);
	}
	
	void KH(void)
	{
		kill(E);
	}
	
	void KL(void)
	{
		kill(F);
	}
	
	int Temp;
	
	int rx(void)
	{
	//int Temp;
	
		while(USART1->SR & (1<<5))            //AGAR LCD SE KUCH NHI BHEJA TO CODE ATAK JAEGA .. ISLIYE IF RXNE SET HO TO HI RECIEVE ME JAEGA
			{
				while (!(USART1->SR & (1<<5)));  // Wait for RXNE to SET.. This indicates that the data has been Received
				Temp = USART1->DR;  // Read the data. 
				return Temp;	
			}
	
	}

	
	
int main()
	{
		
	int startstate,killstate,brakestate,chargestate,fstate,rstate,accstate,rtdsstate;
	int i,j;
	int k=1;
	int f,r;
	int m=0;	
	
	RCC->APB2ENR |= (7<<2);//PORT A PORT B PORT C clock enable
	
	RCC->APB2ENR |= ((1<<14));//usart 1
	
	
	//LCD receive and transmit initialise
	
	//PA9 TX alternate function output push pull and 50 Mhz
	
	GPIOA->CRH |= ((1<<5) | (1<<4) | (1<<7));
	GPIOA->CRH &= ~(1<<6);
	
	//PA10 RX floating input set krne ki jarurat nhi pele se hi hota pr kr dete hai
	
	GPIOA->CRH |= (1<<10);
	GPIOA->CRH &= ~((1<<11) | (1<<9) | (1<<8));
	
	USART1->BRR = 0x1D4C; //for 72Mhz apb1 usart 9600 baud rate
	
	USART1->CR1 |= ((1<<13) | (1<<3) | (1<<2));//usart re te enable
	
	
	//gpio initialise
	
	//start
	//A3 SWITCH INPUT
  GPIOA->CRL &= ~((1<<12) | (1<<13) | (1<<14));
	GPIOA->CRL |= (1<<15);
	
	//charging
	//A2 SWITCH INPUT 
	GPIOA->CRL &= ~((1<<8) | (1<<9) | (1<<10));
	GPIOA->CRL |=(1<<11);
	
	//kill switch 
  //A7 SWITCH INPUT 
	GPIOA->CRL &= ~((1<<28) | (1<<29) | (1<<30));
	GPIOA->CRL |=(1<<31);
	
	//brake
  //B11 SWITCH INPUT 
  GPIOB->CRH &= ~((1<<12) | (1<<13) | (1<<14));
	GPIOB->CRH |=(1<<15);
	
	//A8 BRAKE LIGHT
  GPIOA->CRH |= ((1<<0) | (1<<1));
	GPIOA->CRH &= ~((1<<2) | (1<<3));
	
	//FNR
  //B10 FORWARD SWITCH INPUT 
	GPIOB->CRH &= ~((1<<8) | (1<<9) | (1<<10));
	GPIOB->CRH |=(1<<11);
	
	//B1 REVERSE SWITCH INPUT 
	GPIOB->CRL &= ~((1<<4) | (1<<5) | (1<<6));
	GPIOB->CRL |=(1<<7);
	
	//A11->A15->b5 REVERSE LIGHT
	GPIOB->CRL |= ((1<<20) | (1<<21));
	GPIOB->CRL &= ~((1<<22) | (1<<23));
 
	//A12->B3->b6 REVERSE ALARM OUTPUT 
	GPIOB->CRL |= ((1<<24) | (1<<25));
	GPIOB->CRL &= ~((1<<26) | (1<<27));
	
	//A10->A12 RTDS OUTPUT
	GPIOA->CRH |= ((1<<16) | (1<<17));
	GPIOA->CRH &= ~((1<<18) | (1<<19));
	
	//A9->A11 TSAL OUTPUT
	GPIOA->CRH |= ((1<<12) | (1<<13));
	GPIOA->CRH &= ~((1<<14) | (1<<15));
	
	//C15 DC OUTPUT
	GPIOC->CRH |= ((1<<28) | (1<<29));
	GPIOC->CRH &= ~((1<<30) | (1<<31));
	GPIOA->CRL |= ((1<<0) | (1<<1));//a0
	GPIOA->CRL &= ~((1<<2) | (1<<3));
	
	//C14 CC OUTPUT
	GPIOC->CRH |= ((1<<24) | (1<<25)); 
	GPIOC->CRH &= ~((1<<26) | (1<<27));
	
	
	
	//adc initialise
	//accelarator input 	
	//ACCELARATOR INPUT B0
	GPIOB->CRL &= ~((1<<0) | (1<<1) | (1<<2) | (1<<3)); //Analog input mode PIN B0
	
	int adcval=0;
	RCC->CFGR |=(2<<14); // setting prescalar /6 10 set kia but 15,14 ko clock must not exceed 14 Mhz
	RCC->APB2ENR |= ((1<<0) | (1<<9)); //ADC1 and alternate function clock enabled 
	ADC1->SMPR2 |=((1<<26)|(1<<25)|(1<<24));//239.5 cycles sample selection 8th select kia hai kyuki 8th channel 
	ADC1->SQR3 |= (1<<3);//sequence 1 ko channel 8 set krne ke liye 01000
	ADC1->CR2 |= ((1<<1) | (1<<0));//adc on and continuous mode set
	
	timer4(1);
	ADC1->CR2 |= (1<<0);//ADC ON again
	timer4(1);//1 millisec delay firse
	ADC1->CR2 |= (1<<2);//enable calibration
	while((ADC1->CR1 &(1<<2)));//CALIBRATION COMPLETE HONE KA WAIT KIA
	
	
	
		//start
		
	ts = rx();	
	if(((!(GPIOA->IDR &(1<<3))) || (ts == 0x0023)))
	{
			i=1;
	}
	//(!(GPIOA->IDR &(1<<3))) || (ts == 0x0023))
	
	if( chargestate==0 && j==1 && i==1 && killstate==0 && (!(GPIOB->IDR &(1<<10))) && (!(GPIOB->IDR &(1<<1))))// IF SWITCH IS HIGH AND BRAKE IS ON
	{
		
		GPIOC->ODR |= (1<<15);//DC ON
		GPIOA->ODR |=(1<<0);
		if(k==1)
		{
			GPIOA->ODR |= (1<<12);//RTDS OUTPUT
			timer4(1000);//2 sec sound
			rtdsstate = 1 ;
			GPIOA->ODR &= ~(1<<12);//RTDS OUTPUT low
			k++;
		}
		startstate=1;
	} 
	
	else if(GPIOA->IDR &(1<<3) || (!(ts == 0x0023)))
	{
		GPIOC->ODR &= ~(1<<15);//DC OFF
		GPIOA->ODR &= ~(1<<0);
		GPIOA->ODR &= ~(1<<11);//tsal off
	  startstate=0;
		j=0;
		i=0;
		f=0;
		k=1;
		r=0;
	}
	
	
	//TSAL ON
	if(startstate==1)
	{
		GPIOA->ODR |= (1<<11);//TSAL OUTPUT
		//timer4(200);//0.2 sec sound
		//GPIOA->ODR &= ~(1<<11);//TSAL OUTPUT low
		//timer4(200);
	}

	
	//ACCELARATOR 
	if(ADC1->SR &(1<<1))//EOC read hua ki nai check 
		{
		adcval = ADC1->DR;
		}
		if(adcval<150)
		{
			accstate=1;
		}
		else
		{
			accstate=0;
		}
	
	//CHARGING
	if (!(GPIOA->IDR &(1<<2)) && killstate==0)
	{
		GPIOC->ODR |=(1<<14); // CC ON
		GPIOC->ODR &= ~(1<<15); //DC OFF
    chargestate=1;	
		CH();
	}
	else
	{
		GPIOC->ODR &= ~(1<<14); // CC OFF
	  chargestate=0;
    CL();
	}
	
	//KILL SWITCH 
	if (!(GPIOA->IDR &(1<<7))) //KILL ON
	{
		GPIOA->ODR &= ~(1<<15); //REVERSE ligth off
		GPIOB->ODR &= ~(1<<3); //REVERSE buzzer
		GPIOA->ODR &= ~(1<<11); //TSAL OUTPUT low
		GPIOC->ODR &= ~(1<<15); // DC OFF
		GPIOC->ODR &= ~(1<<14); //5 CC OFF
	  killstate=1;
		KH();
	}
	else
	{
	  killstate=0;
		KL();
	}
	
	//BRAKE 
	if (GPIOB->IDR &(1<<11)) //BRAKE on 
	{
		GPIOA->ODR |= (1<<8);//brake light on 
		BH();
	  brakestate=1;
		j=1;
	}
	else
	{
		GPIOA->ODR &= ~(1<<8); //brake light off
		BLOW();
	  brakestate=0;
	}
	
	//FNR
	//FORWARD
	
	if ((GPIOB->IDR &(1<<10)) && rstate==0 && startstate==1 && killstate==0 && chargestate==0) //FORWARD ON
	{
		GPIOA->ODR &= ~(1<<15);//REVERSE ligth off
	  fstate=1;
		FH();
	}
	else
	{
	  fstate=0;
		f=1;
	//	FLOW();
	}
	
	//REVERSE 
	if (GPIOB->IDR &(1<<1) && fstate==0 && startstate==1 && killstate==0 && chargestate==0) //REVERSE ON
	{
		GPIOA->ODR |= (1<<15);//REVERSE light on 
		GPIOB->ODR |= (1<<3);//REVERSE alarm on
		RH();
		rstate=1;
	}
	else
	{
		GPIOA->ODR &= ~(1<<15); //REVERSE ligth off
		GPIOB->ODR &= ~(1<<3);//REVERSE alarm on
	//	RLOW();
	  rstate=0;
		r=1;
	}
	//nuetral
	if(r==1 && f==1)
	{
		FLOW();
	}
	}
