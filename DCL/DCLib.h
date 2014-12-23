// This file is distributed under a MIT license. See LICENSE.txt for details.

typedef unsigned char UInt8;
typedef unsigned short UInt16;
typedef short SInt16;
typedef unsigned long UInt32;

#define CMP_BUFFER_SIZE 36312L /* Work buffer size for imploding */
#define EXP_BUFFER_SIZE 12596L /* Work buffer size for exploding */
#define CMP_BINARY  0
#define CMP_ASCII  1
#define DICT_SIZE_1  1024
#define DICT_SIZE_2  2048
#define DICT_SIZE_4  4096

#define DCL_NO_ERROR 0L
#define DCL_ERROR_1  1L
#define DCL_ERROR_2  2L
#define DCL_ERROR_3  3L
#define DCL_ERROR_4  4L

typedef UInt32 read_data_proc(UInt8 *buffer, UInt16 *size, void *param);
typedef void write_data_proc(UInt8 *buffer, UInt16 *size, void *param);

extern const UInt8 dcl_table[];
extern UInt32 implode(read_data_proc read_data, write_data_proc write_data,
      UInt8 *work_buff, void *param, UInt16 type, UInt16 size);
extern UInt32  explode(read_data_proc read_data, write_data_proc write_data,
      UInt8 *work_buff, void *param);
extern UInt32  crc32(UInt8 *buffer, UInt32 size, UInt32 old_crc);

#ifdef __cplusplus
extern "C"
{
#endif

    extern void DCLUnpack(void *pdest, void *psrc, unsigned int *fnsize, int normalsize );

#ifdef __cplusplus
};
#endif
