from flask import Flask, request, jsonify

app = Flask(__name__)

# Define a variable to store the current state
current_state = "Idle"

@app.route('/control_robot', methods=['GET'])
def control_robot():
    global current_state

    # Retrieve command from the URL parameters
    command = request.args.get('command', '')

    if command:
        # Process the command and update the current state
        # Your logic to process the command goes here
        # For demonstration purposes, let's update the state based on the command
        if command == 'start':
            current_state = 'Running'
        elif command == 'stop':
            current_state = 'Stopped'
        else:
            current_state = 'Unknown'

        return jsonify({'response': f'Command processed: {command}', 'state': current_state})
    else:
        # If no command is provided, return the current state
        return jsonify({'state': current_state})

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8000)

