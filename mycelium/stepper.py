import pigpio
from time import sleep

halfStepSequence = (
  (1, 0, 0, 0),
  (1, 1, 0, 0),
  (0, 1, 0, 0),
  (0, 1, 1, 0),
  (0, 0, 1, 0),
  (0, 0, 1, 1),
  (0, 0, 0, 1),
  (1, 0, 0, 1)
)

fullStepSequence = (
  (1, 0, 0, 0),
  (0, 1, 0, 0),
  (0, 0, 1, 0),
  (0, 0, 0, 1),
)

pins = [17, 18, 27, 22]

pi = pigpio.pi()
for i in pins:
    pi.set_mode(i, pigpio.OUTPUT) 

try:
    for i in range(10000):
        for k in range(4):
            index = i % len(halfStepSequence)
            pi.write(pins[k], halfStepSequence[index][k])
        sleep(0.001)
except:
    for k in range(4):
        pi.write(pins[k], 0)
 
for k in range(4):
    pi.write(pins[k], 0)
 

