#ifndef _EFI_H_
#define _EFI_H_

#define EFIAPI __attribute__((ms_abi))

#define EFI_SUCCESS 0
#define NULL ((void*)0)

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
typedef void *EfiEvent;
typedef usize EfiTpl;

typedef struct {
	u32 data1;
	u16 data2;
	u16 data3;
	u8 data4[8];
} EfiGuid;

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

typedef struct {
	u16 scan_code;
	u16 unicode_char;
} EfiInputKey;

struct _EfiSimpleTextInputProtocol;

typedef struct _EfiSimpleTextInputProtocol EfiSimpleTextInputProtocol;

typedef EfiStatus (EFIAPI *EfiInputReset) (EfiSimpleTextInputProtocol *this, bool extended_verification);
typedef EfiStatus (EFIAPI *EfiInputReadKey) (EfiSimpleTextInputProtocol *this, EfiInputKey *key);

struct _EfiSimpleTextInputProtocol {
	EfiInputReset reset;
	EfiInputReadKey read_key_stroke;
	EfiEvent wait_for_key;
};

typedef struct {
	u16 year;
	u8 month;
	u8 day;
	u8 hour;
	u8 minute;
	u8 second;
	u8 pad1;
	u32 nanosecond;
	i16 time_zone;
	u8 daylight;
	u8 pad2;
} EfiTime;

typedef struct {
	u32 resolution;
	u32 accuracy;
	bool sets_to_zero;
} EfiTimeCapabilities;

typedef enum {
	AllocateAnyPages,
	AllocateMaxAddress,
	AllocateAddress,
	MaxAllocateType
} EfiAllocateType;

typedef enum {
	EfiReservedMemoryType,
	EfiLoaderCode,
	EfiLoaderData,
	EfiBootServicesCode,
	EfiBootServicesData,
	EfiRuntimeServicesCode,
	EfiRuntimeServicesData,
	EfiConventionalMemory,
	EfiUnusableMemory,
	EfiACPIReclaimMemory,
	EfiACPIMemoryNVS,
	EfiMemoryMappedIO,
	EfiMemoryMappedIOPortSpace,
	EfiPalCode,
	EfiMaxMemoryType,
} EfiMemoryType;

typedef struct {
	u32 type;
	u64 physical_start;
	u64 virtual_start;
	u64 number_of_pages;
	u64 attributes;
} EfiMemoryDescriptor;

typedef enum {
	TimerCancel,
	TimerPeriodic,
	TimerRelative
} EfiTimerDelay;

typedef enum {
	EfiResetCold,
	EfiResetWarm,
	EfiResetShutdown,
	EfiResetPlatformSpecific
} EfiResetType;

typedef EfiStatus (EFIAPI *EfiGetTime) (EfiTime *time, EfiTimeCapabilities *capabilities);
typedef EfiStatus (EFIAPI *EfiSetTime) (EfiTime *time);
typedef EfiStatus (EFIAPI *EfiGetWakeupTime) (bool *enabled, bool *pending, EfiTime *time);
typedef EfiStatus (EFIAPI *EfiSetWakeupTime) (bool enabled, EfiTime *time);

typedef EfiStatus (EFIAPI *EfiSetVirtualAddressMap) (usize memory_map_size, usize descriptor_size, u32 descriptor_version, EfiMemoryDescriptor virtual_map);
typedef EfiStatus (EFIAPI *EfiConvertPointer) (usize debug_disposition, void **address);

typedef EfiStatus (EFIAPI *EfiGetVariable) (u16 *variable_name, EfiGuid *vendor_guid, u32 *attributes, usize *data_size, void *data);
typedef EfiStatus (EFIAPI *EfiGetNextVariableName) (usize *variable_name_size, u16 *variable_name, EfiGuid *vendor_guid);
typedef EfiStatus (EFIAPI *EfiSetVariable) (u16 *variable_name, EfiGuid *vendor_guid, u32 attributes, usize data_size, void *data);

typedef EfiStatus (EFIAPI *EfiGetNextHighMonoCount) (u32 *high_count);
typedef EfiStatus (EFIAPI *EfiResetSystem) (EfiResetType reset_type, EfiStatus reset_status, usize data_size, void *reset_data);

typedef struct {
	EfiTableHeader hdr;
	EfiGetTime *get_time;
	EfiSetTime *set_time;
	EfiGetWakeupTime *get_wakeup_time;
	EfiSetWakeupTime *set_wakeup_time;

	EfiSetVirtualAddressMap set_virtual_address_map;
	EfiConvertPointer convert_pointer;

	EfiGetVariable get_variable;
	EfiGetNextVariableName get_next_variable_name;
	EfiSetVariable set_variable;

	EfiGetNextHighMonoCount get_next_high_monotonic_count;
	EfiResetSystem reset_system;
	//TODO finish table
} EfiRuntimeServices;

typedef EfiStatus (EFIAPI *EfiEventNotify) (EfiEvent event, void *context);

typedef EfiStatus (EFIAPI *EfiRaiseTpl) (EfiTpl new_tpl);
typedef EfiStatus (EFIAPI *EfiRestoreTpl) (EfiTpl old_tpl);

typedef EfiStatus (EFIAPI *EfiAllocatePages) (EfiAllocateType type, EfiMemoryType memory_type, usize pages, u64 *memory);
typedef EfiStatus (EFIAPI *EfiFreePages) (u64 memory, usize pages);
typedef EfiStatus (EFIAPI *EfiGetMemoryMap) (usize *memory_map_size, EfiMemoryDescriptor *memory_map, usize *map_key, usize *descriptor_size, u32 *descriptor_version);
typedef EfiStatus (EFIAPI *EfiAllocatePool) (EfiMemoryType pool_type, usize size, void **buffer);
typedef EfiStatus (EFIAPI *EfiFreePool) (void *buffer);

typedef EfiStatus (EFIAPI *EfiCreateEvent) (u32 type, EfiTpl notify_tpl, EfiEventNotify notify_function, void *notify_context, EfiEvent *event);
typedef EfiStatus (EFIAPI *EfiSetTimer) (EfiEvent event, EfiTimerDelay type, u64 trigger_time);
typedef EfiStatus (EFIAPI *EfiWaitForEvent) (usize number_of_events, EfiEvent *event, usize *index);

typedef struct {
	EfiTableHeader hdr;
	EfiRaiseTpl raise_tpl;
	EfiRestoreTpl restore_tpl;

	EfiAllocatePages allocate_pages;
	EfiFreePages free_pages;
	EfiGetMemoryMap get_memory_map;
	EfiAllocatePool allocate_pool;
	EfiFreePool free_pool;

	EfiCreateEvent create_event;
	EfiSetTimer set_timer;
	EfiWaitForEvent wait_for_event;
	// TODO finish table
} EfiBootServices;

typedef struct {
	EfiTableHeader hdr;
} EfiConfigurationTable;

typedef struct {
	EfiTableHeader hdr;
	u16 *fw_vendor;
	u32 fw_revision;
	EfiHandle console_in_handle;
	EfiSimpleTextInputProtocol *con_in;
	EfiHandle console_out_handle;
	EfiSimpleTextOutputProtocol *con_out;
	EfiHandle console_err_handle;
	EfiSimpleTextOutputProtocol *con_err;
	EfiRuntimeServices *runtime_services;
	EfiBootServices *boot_services;
	usize num_entries;
	EfiConfigurationTable *configuration_table;
} EfiSystemTable;

extern EfiSystemTable *ST;


#endif // _EFI_H_
