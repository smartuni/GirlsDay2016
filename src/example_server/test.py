from ../client_wrapper/COAPclient import *
import time

logging.basicConfig(level=logging.DEBUG)

for x in range(0,10):
	try:
		bla=get("coap://localhost/plant")
		print(str(bla))
		time.sleep(3)
	except:
		print(e)
