#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#define NULL 0
volatile int cnt=0;
volatile int count=0;
volatile char temper[50];
volatile char temper2;
volatile int adc_num=0;
volatile int adc_count=0;
volatile int x1=0;
volatile int x2=0;

void UART1_Transmit(unsigned char cData){
	while(!(UCSR1A &(1<<UDRE1)));
	UDR1 = cData;
}

unsigned char UART1_Receive(){
	while(!(UCSR1A & (1<<RXC1)));
	temper[count]=UDR1;
	return temper[count];
}

void UART0_Transmit(unsigned char cData2){
	while(!(UCSR0A &(1<<UDRE0)));
	UDR0 = cData2;
}

unsigned char UART0_Receive(){
	while(!(UCSR0A & (1<<RXC0)));
	temper2=UDR0;
	return temper2;
}

int GetADC1(void)                     //ADC값(LM35) 받는 함수
{
	ADMUX = 0x41;                     // Channel Selection
	ADCSRA |= (1 << ADSC);               //ADC Start Conversion
	while(!(ADCSRA & (1 << ADIF)));         //ADC Interrupt Flag
	adc_num = ADC;
	return adc_num;
}


ISR(TIMER2_OVF_vect){
	
if(temper[count] =='a'){
	cnt++;
	if(cnt==10){ // 0도 -> 45도로 팔 들기 (a)

		OCR1A= 5000 * 0.075 + x1;
		OCR1B= 5000 * 0.075 -x2;

		if(OCR1A!=5000*0.1 && OCR1B!=5000*0.05)
		x1++;
		x2++;
		
		cnt=0;
		if(OCR1A == 5000 * 0.1 && OCR1B ==5000 * 0.05){
			count++;
			x1=0;
			x2=0;
		}
	}
}

if(temper[count] =='b'){
	cnt++;
	if(cnt==10){ // 45도 -> 0도 팔 내리기 (b)
		OCR1A = 5000 * 0.1 - x1;
		OCR1B = 5000 * 0.05 + x2;

		if(OCR1A != 5000 * 0.075 && OCR1B != 5000 * 0.075 )
		x1++;
		x2++;		
		
		cnt=0;
		if(OCR1A == 5000 * 0.075 && OCR1B == 5000 * 0.075 ){
			count++;
			x1=0;
			x2=0;
		}
	}
}

if(temper[count] =='c'){
	cnt++;
	if(cnt==1){ // 45도 -> 120도로 팔 들기 (c)
		OCR1A= 5000 * 0.1 + x1;
		OCR1B= 5000 * 0.05 - x2;

		if(OCR1A!=5000*0.118)
		x1++;
		if(OCR1B!=5000*0.025)
		x2++;	

		cnt=0;
		if(OCR1A == 5000 * 0.118 && OCR1B ==5000 * 0.025){
			count++;
			x1=0;
			x2=0;
		UART0_Transmit('c');

		}
	}

}

if(temper[count] =='d'){
	cnt++;
	if(cnt==5){ // 120도 -> 45도로 팔 내리기 (d)
		OCR1A= 5000 * 0.118 - x1;
		OCR1B= 5000 * 0.025 + x2;

		if(OCR1A!=5000*0.1)
		x1++;
		if(OCR1B!=5000*0.05)
		x2++;
		
		cnt=0;
		if(OCR1A == 5000 * 0.095 && OCR1B ==5000 * 0.05){
			count++;
			x1=0;
			x2=0;
		UART0_Transmit('d');
		}
	}

}


if(temper[count] =='e'){
	cnt++;
	if(cnt==10){ // 박스 잡기 0도 -> 45도 (e)
		OCR3A= 5000 * 0.075 + x1;
		OCR3B= 5000 * 0.075 - x2;

		if(OCR3A!=5000*0.097 && OCR3B!=5000*0.052)
		x1++;
		x2++;	
		cnt=0;
		if(OCR3A == 5000 * 0.097 && OCR3B ==5000 * 0.052){
			count++;
			x1=0;
			x2=0;
		}
	}
}

if(temper[count] =='f'){
	cnt++;
	if(cnt==10){ // 박스 놓기 45도 -> 0도 (f)
		OCR3A= 5000 * 0.095 - x1;
		OCR3B= 5000 * 0.055 + x2;

		if(OCR3A != OCR3B)
		x1++;
		x2++;
		
		cnt=0;
		if(OCR3A == OCR3B){
			count++;
			x1=0;
			x2=0;
		}
	}
}
	
if(temper[count] =='g'){
	cnt++;
	if(cnt==10){ // 팔 내리기 0도 -> -45도 (g)
		OCR1A= 5000 * 0.075 - x1;
		OCR1B= 5000 * 0.075 + x2;

		if(OCR1A != 5000 * 0.05 && OCR1B != 5000 * 0.1)
		x1++;
		x2++;
		
		cnt=0;
		if(OCR1A == 5000 * 0.05 && OCR1B ==5000 * 0.1){
			count++;
			x1=0;
			x2=0;
		}
	}
}

if(temper[count] =='h'){
	cnt++;
	if(cnt==10){ // 팔 올리기 0도 <- -45도 (g)
		OCR1A= 5000 * 0.05 + x1;
		OCR1B= 5000 * 0.1 - x2;

		if(OCR1A != OCR1B)
		x1++;
		x2++;
		
		cnt=0;
		if(OCR1A == OCR1B){
			count++;
			x1=0;
			x2=0;
		}
	}
}

if( 250 < adc_num && adc_num < 400){
	adc_count++;
	if(adc_count>5){
		if(adc_num>500){
			temper[count]='g';
			count++;
		}
	}
}
if(temper[count]=='g'){
	_delay_ms(2000);
	temper[count]='h';
	count++;
}
TCNT2 = 131;
}

//void Uart1_Trans_Num(int num) {
	////UART_Trasnmit은 문자를 보내기 때문에 숫자를 그대로 쓰면 엉뚱한 값이 송신된다.
	////따라서 아스키코드 표에 맞게 송신해 주기 위하여 +48을 해주면 해당 숫자에 대응하는 아스키 코드로 전송한다.
	//if(num < 0) {//음수를 입력받을 경우에는 '-'를 출력한 뒤 양수로 변환하고 계산한다.
		//UART1_Transmit('-');
		//num *= -1;
	//}
	//num = (num%10000);//나머지 연산으로 다섯 번째 자리수를 삭제.
	//UART1_Transmit(num/1000 +48);//네 번째 자리수 송신 나누기 연산으로 네 번째 자리 수만 취한다.
	//num = (num%1000);//나머지 연산으로 네 번째 자리수를 삭제.
	//UART1_Transmit(num/100 +48);//두 번째 자리수를 송신.
	//num = num%100;//나머지 연산으로 세 번째 자리수를 삭제.
	//UART1_Transmit(num/10 +48);//두 번째 자리수를 송신.
	//num = num%10;//나머지 연산으로 두 번째 자리수를 삭제.
	//UART1_Transmit(num +48);//첫 번째 자리수를 송신.
	//UART1_Transmit('\n');
	//UART1_Transmit('\r');
//}

int main(void){
	
	DDRB=0xff;
	DDRE=0xff;
	DDRD=0x08;
	DDRF=0x00;
	
	TCCR1A = (1<<COM1A1)|(1<<COM1B1)|(1<<WGM11);
	TCCR1B = (1<<WGM13)|(1<<WGM12)|(0<<CS12)|(1<<CS11)|(1<<CS10);
	//prescale 64
	TCCR3A = (1<<COM3A1)|(1<<COM3B1)|(1<<WGM31);
	TCCR3B = (1<<WGM33)|(1<<WGM32)|(0<<CS32)|(1<<CS31)|(1<<CS30);
	//prescale 64
	ADMUX = 0x41;
	ADCSRA = (1 << ADEN)|(1 << ADPS2)|(1 << ADPS1)|(1 << ADPS0);
	
	TCCR2=(0<<WGM20)|(0<<WGM21)|(1<<CS22)|(0<<CS21)|(0<<CS20);
	TIMSK=(1<<TOIE2);
	TCNT2=131;
	//prescale 256
	
	UCSR1A=0x00;
	UCSR1B=0x18;
	UCSR1C=0x06;
	UBRR1H=0;
	UBRR1L=8;
	
	UCSR0A=0x00;
	UCSR0B=0x18;
	UCSR0C=0x06;
	UBRR0H=0;
	UBRR0L=8;
	
	ICR1 = 4999;
	ICR3 = 4999;
	OCR1A =5000*0.075;
	OCR1B =5000*0.075;
	OCR3A =5000*0.075;
	OCR3B =5000*0.075;
	sei();
	
	while(1){
		UART1_Receive();
	}
	
}
