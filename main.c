#include "avr.c"
#include "lcd.c"
#include <stdio.h>

typedef enum {true, false} bool;

	
struct time_variable{
	int year,
		month,
		day,
		hour,
		minute,
		second;
};

bool military_time, leapyear, colonblinks;
int time_in_12;
int on_off = 0;
	
void autoModify(struct time_variable *t){
	if(t->second>=60){
		t->second-=60;
		t->minute++;
	}
	if(t->minute>=60){
		t->minute-=60;
		t->hour++;
	}
	if(t->hour>=24){

		if(t->hour>=12){
			time_in_12 = t->hour%12;
		}
		t->hour-=24;
		t->day++;
	}
	int i =(((t->year % 4 == 0) && (t->year % 100 != 0)) ||(t->year % 400 == 0))?1:0;
	
	if(i == 1){
		leapyear = true;
	}else{
		leapyear = false;
	}
	if(t->month == 2){
		if(leapyear == false){
			if(t->day>28){
				t->day = 1;
				t->month++;
			}
		}else{
			if(t->day==30){
				t->day = 1;
				t->month++;
			}
		}
	}else if(t->month == 4 ||t->month == 6 ||t->month == 9 ||t->month == 11){
		if(t->day>30){
			t->day = 1;
			t->month++;
		}
	}else{
		if(t->day>31){
			t->day = 1;
			t->month++;
		}
	}
	
	if(leapyear == false && t->month ==2){
		
	}
	
	
	if(t->month>=13){
		t->month=1;
		t->year++;
	}
}

int is_pressed(int r, int c){
	//set all 
	DDRC = 0;
	PORTC = 0;

	CLR_BIT(DDRC,r);
	SET_BIT(PORTC,r);
	
	SET_BIT(DDRC,c+4);
	CLR_BIT(PORTC,c+4);
	return (GET_BIT(PINC,r))?0:1; //if r bit is 0 return 1 else return 0
}

int get_key(void){
	int r, c;
	for(r = 0; r < 4; r++){
		for(c = 0; c < 4; c++){
			if(is_pressed(r,c)){
				return (r*4)+c+1;
			}
		}
	}
	return 0;
}

void initial_time(struct time_variable *tm, char buff[2][16]){
	tm->year = 2016;
	tm->month = 2;
	tm->day = 28;

	tm->hour = 23;
	tm->minute = 59;
	tm->second = 50;
	
	time_in_12 = tm->hour%12;
}

void display(struct time_variable *tm, char buff[2][16]){
	
	lcd_pos(0,0);
	sprintf(buff[0], "%02d/%02d/%04d   ", tm->month, tm->day,tm->year);
	lcd_puts2(buff[0]);


	if(colonblinks == true){
		lcd_pos(1,0);
		if(military_time == true){
			if(1 == on_off){
				sprintf(buff[1], "%02d %02d %02d   ",tm->hour,tm->minute,tm->second);
				on_off = 0;
			}else{
				sprintf(buff[1], "%02d:%02d:%02d   ",tm->hour,tm->minute,tm->second);
				on_off = 1;
			}
		}else{
			if(1 == on_off){
				if(tm->hour>12){
					sprintf(buff[1], "%02d %02d %02d PM",time_in_12,tm->minute,tm->second);
				}else{
					sprintf(buff[1], "%02d %02d %02d AM",time_in_12,tm->minute,tm->second);
				}
				on_off = 0;
			}else{
				if(tm->hour>12){
					sprintf(buff[1], "%02d:%02d:%02d PM",time_in_12,tm->minute,tm->second);
				}else{
					sprintf(buff[1], "%02d:%02d:%02d AM",time_in_12,tm->minute,tm->second);
				}
			on_off = 1;
			}
		}
		lcd_puts2(buff[1]);
	}else{
		lcd_pos(1,0);
		if(military_time == true){
			sprintf(buff[1], "%02d:%02d:%02d   ",tm->hour,tm->minute,tm->second);
		}else{
			if(tm->hour>12){
				sprintf(buff[1], "%02d:%02d:%02d PM",time_in_12,tm->minute,tm->second);
			}else{
				sprintf(buff[1], "%02d:%02d:%02d AM",time_in_12,tm->minute,tm->second);
			}
		}
		lcd_puts2(buff[1]);
	}
}
//date first row
//time second row

int main(void)
{ 
	lcd_init();
	//avr_init();
	DDRC = 0;//set all pins in PC to N/C
    /* Replace with your application code */
	lcd_clr();
	leapyear= false;
	military_time = true;
//	int lcd_ptr[2][16];
	colonblinks = false;
	int k;
	char buff[2][16];
	struct time_variable tm;
	
	bool set_mode = false;	
	
	while (1){
		k  = get_key();

		if(set_mode == true){
			switch(k){
				case 1: //decrement year
					if(tm.year>0){
						tm.year--;
						autoModify(&tm);
					}
					break;
				case 2: //increment year
					if(tm.year<10000){
						tm.year++;
						autoModify(&tm);
					}
					break;
				case 3: //inc hr
					tm.hour++;
					autoModify(&tm);			
					break;
					
				case 4: //dec hr
					if(tm.hour> 0){
						tm.hour--;
						autoModify(&tm);
					}
					break;
					
				case 5: //inc month
					tm.month++;
					autoModify(&tm);
					break;
				case 6: //dec month
					if(tm.month>1){
						tm.month--;
						autoModify(&tm);
					}
					break;
				case 7: //inc min
					tm.minute++;
					autoModify(&tm);
					break;
				case 8: // dec min
					if(tm.minute){
						tm.minute--;
						autoModify(&tm);
					}
					break;
				case 9: //inc day
					tm.day++;
					autoModify(&tm);
					break;
				case 10: //dec day
					if(tm.day>1){
						tm.day--;
						autoModify(&tm);	
					}
				
					break;
				case 11: //inc sec
					tm.second++;
					autoModify(&tm);
					break;
				case 12: //dec sec
					if(tm.second){
						tm.second--;
						autoModify(&tm);
					}
					
					break;
				case 15: //run modex
					set_mode = false;
					tm.second = 0;
					break;
	
				default:
				
					break;
			}
		}else{
			if(14 == k){
				set_mode = true;
			}else if(13 == k){ 
				initial_time(&tm,buff);
			}else if(8 == k){ //military time: B
				military_time = !military_time;
			}else if(16 == k){ //colon blinks
				colonblinks = !colonblinks;
			}else{
				
			}
		}
		
		
/*
A: set
D: military time
C: !military time
#: enter
*: initial time
*/
		
		autoModify(&tm);
		display(&tm, buff);
		tm.second++;
		avr_wait(750);

    }
}

/*
1:(0*4)+4+1 = 1
2:(0*4)+5+1 = 2
3:(0*4)+6+1 = 3
A:(0*4)+7+1 = 4
4:(1*4)+4+1 = 5
5:(1*4)+5+1 = 6
6:(1*4)+6+1 = 7
B:(1*4)+7+1 = 8
7:(2*4)+4+1 = 9
8:(2*4)+5+1 = 10
9:(2*4)+6+1 = 11
C:(2*4)+7+1 = 12
*:(3*4)+4+1 = 13
0:(3*4)+5+1 = 14
#:(3*4)+6+1 = 15
D:(3*4)+7+1 = 16
*/