#ifndef _EFI_H_
#define _EFI_H_

#define EFIAPI __attribute__((ms_abi))

typedef unsigned long u64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef u64 usize;

typedef long i64;
typedef int i32;
typedef short i16;
typedef char i8;
typedef i64 isize;

typedef u8 bool;

const static bool true = 1;
const static bool false = 0;

typedef usize EfiStatus;
typedef void *EfiHandle;

typedef struct {
	u64 signature;
	u32 revision;
	u32 header_size;
	u32 crc32;
	u32 _reserved;
} EfiTableHeader;

struct _EfiSimpleTextOutputProtocol;

typedef struct _EfiSimpleTextOutputProtocol EfiSimpleTextOutputProtocol;

typedef EfiStatus (EFIAPI *EfiTextReset) (EfiSimpleTextOutputProtocol *this, bool extended_verification);
typedef EfiStatus (EFIAPI *EfiTextString) (EfiSimpleTextOutputProtocol *this, u16 *string);

struct _EfiSimpleTextOutputProtocol {
	EfiTextReset reset;
	EfiTextString output_string;
};

struct _EfiSimpleTextInputProtocol;

typedef struct _EfiSimpleTextInputProtocol EfiSimpleTextInputProtocol;

struct _EfiSimpleTextInputProtocol {

};

typedef struct {
	EfiTableHeader hdr;
	u16 *fw_vendor;
	u32 fw_revision;
	EfiHandle console_in_handle;
	EfiSimpleTextInputProtocol *con_in;
	EfiHandle console_out_handle;
	EfiSimpleTextOutputProtocol *con_out;
} EfiSystemTable;


#endif // _EFI_H_
