import pigpio
from nrf24 import *
import time
import struct

print("Python NRF24 Simple Receiver Example.")

address = '1SNSR'

pi = pigpio.pi()
nrf = NRF24(pi, ce=25,payload_size=32, channel=0x76, data_rate=RF24_DATA_RATE.RATE_250KBPS, pa_level=RF24_PA.MAX, spi_speed=1000000)
nrf.set_address_bytes(len(address))
#nrf.open_reading_pipe(RF24_RX_ADDR.P1, address)

nrf.open_writing_pipe(address)
nrf.show_registers()

while True:
    nrf.reset_packages_lost()
    nrf.send("1000")
    while nrf.is_sending():
        time.sleep(0.0004)

    if nrf.get_packages_lost() == 0:
        print("Success")
    else:
        print("Error")
    print("data sent")
    time.sleep(1)
    #if nrf.get_packages_lost() == 0:

