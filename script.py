import boto3


access_key = 'AKIA2ITYZKRINIP3IR4D'
secret_key = 'iVuO2MAUTjf/KcTdoI35uoPHnoEM6ruAplJP5Elk'
region = 'ap-southeast-2'
message = 'Please shut your windows'
topic_arn = 'arn:aws:sns:ap-southeast-2:705701696592:Pushing-Notifications'

client = boto3.client('sns',aws_access_key_id=access_key,aws_secret_access_key=secret_key, region_name=region)

response = client.publish(TopicArn=topic_arn, Message=message)

print(response) 
