import cherrypy
import json
import os
import serial

PATH = os.path.abspath(os.path.dirname(__file__))

class VendingMachine(object):
	def stock(self, id=None):
		if (id == None):
			return None
		cmd = json.dumps({'cmd':'stock', 'id':int(id)})
		return self.arduino(cmd)
	stock.exposed = True	
	
	def vend(self, id=None):
		if (id == None):
			return None
		cmd = json.dumps({'cmd':'vend', 'id':int(id)})
		return self.arduino(cmd)
	vend.exposed = True
	
	def arduino(self, cmd):
		ser = serial.Serial('COM3', timeout=5)
		ser.readline()
		ser.write(cmd)
		response = ser.readline()
		ser.close()
		
		if (response == ''):
			print(cmd)
			print(response)
			return 'No response'

		return response
	
cherrypy.tree.mount(VendingMachine(), '/', config={
        '/': {
                'tools.staticdir.on': True,
                'tools.staticdir.dir': PATH,
                'tools.staticdir.index': 'index.html',
            },
    })
	
cherrypy.engine.start()
cherrypy.engine.block()
