import os, sys

scriptpath = os.path.dirname(os.path.realpath(__file__))

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

def printinterleavedarray(name, arr):
    print("static const uint32_t %s[] = {" % name)
    splitarray(arr)
    print("};\n")

def printarray(name, arr, num_items_per_row):
    print("static const uint32_t %s[] = {" % name)
    
    offset = 0
    numsteps = len(arr) // num_items_per_row
    for i in range(0, numsteps):
        p(arr[offset:offset+num_items_per_row])
        offset = offset + num_items_per_row

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

def make_alignment_positions_array():
    with open(os.path.join(scriptpath, 'alignmentpatterns.txt'), 'rb') as f:
        lines = f.readlines()

    max_size = 8 # add a trailing 0 for termination
    l = [0] * max_size * 2 # add two empty rows
    for line in lines:
        if line is None or line[0] == '#':
            continue

        tokens = line.strip().split()
        tokens = tokens[3:] # skip "QR Version 29"
        extra = max_size - len(tokens)
        tokens += [0] * extra
        l.extend( map(int, tokens) )
    return l


#printinterleavedarray('JC_QRE_DATA_CODEWORD_COUNT', make_block_count_array(1))
#printinterleavedarray('JC_QRE_ERROR_CORRECTION_CODEWORD_COUNT', make_block_count_array(2))
#printinterleavedarray('JC_QRE_BLOCK_COUNT_GROUP1', make_block_count_array(3))
#printinterleavedarray('JC_QRE_BLOCK_COUNT_GROUP2', make_block_count_array(5))
#printinterleavedarray('JC_QRE_CODEWORDS_PER_BLOCK_GROUP1', make_block_count_array(4))
#printinterleavedarray('JC_QRE_CODEWORDS_PER_BLOCK_GROUP2', make_block_count_array(6))

#printinterleavedarray('JC_QRE_CAPACITY_NUMERIC', make_capacity_array(1))
#printinterleavedarray('JC_QRE_CAPACITY_ALPHANUMERIC', make_capacity_array(2))
#printinterleavedarray('JC_QRE_CAPACITY_BYTE', make_capacity_array(3))
#printinterleavedarray('JC_QRE_CAPACITY_KANJI', make_capacity_array(4))
#printinterleavedarray('JC_QRE_CHARACTER_COUNT_BIT_SIZE', make_character_count_array())

#printinterleavedarray('JC_QRE_FORMAT_BITS', make_format_strings_array())
#printsinglearray('JC_QRE_VERSION_BITS', make_version_strings_array())

printarray('JC_QRE_ALIGNMENT_POSITIONS', make_alignment_positions_array(), 8)


