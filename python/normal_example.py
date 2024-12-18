import random
from matplotlib import pyplot as plt
import numpy as np

sigma = 100
mean = 5
num_samples = 100000000
buckets = 1000
normal_distribution = np.zeros(shape=[buckets+1])
samples = np.random.normal(mean, sigma, num_samples)

for x in samples:
    index = int((x+buckets/2+1))
    if index <0 or index>=len(normal_distribution):
        continue
    normal_distribution[index]+=1


x = np.arange(0,buckets+1)

print(normal_distribution, x)
plt.bar(x, normal_distribution)
plt.show()
    

