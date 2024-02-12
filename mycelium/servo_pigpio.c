#include <time.h>
#include <stdio.h>
#include <pigpiod_if2.h>
#include <stdlib.h>

double map(double v, double start, double end, double new_start, double new_end)
{
	return (v-start)*(new_end-new_start)/(end-start) + new_start;
}

int servo(int pos)
{
	int pwm_frequency = 50;
	int gpio_pin = 18;
	int pi = pigpio_start(NULL, NULL);
	set_mode(pi, gpio_pin, PI_OUTPUT);
	set_PWM_frequency(pi, gpio_pin, 50);
	double k = map(pos, 0, 180, 255.0*50*.5/1000.0, 255.0*2.5*50/1000.0);
	set_PWM_dutycycle(pi, gpio_pin, k);
	time_sleep(.1);
	pigpio_stop(pi);
}

int main(int argc, char *argv[])
{
	if (argc !=2)
		printf("error: usage %s <value>\n", argv[0]);
	servo(atoi(argv[1]));
	return 0;
}
