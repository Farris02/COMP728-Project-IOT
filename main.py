'''
The dependancies for the motor need to be downloaded here before starting the Python script:
https://pypi.org/project/adafruit-circuitpython-motorkit/
'''

import RPi.GPIO as GPIO
import datetime
import time
import boto3
from adafruit_motorkit import MotorKit

raindropSensor = 11 # Pin 11 / GPIO17 is the pin for the raindrop sensor
humitureSensor = 12 # Pin 12 / GPIO18 is the pin for the humiture sensor
infraredSensor = 13 # Pin 13 / GPIO27 is the pin for the infrared sensor
'''
relayOne = 15 # Pin 15 / GPIO22 is the pin for the first relay
relayTwo = 16 # Pin 16 / GPIO23 is the pin for the second relay
'''

# Selects the mode for the board
GPIO.setmode(GPIO.BOARD)

# Sets up the inputs and outputs
GPIO.setup(raindropSensor, GPIO.IN)
GPIO.setup(infraredSensor, GPIO.IN, pull_up_down=GPIO.PUD_UP)
'''
GPIO.setup(relayOne, GPIO.OUT)
GPIO.output(relayOne, GPIO.LOW)
GPIO.setup(relayTwo, GPIO.OUT)
GPIO.output(relayTwo, GPIO.LOW)
'''

# Sets up the motor kit
kit = MotorKit()

# Humiture Constants
MAX_UNCHANGE_COUNT = 100
STATE_INIT_PULL_DOWN = 1
STATE_INIT_PULL_UP = 2
STATE_DATA_FIRST_PULL_DOWN = 3
STATE_DATA_PULL_UP = 4
STATE_DATA_PULL_DOWN = 5

# Global variable to make sure that the motor isn't currently in use
motorInUse = False;

# Returns a 1 or a 0. 1 Being no rain and 0 being rain.
def raindropSensorResult():
    return GPIO.input(raindropSensor)

# Returns the humdity, humidity decimals, temperature, temperature decimals, and checksum
def humitureSensorResult():
    # Sets up the humiture sensor detection each time the function is called
    GPIO.setup(humitureSensor, GPIO.OUT)
    GPIO.output(humitureSensor, GPIO.HIGH)
    time.sleep(0.05)
    GPIO.output(humitureSensor, GPIO.LOW)
    time.sleep(0.02)
    GPIO.setup(humitureSensor, GPIO.IN, GPIO.PUD_UP)

    # Only breaks out of the loop if the state of the room stays the same for 100 loops
    unchanged_count = 0
    last = -1
    data = []
    while True:
        current = GPIO.input(humitureSensor)
        data.append(current)
        if last != current:
            unchanged_count = 0
            last = current
        else:
            unchanged_count += 1
            if unchanged_count > MAX_UNCHANGE_COUNT:
                break

    state = STATE_INIT_PULL_DOWN

    lengths = []
    current_length = 0

    for current in data:
        current_length += 1

        if state == STATE_INIT_PULL_DOWN:
            if current == GPIO.LOW:
                state = STATE_INIT_PULL_UP
            else:
                continue
        if state == STATE_INIT_PULL_UP:
            if current == GPIO.HIGH:
                state = STATE_DATA_FIRST_PULL_DOWN
            else:
                continue
        if state == STATE_DATA_FIRST_PULL_DOWN:
            if current == GPIO.LOW:
                state = STATE_DATA_PULL_UP
            else:
                continue
        if state == STATE_DATA_PULL_UP:
            if current == GPIO.HIGH:
                current_length = 0
                state = STATE_DATA_PULL_DOWN
            else:
                continue
        if state == STATE_DATA_PULL_DOWN:
            if current == GPIO.LOW:
                lengths.append(current_length)
                state = STATE_DATA_PULL_UP
            else:
                continue
    if len(lengths) != 40:
            # The humiture sensor is supposed to give 40 bits of information. Therefore, if the number of bits received is not 40, something has gone wrong.
            return False

    shortest_pull_up = min(lengths)
    longest_pull_up = max(lengths)
    halfway = (longest_pull_up + shortest_pull_up) / 2
    bits = []
    the_bytes = []
    byte = 0

    for length in lengths:
        bit = 0
        if length > halfway:
            bit = 1
        bits.append(bit)
    for i in range(0, len(bits)):
        byte = byte << 1
        if (bits[i]):
            byte = byte | 1
        else:
            byte = byte | 0
        if ((i + 1) % 8 == 0):
            the_bytes.append(byte)
            byte = 0
    checksum = (the_bytes[0] + the_bytes[1] + the_bytes[2] + the_bytes[3]) & 0xFF
    if the_bytes[4] != checksum:
        # If the checksum does not match, then the data has been corrupted.
        return False

    return the_bytes

# Returns a 1 or a 0. 1 Being nothing detected and 0 being something detected.
def infraredSensorResult():
    return GPIO.input(infraredSensor)

#Pushing notifications to User
def sendnotifications(message):
    access_key = 'AKIA2ITYZKRINIP3IR4D'
    secret_key = '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'
    region = 'ap-southeast-2'
    topic_arn = 'arn:aws:sns:ap-southeast-2:705701696592:Pushing-Notifications'

    client = boto3.client('sns',aws_access_key_id=access_key,aws_secret_access_key=>

    response = client.publish(TopicArn=topic_arn, Message=message)

    print(response)

# Open Window (Motor Shield)
def openWindow():
    if (!motorInUse):
        motorInUse = True
        kit.motor1.throttle = 0
        time.sleep(0.5)
        kit.motor1.throttle = 1.0
        time.sleep(5)
        kit.motor1.throttle = 0
        time.sleep(0.5)
        motorInUse = False

# Close Window (Motor Shield)
def closeWindow():
    if (!motorInUse):
        motorInUse = True
        kit.motor1.throttle = 0
        time.sleep(0.5)
        while (infraredSensorResult() != 0):
            kit.motor1.throttle = -1.0
        kit.motor1.throttle = 0
        time.sleep(0.5)
        motorInUse = False

'''
# Open Window (Relay)
def openWindow():
    if (!motorInUse):
        motorInUse = True
        GPIO.output(relayOne, GPIO.LOW)
        GPIO.output(relayTwo, GPIO.LOW)
        time.sleep(0.5)
        GPIO.output(relayOne, GPIO.HIGH)
        time.sleep(5)
        GPIO.output(relayOne, GPIO.LOW)
        motorInUse = False

# Close Window (Relay)
def closeWindow():
    if (!motorInUse):
        motorInUse = True
        GPIO.output(relayOne, GPIO.LOW)
        GPIO.output(relayTwo, GPIO.LOW)
        time.sleep(0.5)
        # While the infrared sensor doesn't detect the wall, keep closing the window
        while (infraredSensorResult() != 0):
            GPIO.output(relayTwo, GPIO.HIGH)
        GPIO.output(relayTwo, GPIO.LOW)
        motorInUse = False
'''

# Data Loop
def loop():
    printIt = False
    while True:
        raining = raindropSensorResult()
        humitureResult = humitureSensorResult()
        infraredResult = infraredSensorResult()
        currentTime = datetime.datetime.now()
        if (humitureResult) :
            humidity, humidityDecimal, temperature, temperatureDecimal, checkSum = humitureResult
            printIt = True
        if (printIt):
            print("Window Closed: " + str("Yes" if infraredResult == 0 else "No") + ", Raining: " + str("Yes" if raining == 0 else "No") + ", Humidity: " + str(humidity) + "." + str(humidityDecimal) + "%, Temperature: " + str(temperature) + "." + str(temperatureDecimal) + "'C, Date and Time: " + str(currentTime))
        time.sleep(1.1)

# Main
if __name__ == "__main__":
    try:
        loop()
    except KeyboardInterrupt:
        GPIO.cleanup()
