import numpy as np
import math

with open("/Users/dima/Documents/DimaDos/Mondelbrot/accuracy.txt", 'r') as file:
    time = list(float(i) for i in file.readlines())

print(math.sqrt(np.var(time)))
