
## qrcode
A simple C/C++ single file header only library to generate QR code images.

I made this encoder in order for it to be small, fast and easy to include in a a project.
It currently only supports regular QRCodes.

![50 points](test/images/helloworld.png)

## Table of Contents
1. [Branch Status](#branch-status)
1. [Usage](#usage)
1. [Build](#build)
1. [Tools](#tools)
    1. [qrencode](#qrencode)
1. [Downloads](#downloads)

## Branch Status <a name="branch-status"></a>

|Branch      |OSX/Linux |Windows   |
|------------|----------|----------|
|master      | [![Build Status][tr_img_master]][tr_master] | [![Build status][apv_img_master]][apv_master] |
|dev         | [![Build Status][tr_img_dev]][tr_dev] | [![Build status][apv_img_dev]][apv_dev] |


## Usage

Using the encoder is straight forward:

    #define JC_QRENCODE_IMPLEMENTATION
    #include "jc_qrencode.h"

    const char* text = "HELLO WORLD";
    JCQRCode* qr = jc_qrencode((const uint8_t*)text, (uint32_t)strlen(text));
    if( !qr )
    {
        fprintf(stderr, "Failed to encode text\n");
        return 1;
    }

    // Save a png file
    stbi_write_png("out.png", qr->size, qr->size, 1, qr->data, 256); // Stride is currently 256 bytes

    free(qr); // free the qr code


## Build <a name="build"></a>

### Build the tool

Building the tool (or library) is easy:

OSX/Linux (using clang)

    qrcode> ./compile_clang.sh

Windows (in a regular command prompt)

    qrcode> compile_cl.bat


## Tools <a name="tools"></a>

### qrencode <a name="qrencode" ></a>

You use it like so:

    > qrencode Hello World!
    TEXT: 'Hello World!'
    Wrote to out.png

    > qrencode -o out.png Hello World!

#### Usage

    > qrencode
    Usage: qrencode [options] [text]
        -i <inputfile>      A file containing text
        -o <outputfile.png>
        -w <width>          The output width of the image
        -h <height>         The output height of the image


## Downloads <a name="downloads"></a>

For those that don't want to build the qrencode tool themselves, I've included links to the latest builds from
CI.

|Branch      |OSX/Linux |Windows   |
|------------|----------|----------|
|master      |          |          |
|dev         |          |[qrencode.zip](https://ci.appveyor.com/api/projects/JCash/qrcode/artifacts/qrencode.zip?branch=dev)     |

## Reference material

I followed the excellent tutorial here

[www.thonky.com](http://www.thonky.com/qr-code-tutorial/introduction)

## Reed-Solomon error correction
[wikiversity](https://en.wikiversity.org/wiki/Reed%E2%80%93Solomon_codes_for_coders)
[https://github.com/qsantos](https://github.com/qsantos/qrcode/blob/master/rs.c)
[https://github.com/kazuhikoarase](https://github.com/kazuhikoarase/qrcode-generator/blob/master/java/src/java/com/d_project/qrcode/QRUtil.java)

## Generators
[Generator](https://keremerkan.net/qr-code-and-2d-code-generator/)


[tr_img_master]: https://travis-ci.org/JCash/qrcode.svg?branch=master
[tr_master]: https://travis-ci.org/JCash/qrcode?branch=master
[tr_img_dev]: https://travis-ci.org/JCash/qrcode.svg?branch=dev
[tr_dev]: https://travis-ci.org/JCash/qrcode?branch=dev
[apv_img_master]: https://ci.appveyor.com/api/projects/status/l43iuepu6q5mknxp/branch/master?svg=true
[apv_master]: https://ci.appveyor.com/project/JCash/qrcode/branch/master
[apv_img_dev]: https://ci.appveyor.com/api/projects/status/l43iuepu6q5mknxp/branch/dev?svg=true
[apv_dev]: https://ci.appveyor.com/project/JCash/qrcode/branch/dev