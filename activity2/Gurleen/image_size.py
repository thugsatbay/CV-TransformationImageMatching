file = open("b657-wars.txt",'r')
lines = file.readlines()

x = []
y = []


for line in lines:
	words = line.split()
	x.append(int(words[0]))
	y.append(int(words[1]))

print (max(x))
print (max(y))