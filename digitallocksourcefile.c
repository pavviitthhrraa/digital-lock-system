#include <reg51.h>
#include <string.h>

/* ================================================
   PIN DEFINITIONS
   LCD Data    = P0 (D0-D7)
   Keypad      = P1 (Row/Col)
   LCD RS      = P2.0
   LCD EN      = P2.1
   Buzzer      = P3.1 (ACTIVE LOW - sinking mode)
                 5V -> Buzzer(+)  Buzzer(-) -> P3.1
   IN1 Motor   = P3.2
   IN2 Motor   = P3.3
   Red LED     = P3.6
   Green LED   = P3.7
   ================================================ */

sbit RS        = P2^0;
sbit EN        = P2^1;
sbit BUZZER    = P3^1;   /* ACTIVE LOW: 0=ON, 1=OFF */
sbit IN1       = P3^2;
sbit IN2       = P3^3;
sbit RED_LED   = P3^6;
sbit GREEN_LED = P3^7;

/* ================================================
   CONSTANTS
   ================================================ */
#define MAX_ATTEMPTS 3
#define UNLOCK_DELAY 3000
#define LOCK_DELAY   2000

/* ================================================
   GLOBAL VARIABLES
   ================================================ */
char pass[5]  = "1234";
char pass2[5];
int  i        = 0;
int  attempts = 0;

/* ================================================
   DELAY
   ================================================ */
void delay(unsigned int a)
{
    unsigned int x, y;
    for(x = 0; x < a; x++)
        for(y = 0; y < 255; y++);
}

/* ================================================
   BUZZER CONTROL
   Active LOW sinking mode
   5V  -> Buzzer(+) long leg
   P3.1 -> Buzzer(-) short leg
   0 = ON   (pin sinks current)
   1 = OFF  (pin high, no current)
   ================================================ */
void buzzer_on()
{
    BUZZER = 0;   /* LOW = ON */
}

void buzzer_off()
{
    BUZZER = 1;   /* HIGH = OFF */
}

/* ================================================
   LCD FUNCTIONS
   ================================================ */
void cmd(unsigned char cm)
{
    P0 = cm;
    RS = 0;
    EN = 1;
    delay(2);
    EN = 0;
    delay(2);
    P0 = 0xFF;
}

void dat(unsigned char dt)
{
    P0 = dt;
    RS = 1;
    EN = 1;
    delay(2);
    EN = 0;
    delay(2);
    P0 = 0xFF;
}

/* Set cursor on 1602 LCD */
void set_cursor(unsigned char col)
{
    if(col < 8)
        cmd(0x80 + col);
    else
        cmd(0xC0 + (col - 8));
}

/* Display string from column */
void display_at(unsigned char col, char *str)
{
    while(*str != '\0')
    {
        if(col == 8) cmd(0xC0);
        dat(*str);
        str++;
        col++;
    }
}

/* Clear LCD */
void lcd_clear()
{
    cmd(0x01);
    delay(10);
    cmd(0x80);
}

/* LCD Initialize */
void lcdint()
{
    P0 = 0xFF;
    delay(50);
    cmd(0x38); delay(10);
    cmd(0x38); delay(10);
    cmd(0x38); delay(10);
    cmd(0x08); delay(5);
    cmd(0x01); delay(10);
    cmd(0x06); delay(5);
    cmd(0x0C); delay(5);
    cmd(0x80); delay(5);
}

/* Show attempt counter At:X/3 on right half */
void show_attempts()
{
    set_cursor(8);
    dat('A');
    dat('t');
    dat(':');
    dat('0' + attempts);
    dat('/');
    dat('0' + MAX_ATTEMPTS);
}

/* Show password prompt */
void show_prompt()
{
    lcd_clear();
    display_at(0, "Pass:");
    show_attempts();
}

/* ================================================
   MOTOR FUNCTIONS
   ================================================ */
void motor_unlock()
{
    IN1 = 1;
    IN2 = 0;
}

void motor_lock()
{
    IN1 = 0;
    IN2 = 1;
}

void motor_stop()
{
    IN1 = 0;
    IN2 = 0;
}

/* ================================================
   KEY RELEASE WAIT
   Prevents double key detection
   ================================================ */
void wait_release()
{
    delay(30);
    while(1)
    {
        P1 = 0xF0;
        delay(10);
        if((P1 & 0xF0) == 0xF0) break;
    }
    delay(30);
}

/* ================================================
   ACTION FUNCTIONS
   ================================================ */

/* Correct password - unlock door */
void do_unlock()
{
    lcd_clear();
    display_at(0, "Door");
    display_at(8, "Unlockd!");
    GREEN_LED = 1;
    RED_LED   = 0;
    buzzer_off();          /* buzzer OFF on correct */
    motor_unlock();
    delay(UNLOCK_DELAY);

    /* Auto door close */
    lcd_clear();
    display_at(0, "Door");
    display_at(8, "Closing.");
    motor_lock();
    delay(LOCK_DELAY);
    motor_stop();

    /* Door locked */
    lcd_clear();
    display_at(0, "Door");
    display_at(8, "Locked  ");
    delay(1000);
    GREEN_LED = 0;
}

/* Wrong password alert */
void do_wrong()
{
    lcd_clear();
    display_at(0, "Wrong!");
    show_attempts();
    RED_LED = 1;
    buzzer_on();           /* buzzer ON - active LOW */
    motor_stop();
    delay(2000);
    RED_LED = 0;
    buzzer_off();          /* buzzer OFF */
}

/* System lock after max attempts */
void do_system_lock()
{
    lcd_clear();
    display_at(0, "Sys");
    display_at(8, "LOCKED!!");
    buzzer_on();           /* buzzer ON */
    motor_lock();
    delay(LOCK_DELAY);
    motor_stop();
    buzzer_off();          /* buzzer OFF */
    RED_LED = 1;

    /* Blink red LED forever
       Only hardware RESET can escape this */
    while(1)
    {
        RED_LED = 1; delay(300);
        RED_LED = 0; delay(300);
    }
}

/* Clear all entered digits - C key */
void clear_input()
{
    i        = 0;
    pass2[0] = '\0';
    show_prompt();
}

/* Delete last digit - B key */
void backspace()
{
    if(i > 0)
    {
        i--;
        pass2[i] = '\0';
        set_cursor(5 + i);
        dat(' ');
        set_cursor(5 + i);
    }
}

/* Check entered password */
void check_password()
{
    pass2[i] = '\0';
    attempts++;

    if(strcmp(pass, pass2) == 0)
    {
        /* Correct */
        attempts = 0;
        do_unlock();
    }
    else
    {
        /* Wrong */
        if(attempts >= MAX_ATTEMPTS)
        {
            do_wrong();
            do_system_lock();   /* never returns */
        }
        else
        {
            do_wrong();
        }
    }

    /* Reset for next attempt */
    i         = 0;
    GREEN_LED = 0;
    RED_LED   = 0;
    buzzer_off();          /* buzzer OFF on reset */
    show_prompt();
}

/* ================================================
   KEYPAD SCAN

   Physical layout:
   [ 1 ][ 2 ][ 3 ][ A ]  Row 0  P1=0xFE
   [ 4 ][ 5 ][ 6 ][ B ]  Row 1  P1=0xFD
   [ 7 ][ 8 ][ 9 ][ C ]  Row 2  P1=0xFB
   [ * ][ 0 ][ # ][ D ]  Row 3  P1=0xF7

   Key assignments:
   0-9 = password digits
   C   = clear all input
   B   = backspace
   #   = confirm password
   A,D,* = ignored
   ================================================ */
char scan_key()
{
    /* Row 0: 1,2,3,A */
    P1 = 0xFE; delay(2);
    if(P1==0xEE){delay(30);if(P1==0xEE)return '1';}
    if(P1==0xDE){delay(30);if(P1==0xDE)return '2';}
    if(P1==0xBE){delay(30);if(P1==0xBE)return '3';}
    if(P1==0x7E){delay(30);if(P1==0x7E)return 'A';}

    /* Row 1: 4,5,6,B */
    P1 = 0xFD; delay(2);
    if(P1==0xED){delay(30);if(P1==0xED)return '4';}
    if(P1==0xDD){delay(30);if(P1==0xDD)return '5';}
    if(P1==0xBD){delay(30);if(P1==0xBD)return '6';}
    if(P1==0x7D){delay(30);if(P1==0x7D)return 'B';}

    /* Row 2: 7,8,9,C */
    P1 = 0xFB; delay(2);
    if(P1==0xEB){delay(30);if(P1==0xEB)return '7';}
    if(P1==0xDB){delay(30);if(P1==0xDB)return '8';}
    if(P1==0xBB){delay(30);if(P1==0xBB)return '9';}
    if(P1==0x7B){delay(30);if(P1==0x7B)return 'C';}

    /* Row 3: *,0,#,D */
    P1 = 0xF7; delay(2);
    if(P1==0xE7){delay(30);if(P1==0xE7)return '*';}
    if(P1==0xD7){delay(30);if(P1==0xD7)return '0';}
    if(P1==0xB7){delay(30);if(P1==0xB7)return '#';}
    if(P1==0x77){delay(30);if(P1==0x77)return 'D';}

    return 0;
}

/* ================================================
   MAIN
   ================================================ */
void main()
{
    char key;

    /* Initialize all ports safely */
    P0 = 0xFF;
    P1 = 0xFF;
    P2 = 0xFF;
    P3 = 0x00;

    /* Initialize outputs */
    GREEN_LED = 0;
    RED_LED   = 0;
    buzzer_off();          /* BUZZER = 1 = OFF at startup */
    motor_stop();

    /* Initialize LCD */
    lcdint();

    /* Show initial prompt */
    show_prompt();

    /* ============ MAIN LOOP ============ */
    while(1)
    {
        key = scan_key();
        if(key == 0) continue;

        wait_release();

        /* C key: clear all input */
        if(key == 'C')
        {
            clear_input();
            continue;
        }

        /* B key: backspace */
        if(key == 'B')
        {
            backspace();
            continue;
        }

        /* # key: manual confirm */
        if(key == '#')
        {
            if(i > 0)
                check_password();
            continue;
        }

        /* Digits 0-9 */
        if(i < 4)
        {
            if(key >= '0' && key <= '9')
            {
                pass2[i] = key;
                set_cursor(5 + i);
                dat('*');
                i++;
            }
        }

        /* Auto check after 4 digits */
        if(i == 4)
        {
            check_password();
        }
    }
}

/* ================================================
   PIN CONNECTIONS SUMMARY
   P0.0-P0.7  -> LCD D0-D7
   P1.0-P1.3  -> Keypad Row 1-4
   P1.4-P1.7  -> Keypad Col 1-4
   P2.0       -> LCD RS
   P2.1       -> LCD EN
   P3.1       -> Buzzer (-) short leg  ACTIVE LOW
                 5V -> Buzzer (+) long leg
   P3.2       -> L293D IN1
   P3.3       -> L293D IN2
   P3.6       -> Red LED (330 ohm)
   P3.7       -> Green LED (330 ohm)
   ================================================ */