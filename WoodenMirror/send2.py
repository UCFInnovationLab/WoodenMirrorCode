import serial
import time

# Configure the serial connection
ser = serial.Serial(
    port='COM7',       # Replace with your serial port (e.g., 'COM3' on Windows, '/dev/ttyUSB0' on Linux)
    baudrate=115200,     # Baud rate (must match the device you're communicating with)
    timeout=1          # Read timeout (optional)
)

# Check if the serial port is open
if ser.is_open:
    print("Serial port is open.")
else:
    print("Opening serial port...")
    ser.open()

def red():
	ser.write(b'\x30')
	time.sleep(.01)
	ser.write(b'\x00')  
	time.sleep(.01)
	ser.write(b'\x00')  
	time.sleep(.01)
	
def green():
	ser.write(b'\x00')
	time.sleep(.01)
	ser.write(b'\x30')  
	time.sleep(.01)
	ser.write(b'\x00')  
	time.sleep(.01)

def blue():
	ser.write(b'\x00')
	time.sleep(.01)
	ser.write(b'\x00')  
	time.sleep(.01)
	ser.write(b'\x30')  
	time.sleep(.01)
      
def white():
	ser.write(b'\x30')
	time.sleep(.01)
	ser.write(b'\x30')  
	time.sleep(.01)
	ser.write(b'\x30')  
	time.sleep(.01)


# Function to send characters
def send_characters(data):
    ser.write(data.encode())    # Encode the string into bytes and send it through the serial port
    print(f"Sent: {data}")

try:
	while True:
		red()
		green()
		blue()
		white()
        
		time.sleep(1)
        
		white()
		red()
		green()
		blue()
        
		time.sleep(1)
        
		blue()
		white()
		red()
		green()


		time.sleep(1)

		green()
		blue()
		white()
		red()
		
		time.sleep(1)

		

        
except KeyboardInterrupt:
    print("Program interrupted by the user.")
finally:
    ser.close()  # Close the serial port when done
    print("Serial port closed.")
