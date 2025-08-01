import sys

tokens = {"PV": 0, "PC": 1, "SX": 2, "SY": 3, "IX": 4, "IY": 5, "DX": 6, "DY": 7, "SV": 8, "IV": 9, "DV": 10, "RS": 11,
          "CR": 12, "GC": 13, "GV": 14, "XV": 15, "YV": 16, "JM": 17, "CJ": 18, "XX": 19, "JF": 20, "JB": 21, "CF": 22,
          "CB": 23, "GS": 24, "JR": 25, "RR": 26, "RC": 27, "BC": 28, "RN": 29, "US": 30, "DB": 31, "PS": 32, "+C": 33,
          "-C": 34, "IC": 35, "DC": 36, "SA": 37, "AV": 38, "PH": 39, "PL": 40, "XT": 41} # thanks past me

program = ""

with open(sys.argv[1]) as file:
    program = file.read()

mode = "not hex"

code = program.upper().strip().split()

out = bytearray()

for i in code:
    if i in tokens.keys():
        byte = tokens[i]
    
    else:
        try:
            if mode == "hex":
                raise ValueError
            
            byte = int(i)
        
        except ValueError:
            byte = int(i, base=16)
    
    out += byte.to_bytes()

with open("program.bin", "wb") as file:
    file.write(out)