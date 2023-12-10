import paho.mqtt.client as mqtt #import the client1
import time

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("meduk2/capstone/#")

def on_message(client, userdata, message):
    print("message received " ,str(message.payload.decode("utf-8")))
    #print("message topic=",message.topic)
    #print("message qos=",message.qos)
    #print("message retain flag=",message.retain)
    f = open('meduk2-capstone-data.csv', 'a+')
    f.write(str(message.payload.decode("utf-8")))
    f.write('\n')
    f.close()

broker_address="192.168.1.68"
print("creating new instance")
client = mqtt.Client() #create new instance
client.on_connect = on_connect
client.on_message=on_message #attach function to callback
print("connecting to broker")
client.connect(broker_address) #connect to broker
#client.loop_start() #start the loop
print("Subscribing to topic","meduk2/capstone/#")
client.subscribe("meduk2/capstone/#")
client.loop_forever()
