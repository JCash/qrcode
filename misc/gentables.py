import os, sys

scriptpath = os.path.dirname(os.path.realpath(__file__))

datacodeblocks = [-1, -1, -1, -1,19,16,13,9,34,28,22,16,55,44,34,26,80,64,48,36,108,86,62,46,136,108,76,60,156,124,88,66,194,154,110,86,232,182,132,100,274,216,154,122,324,254,180,140,370,290,206,158,428,334,244,180,461,365,261,197,523,415,295,223,589,453,325,253,647,507,367,283,721,563,397,313,795,627,445,341,861,669,485,385,932,714,512,406,1006,782,568,442,1094,860,614,464,1174,914,664,514,1276,1000,718,538,1370,1062,754,596,1468,1128,808,628,1531,1193,871,661,1631,1267,911,701,1735,1373,985,745,1843,1455,1033,793,1955,1541,1115,845,2071,1631,1171,901,2191,1725,1231,961,2306,1812,1286,986,2434,1914,1354,1054,2566,1992,1426,1096,2702,2102,1502,1142,2812,2216,1582,1222,2956,2334,1666,1276]
numblocksingroup1 = [0, 0, 0, 0,1,1,1,1,1,1,1,1,1,1,2,2,1,2,2,4,1,2,2,2,2,4,4,4,2,4,2,4,2,2,4,4,2,3,4,4,2,4,6,6,4,1,4,3,2,6,4,7,4,8,8,12,3,4,11,11,5,5,5,11,5,7,15,3,1,10,1,2,5,9,17,2,3,3,17,9,3,3,15,15,4,17,17,19,2,17,7,34,4,4,11,16,6,6,11,30,8,8,7,22,10,19,28,33,8,22,8,12,3,3,4,11,7,21,1,19,5,19,15,23,13,2,42,23,17,10,10,19,17,14,29,11,13,14,44,59,12,12,39,22,6,6,46,2,17,29,49,24,4,13,48,42,20,40,43,10,19,18,34,20]
numblocksingroup2 = [0, 0, 0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,4,1,0,2,2,2,0,2,4,4,2,1,2,2,0,4,4,8,2,2,6,4,0,1,4,4,1,5,5,5,1,5,7,7,1,3,2,13,5,1,15,17,1,4,1,19,4,11,4,16,5,13,5,10,4,0,6,6,7,0,16,0,5,14,14,14,4,14,16,2,4,13,22,13,2,4,6,4,4,3,26,28,10,23,31,31,7,7,37,26,10,10,25,25,3,29,1,28,0,23,35,35,1,21,19,46,6,23,7,1,7,26,14,41,14,34,10,64,4,14,10,46,18,32,14,32,4,7,22,67,6,31,34,61]

def p(l):
    print('\t'),
    for v in l:
        print("%4d," % v),
    print('')


def splitarray(data):
    L = data[0::4]
    M = data[1::4]
    Q = data[2::4]
    H = data[3::4]
    p(L)
    p(M)
    p(Q)
    p(H)

def printarray(name, arr):
    print("static const uint32_t %s[] = {" % name)
    splitarray(arr)
    print("};\n")

def printsinglearray(name, arr):
    print("static const uint32_t %s[] = {" % name)
    p(arr)
    print("};\n")

def make_capacity_array(index):
    with open(os.path.join(scriptpath, 'charactercapacities.txt'), 'rb') as f:
        lines = f.readlines()

    l = [0, 0, 0, 0]
    for line in lines:
        if line is None or line[0] == '#':
            continue

        tokens = line.strip().split()
        if len(tokens) == 6:
            tokens = tokens[1:]

        l.append(int(tokens[index]))
    return l


def make_block_count_array(index):
    with open(os.path.join(scriptpath, 'blockcount.txt'), 'rb') as f:
        lines = f.readlines()

    l = [0, 0, 0, 0]
    for line in lines:
        if line is None or line[0] == '#':
            continue

        tokens = line.strip().split()
        if len(tokens) == 6:
            tokens = tokens[1:]

        l.append(int(tokens[index]))
    return l

def make_character_count_array():
    numeric         = [10, 12, 14]
    alphanumeric    = [9, 11, 13]
    byte            = [8, 16, 16]
    kanji           = [8, 10, 12]

    n = [0] + [numeric[0]] * 9      + [numeric[1]] * 17         + [numeric[2]] * 14
    a = [0] + [alphanumeric[0]] * 9 + [alphanumeric[1]] * 17    + [alphanumeric[2]] * 14
    b = [0] + [byte[0]] * 9         + [byte[1]] * 17            + [byte[2]] * 14
    k = [0] + [kanji[0]] * 9        + [kanji[1]] * 17           + [kanji[2]] * 14

    l = [ x for t in zip(n, a, b, k) for x in t]

    return l

def reverse(s):
    ss = ''
    for c in reversed(s):
        ss += c
    return ss

def make_format_strings_array():
    with open(os.path.join(scriptpath, 'formatstrings.txt'), 'rb') as f:
        lines = f.readlines()

    l = []
    for line in lines:
        if line is None or line[0] == '#':
            continue

        tokens = line.strip().split()
        l.append( int('0b'+tokens[2], 2) )

    a = l[0:8]
    b = l[8:16]
    c = l[16:24]
    d = l[24:32]
    ll = []
    for i in range(0, 8):
        ll.append(a[i])
        ll.append(b[i])
        ll.append(c[i])
        ll.append(d[i])


    return ll


def make_version_strings_array():
    with open(os.path.join(scriptpath, 'versionstrings.txt'), 'rb') as f:
        lines = f.readlines()

    l = [0]*7
    for line in lines:
        if line is None or line[0] == '#':
            continue

        tokens = line.strip().split()
        l.append( int('0b'+tokens[1], 2) )
    return l

#printarray('JC_QRE_DATA_CODEWORD_COUNT', make_block_count_array(1))
#printarray('JC_QRE_ERROR_CORRECTION_CODEWORD_COUNT', make_block_count_array(2))
#printarray('JC_QRE_BLOCK_COUNT_GROUP1', make_block_count_array(3))
#printarray('JC_QRE_BLOCK_COUNT_GROUP2', make_block_count_array(5))
#printarray('JC_QRE_CODEWORDS_PER_BLOCK_GROUP1', make_block_count_array(4))
#printarray('JC_QRE_CODEWORDS_PER_BLOCK_GROUP2', make_block_count_array(6))

#printarray('JC_QRE_CAPACITY_NUMERIC', make_capacity_array(1))
#printarray('JC_QRE_CAPACITY_ALPHANUMERIC', make_capacity_array(2))
#printarray('JC_QRE_CAPACITY_BYTE', make_capacity_array(3))
#printarray('JC_QRE_CAPACITY_KANJI', make_capacity_array(4))
#printarray('JC_QRE_CHARACTER_COUNT_BIT_SIZE', make_character_count_array())

printarray('JC_QRE_FORMAT_BITS', make_format_strings_array())
printsinglearray('JC_QRE_VERSION_BITS', make_version_strings_array())

