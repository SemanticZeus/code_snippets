import pigpio
from nrf24 import *
import time

print("Python NRF24 Simple Receiver Example.")

address = '1SNSR'

pi = pigpio.pi()
nrf = NRF24(pi, ce=25, channel=0x76, data_rate=RF24_DATA_RATE.RATE_250KBPS, pa_level=RF24_PA.MAX, spi_speed=100000)
nrf.set_address_bytes(len(address))
nrf.open_reading_pipe(RF24_RX_ADDR.P1, address)

nrf.show_registers()

while True:
    while nrf.data_ready():
        payload = nrf.get_payload()    
        print(f"received = {payload}")
