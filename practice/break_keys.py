

"""
if your e is low, and your message short, m^e will be lower than n, and you can just take the 'e'th root of m to get the deciphered message.
"""

n = 6865653949821276403125067
c = 309717089812744704
e = 3

m = round(c ** (1/3))

print(m)


print(1/3)
print(15/3)

letters = ""
for i in range(0, len(str(m)), 2):
    digits = str(m)[i:i+2]
    letter = chr(int(digits))
    letters += letter

print(letters)
