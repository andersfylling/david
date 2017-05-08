import sys
sys.path.insert(0, '../../../external/pgnToFen')
import pgntofen

converter = pgntofen.PgnToFen()
file = "./test/Carlsen.pgn"

stats = converter.pgnFile(file)

output = open("fenstring.txt", "w")

items = stats['succeeded'][0][1]

for x in range(0, len(items), 3):
    output.write(items[x]);
    output.write('\n')

output.close()


