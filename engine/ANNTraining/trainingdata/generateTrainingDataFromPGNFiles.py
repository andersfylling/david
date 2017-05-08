import sys


sys.path.insert(0, '../../../external/pgnToFen')
import pgntofen


pgnConverter = pgntofen.PgnToFen()
pgnConverter.resetBoard()

file = "PGNfiles/ficsgamesdb_201601_standard2000_nomovetimes_1462883.pgn"
stats =  pgnConverter.pgnFile(file)

output = open("fenstring.txt", "w")

items = stats['succeeded'][0][1]

for x in range(0, len(items), 3):
    output.write(items[x])
    output.write('\n')

output.close()


