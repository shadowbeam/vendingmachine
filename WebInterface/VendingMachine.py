import cherrypy
import json
import os
import serial

PATH = os.path.abspath(os.path.dirname(__file__))

class VendingMachine(object):
	def stock(self, id=None):
		response = self.arduino('stock', id)
		print response
		return response
	stock.exposed = True	
	
	def vend(self, id=None):
		return self.arduino('vend', id)
	vend.exposed = True
	
	def addcredit(self, id=None):
		return self.arduino('addcredit', id)
	addcredit.exposed = True
	
	def arduino(self, cmd, id):
		try:
			request = json.dumps({'cmd':cmd, 'id':int(id)})
			ser = serial.Serial('/dev/ttyACM0', timeout=10)
			ser.readline()
			ser.write(request)
			response = ser.readline()
			ser.close()
			if (response == ''):
				return '{"err":"Connection failed"}'
			return response
		except (TypeError, ValueError) as e:
			return '("err":"Invalid id"}'
	
  
cherrypy.tree.mount(VendingMachine(), '/', config={
	'/': {
		'tools.staticdir.on': True,
		'tools.staticdir.dir': PATH,
		'tools.staticdir.index': 'index.html',
	},
})

cherrypy.server.socket_host = '0.0.0.0'
if 'geteuid' not in dir(os) or os.geteuid() == 1:
  cherrypy.server.socket_port = 80

cherrypy.engine.start()
cherrypy.engine.block()
