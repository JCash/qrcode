import sys, os, subprocess

REF_DECODER='build/zxing'
ENCODER='../build/qrencode'
if sys.platform == 'win32':
    REF_DECODER+='.exe'
    ENCODER+='.exe'

def run(cmd, cwd=None):
    p = subprocess.Popen(cmd.split(), stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=cwd)
    p.wait()
    if p.returncode != 0:
        return None
    return p.stdout.read() + ' ' + p.stderr.read()

def reference_decode(srcimage):
    # The reference tool for decoding images into text
    s = run('%s %s' % (REF_DECODER, srcimage))
    if s.endswith('\n '):
        s = s[:-2]
        return s

def encode(text, dstimage):
    # The reference tool for decoding images into text
    return run('%s %s -o %s' % (ENCODER, text, dstimage))


tests = [
    "HELLO WORLD",
    "0123456789",
]

def run_tests():

    if not os.path.exists(REF_DECODER):
        print("Could not find %s" % REF_DECODER)
        return 1

    if not os.path.exists(ENCODER):
        print("Could not find %s" % ENCODER)
        return 1

    for i, text in enumerate(tests):
        print("INPUT: '%s'" % text)
        img = os.path.join('build', 'test_%02d.png' % i)
        encode(text, img)
        s = reference_decode(img)
        if text != s:
            if s is None:
                print("FAILED %d: Failed to decode image: %s" % (i, img))
            else:
                print("FAILED %d: Decoded text differs from source!" % i)
                print("\tsource:  '%s'" % text)
                print("\tdecoded: '%s'" %  s)
        else:
            print("OUTPUT: '%s'" % s)



if __name__ == '__main__':
    run_tests()