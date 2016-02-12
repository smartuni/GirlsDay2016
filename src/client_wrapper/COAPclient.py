import asyncio
import logging
import signal
import sys
from aiocoap import *

logging.basicConfig(level=logging.INFO)


@asyncio.coroutine
def __request(uri, myMessage, method):
	logging.debug("started request coroutine")
	protocol = yield from Context.create_client_context()
	request = Message(code=method)
	if (myMessage != ""):
		request = Message(code=method, payload=myMessage.encode("utf8"))
	request.set_request_uri(uri)
		
	try:
		response = yield from protocol.request(request).response
	except Exception as e:
		print('Failed to fetch resource:')
		print(e)
	else:
		#print('Result: %s\n%r'%(response.code, response.payload))
		return response.payload
	

#"coap://localhost/hello"
def get(uri, message=""):
	logging.debug("started GET method with uri: "+uri)
	response=asyncio.get_event_loop().run_until_complete(__request(uri, message , GET))
	return response

def post(uri, message):
	logging.debug("started POST method with uri: "+uri)
	response=asyncio.get_event_loop().run_until_complete(__request(uri, message, POST))
	return response

def put(uri, message):
	logging.debug("started PUT method with uri: "+uri)
	response=asyncio.get_event_loop().run_until_complete(__request(uri, message, PUT))
	return response

def delete(uri, message):
	logging.debug("started DELETE method with uri: "+uri)
	response=asyncio.get_event_loop().run_until_complete(__request(uri, message, DELETE))
	return response
	
# a quick and dirty graceful leave - emptying event loop

def __signal_handler(signal, frame):
	log.DEBUG('Received SIGINT - deleting event loop now')
	asyncio.get_event_loop().stop()
	asyncio.get_event_loop().close()
	sys.exit(0)
	
signal.signal(signal.SIGINT, __signal_handler)




