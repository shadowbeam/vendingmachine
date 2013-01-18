import cherrypy
import json
import os
import serial

PATH = os.path.abspath(os.path.dirname(__file__))

class VendingMachine(object):
	def stock(self, id=None):
		return self.arduino('stock', id)
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
      ser = serial.Serial('COM3', timeout=5)
      ser.readline()
      ser.write(cmd)
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

cherrypy.engine.start()
cherrypy.engine.block()
