# -*- coding: utf-8 -*-

# Copyright 2017 Amazon.com, Inc. or its affiliates. All Rights Reserved.
#
# Licensed under the Amazon Software License (the "License"). You may not use this file except in
# compliance with the License. A copy of the License is located at
#
#    http://aws.amazon.com/asl/
#
# or in the "license" file accompanying this file. This file is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, express or implied. See the License for the specific
# language governing permissions and limitations under the License.

"""Alexa Smart Home Lambda Function Sample Code.

This file demonstrates some key concepts when migrating an existing Smart Home skill Lambda to
v3, including recommendations on how to transfer endpoint/appliance objects, how v2 and vNext
handlers can be used together, and how to validate your v3 responses using the new Validation
Schema.

Note that this example does not deal with user authentication, only uses virtual devices, omits
a lot of implementation and error handling to keep the code simple and focused.
"""

import logging
import time
import json
import uuid
import iot

# Imports for v3 validation
from validation import validate_message

# Setup logger
logger = logging.getLogger()
logger.setLevel(logging.INFO)

DEVICES_CAPABILITY = [
    {
        "endpointId": "lightUp",
        "friendlyName": "窓側の電気",
        "description": "窓側の電気のスイッチを操作",
        "manufacturerName": "マイコン研究会IoT実行委員会",
        "displayCategories": [
            "LIGHT"
        ],
        "cookie": {
            "extraDetail1": "optionalDetailForSkillAdapterToReferenceThisDevice"
        },
        "capabilities": [
            {
                "type": "AlexaInterface",
                "interface": "Alexa.PowerController",
                "version": "3",
                "properties": {
                    "supported": [{
                        "name": "powerState"
                    }],
                    "proactivelyReported": True,
                    "retrievable": True
                }
            },
            {
                "type": "AlexaInterface",
                "interface": "Alexa",
                "version": "3"
            }
        ]
    },
    {
        "endpointId": "lightDown",
        "friendlyName": "廊下側の電気",
        "description": "廊下側の電気のスイッチを操作",
        "manufacturerName": "マイコン研究会IoT実行委員会",
        "displayCategories": [
            "LIGHT"
        ],
        "cookie": {
            "extraDetail1": "optionalDetailForSkillAdapterToReferenceThisDevice"
        },
        "capabilities": [
            {
                "type": "AlexaInterface",
                "interface": "Alexa.PowerController",
                "version": "3",
                "properties": {
                    "supported": [{
                        "name": "powerState"
                    }],
                    "proactivelyReported": True,
                    "retrievable": True
                }
            },
            {
                "type": "AlexaInterface",
                "interface": "Alexa",
                "version": "3"
            }
        ]
    },
    {
        "endpointId": "aircon",
        "friendlyName": "エアコン",
        "description": "エアコンを操作",
        "manufacturerName": "マイコン研究会IoT実行委員会",
        "displayCategories": [
            "SWITCH"
        ],
        "cookie": {},
        "capabilities": [
            {
                "type": "AlexaInterface",
                "interface": "Alexa.PowerController",
                "version": "3",
                "properties": {
                    "supported": [
                        {
                            "name": "powerState"
                        }
                    ],
                    "proactivelyReported": True,
                    "retrievable": True
                }
            },
            {
                "type": "AlexaInterface",
                "interface": "Alexa",
                "version": "3"
            }
        ]
    },
    {
        "endpointId": "smartLock",
        "friendlyName": "ドア",
        "description": "マイコン研究会IoT実行委員会のスマートロック",
        "manufacturerName": "マイコン研究会IoT実行委員会",
        "displayCategories": [
            "SMARTLOCK"
        ],
        "cookie": {

        },
        "capabilities": [
            {
                "type": "AlexaInterface",
                "interface": "Alexa.LockController",
                "version": "3",
                "properties": {
                    "supported": [
                        {
                            "name": "lockState"
                        }
                    ],
                    "proactivelyReported": True,
                    "retrievable": True
                }
            },
            {
                "type": "AlexaInterface",
                "interface": "Alexa",
                "version": "3"
            }
        ]
    }
]


def lambda_handler(request, context):
    """Main Lambda handler.

    Since you can expect both v2 and v3 directives for a period of time during the migration
    and transition of your existing users, this main Lambda handler must be modified to support
    both v2 and v3 requests.
    """

    try:
        logger.info("Directive:")
        # logger.info(json.dumps(request, indent=4, sort_keys=True))
        logger.info(request)
        version = get_directive_version(request)

        if version == "3":
            logger.info("Received v3 directive!")
            if request["directive"]["header"]["name"] == "Discover":
                response = handle_discovery_v3(request)
            else:
                response = handle_non_discovery_v3(request)

        logger.info("Response:")
        logger.info(response)
        # logger.info(json.dumps(response, indent=4, sort_keys=True))

        # if version == "3":
        # logger.info("Validate v3 response")
        # validate_message(request, response)

        return response
    except ValueError as error:
        logger.error(error)
        raise


def get_utc_timestamp(seconds=None):
    return time.strftime("%Y-%m-%dT%H:%M:%S.00Z", time.gmtime(seconds))


def get_uuid():
    return str(uuid.uuid4())


# v3 handlers
def handle_discovery_v3(request):
    response = {
        "event": {
            "header": {
                "namespace": "Alexa.Discovery",
                "name": "Discover.Response",
                "payloadVersion": "3",
                "messageId": get_uuid()
            },
            "payload": {
                "endpoints": DEVICES_CAPABILITY
            }
        }
    }
    return response


def handle_non_discovery_v3(request):
    request_namespace = request["directive"]["header"]["namespace"]
    request_name = request["directive"]["header"]["name"]
    request_device = request["directive"]["endpoint"]["endpointId"]

    if request_namespace == "Alexa.PowerController":
        if request_name == "TurnOn":
            value = "ON"
        else:
            value = "OFF"

        iot.publish(request_device, value)

        response = {
            "context": {
                "properties": [
                    {
                        "namespace": "Alexa.PowerController",
                        "name": "powerState",
                        "value": value,
                        "timeOfSample": get_utc_timestamp(),
                        "uncertaintyInMilliseconds": 500
                    }
                ]
            },
            "event": {
                "header": {
                    "namespace": "Alexa",
                    "name": "Response",
                    "payloadVersion": "3",
                    "messageId": get_uuid(),
                    "correlationToken": request["directive"]["header"]["correlationToken"]
                },
                "endpoint": {
                    "scope": {
                        "type": "BearerToken",
                        "token": "access-token-from-Amazon"
                    },
                    "endpointId": request_device
                },
                "payload": {}
            }
        }
        return response

    elif request_namespace == "Alexa.LockController":
        if request_name == "Unlock":
            value = "UNLOCKED"
        else:
            value = "LOCKED"

        iot.publish(request_device, value)

        response = {
            "context": {
                "properties": [{
                    "namespace": "Alexa.LockController",
                    "name": "lockState",
                    "value": value,
                    "timeOfSample": get_utc_timestamp(),
                    "uncertaintyInMilliseconds": 1000
                }]
            },
            "event": {
                "header": {
                    "namespace": "Alexa",
                    "name": "Response",
                    "payloadVersion": "3",
                    "messageId": get_uuid(),
                    "correlationToken": request["directive"]["header"]["correlationToken"]
                },
                "endpoint": {
                    "scope": {
                        "type": "BearerToken",
                        "token": "access-token-from-Amazon"
                    },
                    "endpointId": request_device
                },
                "payload": {}
            }
        }
        return response

    elif request_namespace == "Alexa":
        if request_name == "ReportState":
            response = {
                "context": {
                    "properties": [{
                        "namespace": "Alexa.LockController" if request_device == "smartLock" else "Alexa.PowerController",
                        "name": "lockState" if request_device == "smartLock" else "powerState",
                        "value": iot.read_state(request_device),
                        "timeOfSample": get_utc_timestamp(),
                        "uncertaintyInMilliseconds": 500
                    }]
                },
                "event": {
                    "header": {
                        "namespace": "Alexa",
                        "name": "StateReport",
                        "payloadVersion": "3",
                        "messageId": get_uuid(),
                        "correlationToken": request["directive"]["header"]["correlationToken"]
                    },
                    "endpoint": {
                        "endpointId": request_device
                    },
                    "payload": {}
                }
            }
            return response


    # other handlers omitted in this example


def get_directive_version(request):
    try:
        return request["directive"]["header"]["payloadVersion"]
    except:
        try:
            return request["header"]["payloadVersion"]
        except:
            return "-1"
