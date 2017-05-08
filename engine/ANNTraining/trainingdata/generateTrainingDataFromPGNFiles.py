import sys
sys.path.insert(0, '../../../external/pgnToFen')

import pgntofen

pgnConverter = pgntofen.PgnToFen()
pgnConverter.resetBoard()
file = "PGNfiles/ficsgamesdb_201601_standard2000_nomovetimes_1462883.pgn"
stats =  pgnConverter.pgnFile(file)

output = open("fenstring.txt", "w")

for lists in stats['succeeded']:
    for nlist in lists:
        for fenstring in nlist:
            output.write(fenstring)
            output.write('\n')

output.close()
