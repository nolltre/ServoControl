/**
 * File name:	servo.c
 * Copyright:   Daniel Ivarsson
 * Created:		2018-02-01
 * Modified:	2018-02-01
 * Description: A simple ncurses program to move a servo around using vi-like
 *              keyboard bindings.
**/

#include <curses.h>
#include <pigpio.h>

// Define GPIO pins
#define PAN 18
#define TILT 19

// Start-up positons, i.e. values for the PWM signal
#define PAN_DEFAULT 1270
#define TILT_DEFAULT 1270

// Limits of travel for these servos
#define MIN_PAN 750
#define MAX_PAN 2000

#define MIN_TILT 1000
#define MAX_TILT 1670

// How much should each key reading step the servo?
#define STEP_SIZE 5

// These define which row the status messages for the servo positions should
// be printed
#define TILT_ROW 10
#define PAN_ROW 11

// Main function
int main (void)
{

    // This will hold the character entered
    char c = 0;

    // Initialise curses
    newterm(NULL, stdin, stdout);
    timeout(-1);
    // Do not echo to screen
    noecho();
    // No cursor
    curs_set(0);

    // Set up PiGPIO
    if (gpioInitialise() != PI_INIT_FAILED) {

        // NOTE: Will not check return values from now on. You have been warned!

        // Try to init hardware PWM, no duty cycle (off) @ 50 Hz
        gpioHardwarePWM(PAN, 50, 0);
        gpioHardwarePWM(TILT, 50, 0);

        // Set default values
        int panVal = PAN_DEFAULT;
        int tiltVal = TILT_DEFAULT;

        // Move the servos to the initial position
        mvprintw(PAN_ROW, 0, "Started out with pan as %d!\n", panVal);
        gpioServo(PAN, panVal);
        mvprintw(TILT_ROW, 0, "Started out with tilt as %d!\n", tiltVal);
        gpioServo(TILT, tiltVal);

        // Print an informational message and go into the character read loop
        mvprintw(0, 0, "h,j,k,l = moves the servo LEFT, UP, DOWN, RIGHT\ns = stop PWM\nq = quits\n\n");
        while (c != 'q') {
            c = getch();  // Catch one character
            switch(c) {
                case 's':
                    gpioServo(PAN, 0);
                    mvprintw(PAN_ROW, 0, "Pan PWM stopped");
                    gpioServo(TILT, 0);
                    mvprintw(TILT_ROW, 0, "Tilt PWM stopped");
                    break;

                case 'h':
                    if (panVal >= MAX_PAN) {
                        mvprintw(PAN_ROW, 0, "Max pan, %d!\n", panVal);
                    } else {
                        panVal += STEP_SIZE;
                        mvprintw(PAN_ROW, 0, "Setting pan to: %d\n", panVal);
                        gpioServo(PAN, panVal);
                    }
                    break;
                case 'j':
                    if (tiltVal >= MAX_TILT) {
                        mvprintw(TILT_ROW, 0, "Max tilt, %d!\n", tiltVal);
                    } else {
                        tiltVal += STEP_SIZE;
                        mvprintw(TILT_ROW, 0, "Setting tilt to: %d\n", tiltVal);
                        gpioServo(TILT, tiltVal);
                    }
                    break;
                case 'k':
                    if (tiltVal <= MIN_TILT) {
                        mvprintw(TILT_ROW, 0, "Min tilt, %d!\n", tiltVal);
                    } else {
                        tiltVal -= STEP_SIZE;
                        mvprintw(TILT_ROW, 0, "Setting tilt to: %d\n", tiltVal);
                        gpioServo(TILT, tiltVal);
                    }
                    break;
                case 'l':
                    if (panVal <= MIN_PAN) {
                        mvprintw(PAN_ROW, 0, "Min pan, %d!\n", panVal);
                    } else {
                        panVal -= STEP_SIZE;
                        mvprintw(PAN_ROW, 0, "Setting pan to: %d\n", panVal);
                        gpioServo(PAN, panVal);
                    }
                    break;
                default:
                    break;

            }
        }
        // Clean up
        endwin();
    } else {
        // Couldn't start PiGPIO, bail out
        endwin();
        printf("Unable to initialise GPIO!\n");
    }
    // Clean up GPIO
    gpioTerminate();

    return 0 ;
}
