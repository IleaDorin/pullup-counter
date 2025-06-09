# Pull-up Counter

A system that counts pull-ups using an Arduino Uno WiFi with an ultrasonic sensor and sends the data to a Flask server running on EC2.

## Project Structure

```
pullup-counter/
├── arduino/                 # Arduino code
│   └── pullup_counter.ino   # Main Arduino sketch
├── server/                  # Flask server code
│   ├── app.py              # Main Flask application
│   ├── requirements.txt    # Python dependencies
│   └── config.py           # Configuration settings
└── README.md               # This file
```

## Hardware Requirements

- Arduino Uno WiFi
- HC-SR04 Ultrasonic Sensor
- Jumper wires
- Power supply for Arduino

## Software Requirements

### Arduino
- Arduino IDE
- WiFiNINA library

### Server
- Python 3.8+
- Flask
- Other dependencies listed in requirements.txt

## Setup Instructions

1. Arduino Setup:
   - Connect the ultrasonic sensor to the Arduino
   - Upload the pullup_counter.ino sketch
   - Configure WiFi credentials in the sketch

2. Server Setup:
   - Install Python dependencies: `pip install -r requirements.txt`
   - Configure server settings in config.py
   - Run the server: `python app.py`

## How it Works

1. The ultrasonic sensor detects when you're doing a pull-up by measuring the distance
2. The Arduino counts the reps and sends the data to the Flask server
3. The server stores the data and provides an API to access it

## API Endpoints

- `POST /api/reps` - Add new rep count
- `GET /api/reps` - Get all rep counts
- `GET /api/reps/today` - Get today's rep count