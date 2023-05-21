import time
import boto3
import json

# Pushing notifications to User
def sendNotifications(message):
  access_key = 'AKIAU4CW534CO3WDPKVL'
  secret_key = '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'
  region = 'ap-southeast-2'
  topic_arn = 'arn:aws:sns:ap-southeast-2:335189761796:push_notifs'
  client = boto3.client('sns',aws_access_key_id= access_key, aws_secret_access_key = secret_key,region_name = region)
  response = client.publish(TopicArn = topic_arn, Message = message)
  print(response)
  print(message)

# Sending data to AWS
def sendData(file_path, channel_name):
  # Create an IoT Analytics client
  client = boto3.client('iotanalytics', region_name='ap-southeast-2', aws_access_key_id='AKIA5QGOG4BAVSIS4GHD',
                        aws_secret_access_key='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')

  # Open the text file for reading
  with open(file_path, 'r') as file:

      # Read each line in the text file
      for line in file:

          # Removes whitespace
          line = line.strip()

          # Split line into pairs
          pairs = line.split(',')

          # Creates a dictionary to store pairs
          payload = {}

          for pair in pairs:
              # Splits the pair
              sectors, value = pair.split(':')

              sectors = sectors.strip()
              value = value.strip()

              payload[sectors] = value

          payload_json = json.dumps(payload)

          # Send the message to AWS IoT Analytics
          response = client.batch_put_message(
              channelName=channel_name,
              messages=[
                  {
                      'messageId': '1',
                      'payload': payload_json.encode('utf-8')
                  }
              ]
          )

          print(response)

# Continuously checks the sendNotifications file for any new notifications
def loop():
    # Sets up the initial variables
    startTime = time.time() + 10 # 10 Second grace period to start the other scripts
    currentTime = startTime
    currentDay = 0

    while True:
        # Variable that keeps track if a notification has been sent this loop
        sentNotifs = False
        try:
            # Opens and read the sendNotifications file
            file = open('sendNotifications.txt', 'r')
            notifications = file.readlines()
            # If there's something in the sendNotifications file
            if (len(notifications) != 0) :
                # Send a notification for each line in the file
                for line in notifications :
                    sendNotifications(line)
                sentNotifs = True
        finally:
            file.close()
            # If a notification has been sent, clear the sendNotifications file
            if (sentNotifs) :
                try:
                    file = open('sendNotifications.txt', 'w')
                finally:
                    file.close()

        if (currentTime >= (startTime + 86400)) :
            startTime += 86400;
            sendData("SensorData_Day_" + str(currentDay) + ".txt", "data")
            currentDay += 1

        currentTime = time.time()

        time.sleep(0.5)

if __name__ == "__main__":
    try:
        # Setting up the sendNotifications file
        file = open('sendNotifications.txt', 'w')
        file.close()
        # Loops forever
        loop()
    except KeyboardInterrupt:
        one = 1 # Does nothing
