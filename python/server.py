from flask import Flask, request, jsonify
from time import sleep
import pigpio

DIR = 23
STEP = 24
ENABLE = 25

pi = pigpio.pi()

pi.set_mode(DIR, pigpio.OUTPUT)
pi.set_mode(ENABLE, pigpio.OUTPUT)
pi.set_mode(STEP, pigpio.OUTPUT)

pi.set_PWM_dutycycle(STEP, 0)
pi.set_PWM_frequency(STEP, 500)
pi.write(DIR, 1)
pi.write(ENABLE, 1)

app = Flask(__name__)

# Define a variable to store the current state
current_state = "Stopped"

@app.route('/control_robot', methods=['GET'])
def control_robot():
    global current_state

    # Retrieve command from the URL parameters
    command = request.args.get('command', '')
    print(command)
    if command:
        # Process the command and update the current state
        # Your logic to process the command goes here
        # For demonstration purposes, let's update the state based on the command
        if command[:2] == 'cw':
            speed = int(command[2:])
            speed = max(0,speed)
            speed = min(255,speed)
            current_state = 'cw'
            pi.write(DIR, 1)
            pi.write(ENABLE, 0)
            pi.set_PWM_dutycycle(STEP, speed)
        elif command[:3] == 'ccw':
            speed = int(command[3:])
            speed = max(0,speed)
            speed = min(255,speed)
            current_state = 'ccw'
            pi.write(ENABLE, 0)
            pi.write(DIR, 0)
            pi.set_PWM_dutycycle(STEP, speed)
        elif command == 'stop':
            current_state = 'Stopped'
            pi.write(ENABLE, 1)
            pi.set_PWM_dutycycle(STEP, 0)
        else:
            current_state = 'Unknown'

        return jsonify({'response': f'Command processed: {command}', 'state': current_state})
    else:
        # If no command is provided, return the current state
        return jsonify({'state': current_state})

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8000)

