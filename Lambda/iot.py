# -*- coding: utf-8 -*-

import json
import boto3

iot = boto3.client('iot-data')
with open('config.json') as f:
    config = json.load(f)
    things_name = config['thingsName']
    
def publish(device_name, state):
    topic = f'$aws/things/{things_name}/shadow/update'
    payload = {
        "state": {
            "desired": {
            }
        }
    }
    payload["state"]["desired"][device_name] = state
    iot.publish(
        topic=topic,
        qos=0,
        payload=json.dumps(payload, ensure_ascii=False)
    )
    return True


def read_state(device_name):
    state = iot.get_thing_shadow(thingName=things_name)

    payload = state["payload"].read().decode('utf-8')
    shadow = json.loads(payload)
    return shadow["state"]["reported"][device_name]
