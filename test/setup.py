import sys, os, subprocess

# https://www.the-qrcode-generator.com/
# 

ZXING_VERSION='00f634024ceeee591f54e6984ea7dd666fab22ae'
ZXING_URL='https://github.com/zxing/zxing/archive/%s.zip' % ZXING_VERSION
ZXING_ZIP='zxing.zip'
ZXING_EXE='build/zxing'

def run(cmd, cwd=None):
    p = subprocess.Popen(cmd.split(), cwd=cwd)
    p.wait()

def curl(url, path):
    cmd = 'curl -L %s -o %s' % (url,path)
    run(cmd)

def unzip(src, dst):
    cmd = 'unzip %s -d %s' % (src, dst)
    run(cmd)

def cp(src, dst):
    parentdir, _ = os.path.split(dst)
    if not os.path.exists(parentdir):
        os.makedirs(parentdir)
        
    cmd = 'cp %s %s' % (src, dst)
    run(cmd)


def install_zxing():
    if os.path.exists(ZXING_EXE):
        print("zxing already built")
        return
    if not os.path.exists(ZXING_ZIP):
        curl(ZXING_URL, ZXING_ZIP)
    if not os.path.exists("zxing"):
        unzip(ZXING_ZIP, "zxing")

    src = 'zxing/zxing-%s/cpp/build/zxing' % ZXING_VERSION
    if not os.path.exists(src):
        cmd = 'python scons/scons.py DEBUG=false zxing'
        run(cmd, 'zxing/zxing-%s/cpp' % ZXING_VERSION)

    cp(src, ZXING_EXE)

if __name__ == '__main__':

    if sys.platform == 'win32':
        print("")
        print("CL.EXE:")
        run('cl.exe test2.c')
        print("")

    install_zxing()
