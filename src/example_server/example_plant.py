import pyglet
from pyglet.window import mouse
import time
import threading
import logging  
import asyncio
import random
import aiocoap
import aiocoap.resource as resource

logging.basicConfig(level=logging.DEBUG)

mVal=50
mMax=100
mMin=0

l=threading.Lock()


window = pyglet.window.Window(width=500, height=500)
image_normal = pyglet.resource.image('plant-sun.gif')


def show_sun():
	global window, image_sun
	image_normal.blit(0,0)	
	window.dispatch_event('on_draw')

def chVal( blub ):
	global mVal, mMin, mMax
	global window, image_rain, image_sun
	global l
	
	newVal=mVal+blub
	if( (newVal > mMin) and (newVal < mMax )):
		l.acquire()
		mVal = newVal
		l.release()
		window.clear()
	
			


		


label = pyglet.text.Label(str(mVal),
							font_name='Arial',
							font_size=36,
							x=window.width//2, y=100,
							anchor_x='center', anchor_y='center', color=(0,0,0,500))

@window.event
def on_mouse_press(x, y, button, modifiers):
	if button == mouse.LEFT:
		logging.debug('The left mouse button was pressed.')
		chVal(10)
	elif button == mouse.RIGHT:
		chVal(-10)
		logging.debug('The right mouse button was pressed.')
	
	
@window.event
def on_draw():
	global mVal
	logging.debug("in on_draw")
	window.clear()
	image_normal.blit(0,0)
	label.text=str(mVal)
	label.draw()
	
	# returns a moisture value between 0 and 100
class Plant_server(aiocoap.resource.Resource):
	
	
	
	def __init__(self):
		super(Plant_server, self).__init__()


	@asyncio.coroutine
	def render_get(self, request):
		global mVal
		l.acquire()
		payload=str(mVal).encode('ascii')
		l.release()
		return aiocoap.Message(code=aiocoap.CONTENT, payload=payload)

def main():
	t=threading.Thread(target=pyglet.app.run)
	t.daemon=True
	t.start()
	root=aiocoap.resource.Site()
	root.add_resource(('.well-known', 'core'), resource.WKCResource(root.get_resources_as_linkheader))
	root.add_resource(("plant",) , Plant_server())
	asyncio.async(aiocoap.Context.create_server_context(root))
	asyncio.get_event_loop().run_forever()
	
	

if __name__=="__main__":
	main()
	
	

		

