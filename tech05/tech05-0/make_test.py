text = "abcde\nabc\ndeabc\nabc"
byte_text = str.encode(text)
with open("text.txt", 'wb') as f:
	f.write(byte_text)
print(type(text))
print(type(byte_text))
