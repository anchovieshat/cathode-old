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
typedef EfiStatus (EFIAPI *EfiTextTestString) (EfiSimpleTextOutputProtocol *this, u16 *string);
typedef EfiStatus (EFIAPI *EfiTextQueryMode) (EfiSimpleTextOutputProtocol *this, usize mode_number, usize *columns, usize *rows);
typedef EfiStatus (EFIAPI *EfiTextSetMode) (EfiSimpleTextOutputProtocol *this, usize mode_number);
typedef EfiStatus (EFIAPI *EfiTextSetAttribute) (EfiSimpleTextOutputProtocol *this, usize attribute);
typedef EfiStatus (EFIAPI *EfiTextClearScreen) (EfiSimpleTextOutputProtocol *this);
typedef EfiStatus (EFIAPI *EfiTextSetCursorPosition) (EfiSimpleTextOutputProtocol *this, usize column, usize row);
typedef EfiStatus (EFIAPI *EfiTextEnableCursor) (EfiSimpleTextOutputProtocol *this, bool visible);

typedef struct {
	i32 max_mode;
	i32 mode;
	i32 attribute;
	i32 cursor_column;
	i32 cursor_row;
	bool cursor_visible;
} SimpleTextOutputMode;

struct _EfiSimpleTextOutputProtocol {
	EfiTextReset reset;
	EfiTextString output_string;
	EfiTextTestString test_string;
	EfiTextQueryMode query_mode;
	EfiTextSetMode set_mode;
	EfiTextSetAttribute set_attribute;
	EfiTextClearScreen clear_screen;
	EfiTextSetCursorPosition set_cursor_position;
	EfiTextEnableCursor enable_cursor;
	SimpleTextOutputMode *mode;
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
