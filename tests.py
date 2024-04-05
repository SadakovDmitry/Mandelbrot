with open("/Users/dima/Documents/DimaDos/Mondelbrot/tests.txt", 'r') as file:
    fps = list(float(i) for i in file.readlines())

sum = 0
count = 0
for i in fps:
    sum += i
    count += 1

with open("/Users/dima/Documents/DimaDos/Mondelbrot/accuracy.txt", 'a') as file:
    file.write(str(sum / count))
    file.write("\n")
print(sum / count)
