from flask import Flask, render_template, request, jsonify


app = Flask(__name__)

@app.route('/', )
def home():
    data = request.json  # Assuming JSON payload from ESP32
    message = data.get('message', 'No message received')
    print(f"Received message: {message}")
    # Process the message as needed (e.g., save to DB, log, etc.)
    return jsonify({'status': 'success', 'received': message}), 200

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)  # Run on all interfaces, port 5000