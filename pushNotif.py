import time
import boto3

# Pushing notifications to User
def sendNotifications(message):
  access_key = 'AKIA5QGOG4BAVSIS4GHD'
  secret_key = '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'
  region = 'ap-southeast-2'
  topic_arn = 'arn:aws:sns:ap-southeast-2:928145399873:Push-Notifications-IOT'
  client = boto3.client('sns',aws_access_key_id= access_key, aws_secret_access_key = secret_key,region_name = region)
  response = client.publish(TopicArn = topic_arn, Message = message)
  print(response)
  print(message)

# Sending data to AWS
def sendData(data) :
  one = 1 # Add Code Here

# Continuously checks the sendNotifications file for any new notifications
def loop():
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

        try:
            sentData = False
            # Opens and read the sendData file
            file = open('sendData.txt', 'r')
            data = file.read()
            # If there's something in the sendNotifications file
            if (len(notifications) != 0) :
                sendData(data)
                sentData = True
        finally:
            file.close()
            # If data has been sent, clear the sendData file
            if (sentData) :
                try:
                    file = open('sendData.txt', 'w')
                finally:
                    file.close()
        
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
