"""if someone has some bad key generation and some keys have the same divisor, then it's very easy to find that divisor.
"""



def gcd(a, b):
    if b == 0:
        return a
    return gcd(b, a % b)

keys = [1196311, 1250759, 1362733, 1462991, 1509349]

for i, key in enumerate(keys):
    for key2 in keys[i + 1:]:
        print(key, key2, gcd(key,key2))

