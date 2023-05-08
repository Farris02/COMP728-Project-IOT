import time
import boto3

#Pushing notifications to User
def sendNotifications(message):
    access_key = 'AKIA2ITYZKRINIP3IR4D'
    secret_key = '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'
    region = 'ap-southeast-2'
    topic_arn = 'arn:aws:sns:ap-southeast-2:705701696592:Pushing-Notifications'
    client = boto3.client('sns',aws_access_key_id=access_key,aws_secret_access_key=secret_key, region_name=region)
    response = client.publish(TopicArn=topic_arn, Message=message)
    print(response)
    print(message)

def loop():
    while True:
        file = open('sendNotifications.txt', 'r')
        sentNotifs = False
        try:
            notifications = file.readlines()
            if (len(notifications) != 0) :
                for line in notifications :
                    sendNotifications(line)
                    sentNotifs = True
        finally:
            file.close()
            if (sentNotifs) :
                try:
                    file = open('sendNotifications.txt', 'w')
                finally:
                    file.close()
            
        time.sleep(0.5)

if __name__ == "__main__":
    try:
        loop()
    except KeyboardInterrupt:
        one = 1 # Does nothing
