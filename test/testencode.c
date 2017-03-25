#define JC_TEST_IMPLEMENTATION
#include "jc_test.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <memory.h>

#define JC_QRENCODE_IMPLEMENTATION
#include "jc_qrencode.h"

#include <stdint.h>

typedef struct Context
{
    JCQRCode* code;
} Context;

static Context* qrencode_main_setup()
{
    return (Context*) malloc( sizeof(Context) );
}

static void qrencode_main_teardown(Context* ctx)
{
    (void)ctx;
}

static void test_setup(Context* ctx)
{
    ctx->code = 0;
}

static void test_teardown(Context* ctx)
{
    if( ctx->code )
        free(ctx->code);
}


static void qrencode_test_input_type(Context* ctx)
{
    (void)ctx;
    uint32_t type;

    const char* numbers = "09123";
    type = _jc_qre_guess_type((const uint8_t*)numbers, (uint32_t)strlen(numbers));
    ASSERT_EQ( JC_QRE_INPUT_TYPE_NUMERIC, type );

    const char* alpha = "ASDBOIUER";
    type = _jc_qre_guess_type((const uint8_t*)alpha, (uint32_t)strlen(alpha));
    ASSERT_EQ( JC_QRE_INPUT_TYPE_ALPHANUMERIC, type );

    const char* str1 = "ABCDaB";
    type = _jc_qre_guess_type((const uint8_t*)str1, (uint32_t)strlen(str1));
    ASSERT_EQ( JC_QRE_INPUT_TYPE_BYTE, type );
}

static uint32_t BitNeeded(uint32_t v)
{
    uint32_t bitsneeded = 1;
    for( uint32_t i = 1; i < sizeof(v)*8; ++i )
    {
        if( v & (1 << i) )
            bitsneeded = i+1;
    }
    return bitsneeded;
}

static void qrencode_test_bit_buffer(Context* ctx)
{
    (void)ctx;

/*
    uint8_t payload[128];
    uint32_t maxsize = (uint32_t)sizeof(payload);
    uint32_t totalnumbits = 0;
    uint32_t numbitswritten = 0;
    uint32_t numbitsread = 0;
    
    // 
    {
        memset(payload, 0, sizeof(payload));
        totalnumbits = 0;
        numbitswritten = 0;

        // value, num bits
        uint32_t values[] = {
            3, 0,
            2345, 0,
        };
        for( uint32_t i = 0; i < sizeof(values)/sizeof(values[0])/2; ++i ) {
            values[i*2+1] = BitNeeded(values[i*2]);
            totalnumbits += values[i*2+1];
        }

        for( uint32_t i = 0; i < sizeof(values)/sizeof(values[0])/2; ++i ) {
            _jc_qre_bitbuffer_write(payload, maxsize, &numbitswritten, (uint8_t*)&values[i*2+0], values[i*2+1]);
        }

        ASSERT_EQ( totalnumbits, numbitswritten );

        for( uint32_t i = 0; i < sizeof(values)/sizeof(values[0])/2; ++i ) {
            uint32_t value = _jc_qre_bitbuffer_read(payload, maxsize, &numbitsread, values[i*2+1]);
            ASSERT_EQ( values[i*2], value );
        }

        ASSERT_EQ( totalnumbits, numbitsread );
    }
*/
}

static void dbg_print(const uint8_t* buffer, uint32_t numbits)
{
    printf("encoded: ");
    for( uint32_t i = 0; i < numbits; ++i)
    {
        uint8_t currentvalue = buffer[i / 8];
        uint32_t srcindex = i & 0x7;
        uint32_t srcbit = (currentvalue & (1 << srcindex)) ? 1 : 0;

        printf("%d ", srcbit);
    }
    printf("\n");
}

static void dbg_coeff(const uint8_t* coefficients, uint32_t count)
{
    for( uint32_t i = 0; i < count; ++i)
    {
        printf("%d: %d\n", i, coefficients[i]);
    }
}

static void qrencode_test_encode_numeric(Context* ctx)
{
    (void)ctx;
    uint8_t payload[128];

/*
    JCQRCodeSegment seg;
    seg.data.bits = payload;
    seg.data.numbits = 0;

    const char* text = "8675309";
    uint32_t len = (uint32_t)strlen(text);
    uint32_t result = _jc_qre_encode_numeric(&seg, sizeof(payload), (const uint8_t*)text, len);

    ASSERT_EQ( len, result );
    ASSERT_EQ( len, seg.elementcount );

    uint32_t numbitsread = 0;
    uint32_t value = 0;

    value = _jc_qre_bitbuffer_read(seg.data.bits, sizeof(payload), &numbitsread, 10);
    ASSERT_EQ( 867, value );
    value = _jc_qre_bitbuffer_read(seg.data.bits, sizeof(payload), &numbitsread, 10);
    ASSERT_EQ( 530, value );
    value = _jc_qre_bitbuffer_read(seg.data.bits, sizeof(payload), &numbitsread, 4);
    ASSERT_EQ( 9, value );
*/

    //dbg_print(seg.data.bits, seg.data.numbits);
}


static void qrencode_test_encode_alphanumeric(Context* ctx)
{
    (void)ctx;

    uint8_t payload[128];
/*
    JCQRCodeSegment seg;
    seg.data.bits = payload;
    seg.data.numbits = 0;

    const char* text = "HELLO WORLD";
    uint32_t len = (uint32_t)strlen(text);
    uint32_t result = _jc_qre_encode_alphanumeric(&seg, sizeof(payload), (const uint8_t*)text, len);

    ASSERT_EQ( len, result );
    ASSERT_EQ( len, seg.elementcount );

    uint32_t numbitsread = 0;
    uint32_t value = 0;

    value = _jc_qre_bitbuffer_read(seg.data.bits, sizeof(payload), &numbitsread, 11);
    ASSERT_EQ( 779, value );
    //value = _jc_qre_bitbuffer_read(seg.data.bits, sizeof(payload), &numbitsread, 10);
    //ASSERT_EQ( 530, value );
    //value = _jc_qre_bitbuffer_read(seg.data.bits, sizeof(payload), &numbitsread, 4);
    //ASSERT_EQ( 9, value );

    //value = _jc_qre_bitbuffer_read(seg.data.bits, sizeof(payload), &numbitsread, 4);
    //ASSERT_EQ( 9, value );

*/
    //dbg_print(seg.data.bits, seg.data.numbits);
}


static void qrencode_test_rs_create(Context* ctx)
{
    (void)ctx;

    uint8_t coefficients[512];

    uint8_t reference[] = {
          1,   0,   0,   0,   0,   0,   0,
          3,   2,   0,   0,   0,   0,   0,
          7,  14,   8,   0,   0,   0,   0,
         15,  54, 120,  64,   0,   0,   0,
         31, 198,  63, 147, 116,   0,   0,
         63,   1, 218,  32, 227,  38,   0,
        127, 122, 154, 164,  11,  68, 117,    
    };

    for( uint32_t i = 0; i < 7; ++i )
    {
        uint32_t count = i+1;
        _jc_qre_rs_create_generator(count, coefficients);

        for( uint32_t j = 0; j < count; ++j)
        {
            ASSERT_EQ( reference[7 * i + j], coefficients[j] );
        }
    }
}

static void qrencode_test_rs_encode(Context* ctx)
{
    (void)ctx;
    // 5-Q: http://www.thonky.com/qr-code-tutorial/show-division-steps?msg_coeff=67%2C85%2C70%2C134%2C87%2C38%2C85%2C194%2C119%2C50%2C6%2C18%2C6%2C103%2C38&num_ecc_blocks=18
    uint8_t data[] = {67,85,70,134,87,38,85,194,119,50,6,18,6,103,38};
    uint8_t coefficients[512];
    _jc_qre_rs_create_generator(18, coefficients);

    uint8_t ec_codewords[64];
    _jc_qre_rs_encode(18, coefficients, 15, data, ec_codewords);

    uint8_t reference_ec_codewords[] = {213, 199, 11, 45, 115, 247, 241, 223, 229, 248, 154, 117, 154, 111, 86, 161, 111, 39};

    for( uint32_t i = 0; i < 18; ++i)
    {
        ASSERT_EQ( reference_ec_codewords[i], ec_codewords[i] );
    }
}

static void qrencode_test_error_correction(Context* ctx)
{
    (void)ctx;

    // http://www.thonky.com/qr-code-tutorial/structure-final-message
    const uint8_t data[] = {
        67,85,70,134,87,38,85,194,119,50,6,18,6,103,38,
        246,246,66,7,118,134,242,7,38,86,22,198,199,146,6,
        182,230,247,119,50,7,118,134,87,38,82,6,134,151,50,7,
        70,247,118,86,194,6,151,50,16,236,17,236,17,236,17,236
    };

    JCQRCodeInternal qri;
    qri.qrcode.ecl = JC_QRE_ERROR_CORRECTION_LEVEL_QUARTILE;
    qri.qrcode.version = 5;
    memcpy(qri.databuffer, data, sizeof(data));
    qri.datasize = sizeof(data);

    _jc_qre_calc_error_correction(&qri);

    const uint8_t reference_ec_codewords[] = {
        213,199,11,45,115,247,241,223,229,248,154,117,154,111,86,161,111,39,
        87,204,96,60,202,182,124,157,200,134,27,129,209,17,163,163,120,133,
        148,116,177,212,76,133,75,242,238,76,195,230,189,10,108,240,192,141,
        235,159,5,173,24,147,59,33,106,40,255,172,82,2,131,32,178,236
    };

    for( uint32_t i = 0; i < 4 * 18; ++i )
    {
        ASSERT_EQ( reference_ec_codewords[i], qri.errorcorrection[i] );
    }
}

static void qrencode_test_interleave(Context* ctx)
{
    (void)ctx;

    // http://www.thonky.com/qr-code-tutorial/structure-final-message
    const uint8_t data[] = {
        67,85,70,134,87,38,85,194,119,50,6,18,6,103,38,
        246,246,66,7,118,134,242,7,38,86,22,198,199,146,6,
        182,230,247,119,50,7,118,134,87,38,82,6,134,151,50,7,
        70,247,118,86,194,6,151,50,16,236,17,236,17,236,17,236
    };
    const uint8_t ec_codewords[] = {
        213,199,11,45,115,247,241,223,229,248,154,117,154,111,86,161,111,39,
        87,204,96,60,202,182,124,157,200,134,27,129,209,17,163,163,120,133,
        148,116,177,212,76,133,75,242,238,76,195,230,189,10,108,240,192,141,
        235,159,5,173,24,147,59,33,106,40,255,172,82,2,131,32,178,236
    };

    JCQRCodeInternal qri;
    qri.qrcode.ecl = JC_QRE_ERROR_CORRECTION_LEVEL_QUARTILE;
    qri.qrcode.version = 5;
    qri.datasize = sizeof(data);
    memcpy(qri.databuffer, data, sizeof(data));
    memcpy(qri.errorcorrection, ec_codewords, sizeof(ec_codewords));

    _jc_qre_interleave_codewords(&qri);

    const uint8_t reference_interleaved[] = {
        67, 246, 182, 70, 85, 246, 230, 247, 70, 66, 247, 118, 134, 7, 119, 86, 87, 118, 50, 194, 38, 134, 7, 6, 85, 242, 118, 151, 194, 7, 134, 50, 119, 38, 87, 16, 50, 86, 38, 236, 6, 22, 82, 17, 18, 198, 6, 236, 6, 199, 134, 17, 103, 146, 151, 236, 38, 6, 50, 17, 7, 236, 213, 87, 148, 235, 199, 204, 116, 159, 11, 96, 177, 5, 45, 60, 212, 173, 115, 202, 76, 24, 247, 182, 133, 147, 241, 124, 75, 59, 223, 157, 242, 33, 229, 200, 238, 106, 248, 134, 76, 40, 154, 27, 195, 255, 117, 129, 230, 172, 154, 209, 189, 82, 111, 17, 10, 2, 86, 163, 108, 131, 161, 163, 240, 32, 111, 120, 192, 178, 39, 133, 141, 236
    };

    for( uint32_t i = 0; i < sizeof(reference_interleaved); ++i )
    {
        ASSERT_EQ( reference_interleaved[i], qri.interleaved[i] );
    }
}

static int save_image(JCQRCode* qr, const char* path)
{
    int32_t size = qr->size;

    int32_t border = 2;
    int32_t scale = 8;
    int32_t newsize = scale*(size + 2 * border);
    uint8_t* large = (uint8_t*)malloc( newsize * newsize );

    memset(large, 255, newsize*newsize);

    for( int y = 0; y < size*scale; ++y )
    {
        for( int x = 0; x < size*scale; ++x )
        {
            uint8_t module = qr->data[(y/scale)*256 + (x/scale)];
            large[(y + scale*border) * newsize + x + scale*border] = module;
        }
    }

    int result = stbi_write_png(path, newsize, newsize, 1, large, newsize);
    free(large);
    
    if(result)
        printf("Wrote to %s\n", path);
    else
        printf("Failed to write to %s\n", path);
    return result;
}

static void qrencode_test_full(Context* ctx)
{
    (void)ctx;

    const char* text = "HELLO WORLD";
    uint32_t len = (uint32_t)strlen(text);

    JCQRCode* qr = jc_qrencode((const uint8_t*)text, len, JC_QRE_ERROR_CORRECTION_LEVEL_QUARTILE);
    save_image(qr, "test.png");

    free(qr);
}

TEST_BEGIN(qrencode_test, qrencode_main_setup, qrencode_main_teardown, test_setup, test_teardown)
    TEST(qrencode_test_input_type)
    TEST(qrencode_test_bit_buffer)
    TEST(qrencode_test_encode_numeric)
    TEST(qrencode_test_encode_alphanumeric)
    TEST(qrencode_test_rs_create)
    TEST(qrencode_test_rs_encode)
    TEST(qrencode_test_error_correction)
    TEST(qrencode_test_interleave)
    TEST(qrencode_test_full)
TEST_END(qrencode_test)


int main(int argc, const char** argv)
{
    (void)dbg_print;
    (void)dbg_coeff;

    (void)argc;
    (void)argv;
    
    RUN_ALL();
    return 0;
}
