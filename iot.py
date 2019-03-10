# -*- coding: utf-8 -*-

import json
import boto3

iot = boto3.client('iot-data')


def publish(device_name, state):
    if device_name == "smartLock":
        topic = '$aws/things/raspberryPi/shadow/update'
    else:
        topic = '$aws/things/LightController/shadow/update'
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
    if device_name == "smartLock":
        state = iot.get_thing_shadow(thingName="raspberryPi")
    else:
        state = iot.get_thing_shadow(thingName="LightController")

    payload = state["payload"].read().decode('utf-8')
    shadow = json.loads(payload)
    return shadow["state"]["reported"][device_name]
