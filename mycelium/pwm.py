import pigpio
import time

# Connect to the pigpio daemon
pi = pigpio.pi()

# Set the GPIO pin to be used for PWM
GPIO_PIN = 18

# Set the PWM frequency in Hz
PWM_FREQUENCY = 50

# Set the maximum duty cycle for the servo (corresponding to 2 ms pulse)
PWM_MAX_DUTY_CYCLE = (1000/50 * 2)/255 * 100  # For a 50 Hz frequency

try:
    # Start PWM on the chosen GPIO pin
    pi.set_mode(GPIO_PIN, pigpio.OUTPUT)
    pi.set_PWM_frequency(GPIO_PIN, PWM_FREQUENCY)

    print(f"PWM running at {PWM_FREQUENCY} Hz.")
    print("Moving servo to maximum position.")

    # Allow time for the servo to move
    time.sleep(1)

    print("Servo moved to maximum position.")
    print("Press Ctrl+C to exit.")
    m = 15
    while True:
        for i in range(m):
            pi.set_PWM_dutycycle(GPIO_PIN, i/100 *255)
            print(i)
            time.sleep(.1)
        for i in range(m):
            pi.set_PWM_dutycycle(GPIO_PIN, (m-i)/100 *255)
            time.sleep(.1)


except KeyboardInterrupt:
    print("\nExiting program...")
    pi.set_PWM_dutycycle(GPIO_PIN, 0)  # Turn off PWM
    pi.stop()

