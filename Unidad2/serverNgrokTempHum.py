from http.server import BaseHTTPRequestHandler, HTTPServer
import json
import os

contador = 11
led = False
temperatura = 0.0
humedad = 0.0

class MyHTTPRequestHandler(BaseHTTPRequestHandler):

  def _set_response(self, content_type="text/plain", status_code=200):
    self.send_response(status_code)
    self.send_header("Content-type", content_type)
    self.end_headers()

  def throw_custom_error(self, message, status_code=400):
    self._set_response("application/json", status_code)
    self.wfile.write(json.dumps({"message": message}).encode())

  def do_GET(self):
    global led

    if self.path == "/":
      try:
        self._set_response(content_type="text/html")
        html_file_path = os.path.abspath("index2.html")
        with open(html_file_path, "r", encoding="utf-8") as file_to_open:
          self.wfile.write(file_to_open.read().encode())
      except Exception as e:
        print(f"Error: {e}")
        self.wfile.write(f"Error: {e}".encode())
    elif self.path == "/counter":
      self._set_response()
      self.wfile.write(json.dumps({"contador": contador}).encode())
    elif self.path == "/led":
      self._set_response()
      self.wfile.write(json.dumps({"status": led}).encode())
    elif self.path == "/led/on":
      led = True
      self._set_response()
      self.wfile.write(json.dumps({"status": led}).encode())
    elif self.path == "/led/off":
      led = False
      self._set_response()
      self.wfile.write(json.dumps({"status": led}).encode())
    elif self.path == "/temperatura":
      self._set_response()
      self.wfile.write(json.dumps({"temperatura": temperatura}).encode())
      pass
    elif self.path == "/humedad":
      self._set_response()
      self.wfile.write(json.dumps({"humedad": humedad}).encode())
      pass
    else:
      self.throw_custom_error("Invalid path")

  def do_POST(self):
    content_length = int(self.headers["Content-Length"])
    post_data = self.rfile.read(content_length)

    try:
      body_json = json.loads(post_data.decode())
    except:
      self.throw_custom_error("Invalid JSON")
      return

    global contador, temperatura, humedad

    # Agrega las nuevas rutas
    if self.path == '/temperatura':
      # Implementar la lógica para manejar solicitudes POST a la ruta /temperatura
      if "temperatura" in body_json:
        try:
          temperatura = float(body_json["temperatura"])
          self._set_response()
          self.wfile.write(json.dumps({"message": "Temperature updated", "temperatura": temperatura}).encode())
        except ValueError:
          self.throw_custom_error("Invalid temperature value")
      else:
        self.throw_custom_error("Missing 'temperatura' field in JSON")
    elif self.path == '/humedad':
      # Implementar la lógica para manejar solicitudes POST a la ruta /humedad
      if "humedad" in body_json:
        try:
          humedad = float(body_json["humedad"])
          self._set_response()
          self.wfile.write(json.dumps({"message": "Humidity updated", "humedad": humedad}).encode())
        except ValueError:
          self.throw_custom_error("Invalid humidity value")
      else:
        self.throw_custom_error("Missing 'humedad' field in JSON")
    elif self.path == '/counter':
      # Implementar la lógica para manejar solicitudes POST a la ruta /counter
      if body_json.get('action') is None or body_json.get('quantity') is None:
        self.throw_custom_error("Missing action or quantity")
        return

      if body_json['action'] != 'asc' and body_json['action'] != 'desc':
        self.throw_custom_error("Invalid action")
        return

      try:
        quantity = int(body_json['quantity'])
      except ValueError:
        self.throw_custom_error("Invalid quantity")
        return

      if body_json['action'] == 'asc':
        contador += quantity
      elif body_json['action'] == 'desc':
        contador -= quantity

      # Respuesta predeterminada para todas las rutas POST
      response_data = json.dumps({"message": "Received POST data, new value: " + str(contador), "status": "OK"})
      self._set_response("application/json")
      self.wfile.write(response_data.encode())
    else:
      self.throw_custom_error("Invalid path")

def run_server(server_class=HTTPServer, handler_class=MyHTTPRequestHandler, port=7800):
  server_address = ("", port)
  httpd = server_class(server_address, handler_class)
  print(f"Starting server on port {port}...")
  httpd.serve_forever()

if __name__ == "__main__":
  run_server()