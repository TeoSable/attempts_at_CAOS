from random import randrange
with open("input.txt", 'wb') as f:
	for i in range(4095000):
		f.write(randrange(1024000).to_bytes(4, 'little', signed=True))
