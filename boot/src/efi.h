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

#define EFI_DEVICE_PATH_PROTOCOL_GUID {0x09576e91, 0x6d3f, 0x11d2, {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}}
#define MEDIA_DEVICE_PATH 4
#define MEDIA_FILE_PATH 4

typedef struct {
	u8 type;
	u8 sub_type;
	u8 length[2];
} EfiDevicePathProtocol;


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

typedef enum {
	EfiNativeInterface
} EfiInterfaceType;

typedef enum {
	AllHandles,
	ByRegisterNotify,
	ByProtocol
} EfiLocateSearchType;

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
typedef EfiStatus (EFIAPI *EfiSignalEvent) (EfiEvent event);
typedef EfiStatus (EFIAPI *EfiCloseEvent) (EfiEvent event);
typedef EfiStatus (EFIAPI *EfiCheckEvent) (EfiEvent event);

typedef EfiStatus (EFIAPI *EfiInstallProtocolInterface) (EfiHandle *handle, EfiGuid *protocol, EfiInterfaceType interface_type, void *interface);
typedef EfiStatus (EFIAPI *EfiReinstallProtocolInterface) (EfiHandle handle, EfiGuid *protocol, void *old_interface, void *new_interface);
typedef EfiStatus (EFIAPI *EfiUninstallProtocolInterface) (EfiHandle handle, EfiGuid *protocol, void *interface);
typedef EfiStatus (EFIAPI *EfiHandleProtocol) (EfiHandle handle, EfiGuid *protocol, void **interface);
typedef EfiStatus (EFIAPI *EfiRegisterProtocolNotify) (EfiGuid *protocol, EfiEvent event, void **registration);

typedef EfiStatus (EFIAPI *EfiLocateHandle) (EfiLocateSearchType search_type, EfiGuid *protocol, void *search_key, usize *buffer_size, EfiHandle *buffer);
typedef EfiStatus (EFIAPI *EfiLocateDevicePath) (EfiGuid *protocol, EfiDevicePathProtocol **device_path, EfiHandle *device);
typedef EfiStatus (EFIAPI *EfiInstallConfigurationTable) (EfiGuid *guid, void *table);

typedef EfiStatus (EFIAPI *EfiImageLoad) (bool boot_policy, EfiHandle parent_image_handle, EfiDevicePathProtocol *device_path, void *source_buffer, usize source_size, EfiHandle *image_handle);
typedef EfiStatus (EFIAPI *EfiImageStart) (EfiHandle image_handle, usize *ExitDataSize, u16 **exit_data);
typedef EfiStatus (EFIAPI *EfiExit) (EfiHandle image_handle, EfiStatus exit_status, usize exit_data_size, u16 *exit_data);
typedef EfiStatus (EFIAPI *EfiImageUnload) (EfiHandle image_handle);
typedef EfiStatus (EFIAPI *EfiExitBootServices) (EfiHandle image_handle, usize map_key);

typedef EfiStatus (EFIAPI *EfiGetNextMonotonicCount) (u64 *count);
typedef EfiStatus (EFIAPI *EfiStall) (usize microseconds);
typedef EfiStatus (EFIAPI *EfiSetWatchdogTimer) (usize timeout, u64 watchdog_code, usize data_size, u16 *watchdog_data);

typedef EfiStatus (EFIAPI *EfiConnectController) (EfiHandle controller_handle, EfiHandle *driver_image_handle, EfiDevicePathProtocol *remaining_device_path, bool recursive);
typedef EfiStatus (EFIAPI *EfiDisconnectController) (EfiHandle controller_handle, EfiHandle *driver_image_handle, EfiHandle child_handle);

typedef struct {
	EfiHandle agent_handle;
	EfiHandle controller_handle;
	u32 attributes;
	u32 open_count;
} EfiOpenProtocolInformationEntry;

typedef EfiStatus (EFIAPI *EfiOpenProtocol) (EfiHandle handle, EfiGuid *protocol, void **interface, EfiHandle agent_handle, EfiHandle controller_handle, u32 attributes);
typedef EfiStatus (EFIAPI *EfiCloseProtocol) (EfiHandle handle, EfiGuid *protocol, EfiHandle agent_handle, EfiHandle controller_handle);
typedef EfiStatus (EFIAPI *EfiOpenProtocolInformation) (EfiHandle handle, EfiGuid *protocol, EfiOpenProtocolInformationEntry **entry_buffer, usize *entry_count);

typedef EfiStatus (EFIAPI *EfiProtocolsPerHandle) (EfiHandle handle, EfiGuid **protocol_buffer, usize *protocol_buffer_count);
typedef EfiStatus (EFIAPI *EfiLocateHandleBuffer) (EfiLocateSearchType search_type, EfiGuid *protocol, void *search_key, usize *no_handles, EfiHandle **buffer);
typedef EfiStatus (EFIAPI *EfiLocateProtocol) (EfiGuid *protocol, void *registration, void **interface);

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
	EfiSignalEvent signal_event;
	EfiCloseEvent close_event;
	EfiCheckEvent check_event;

	EfiInstallProtocolInterface install_protocol_interface;
	EfiReinstallProtocolInterface reinstall_protocol_interface;
	EfiUninstallProtocolInterface uninstall_protocol_interface;
	EfiHandleProtocol handle_protocol;
	void *reserved;
	EfiRegisterProtocolNotify register_protocol_notify;

	EfiLocateHandle locate_handle;
	EfiLocateDevicePath locate_device_path;
	EfiInstallConfigurationTable install_configuration_table;

	EfiImageLoad load_image;
	EfiImageStart start_image;
	EfiExit exit;
	EfiImageUnload unload_image;
	EfiExitBootServices exit_boot_services;

	EfiGetNextMonotonicCount get_next_monotonic_count;
	EfiStall stall;
	EfiSetWatchdogTimer set_watchdog_timer;

	EfiConnectController connect_controller;
	EfiDisconnectController disconnect_controller;

	EfiOpenProtocol open_protocol;
	EfiCloseProtocol close_protocol;
	EfiOpenProtocolInformation open_protocol_information;

	EfiProtocolsPerHandle protocols_per_handle;
	EfiLocateHandleBuffer locate_handle_buffer;
	EfiLocateProtocol locate_protocol;
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

#define EFI_DEVICE_PATH_TO_TEXT_PROTOCOL_GUID {0x8b843e20, 0x8132, 0x4852, {0x90, 0xcc, 0x55, 0x1a, 0x4e, 0x4a, 0x7f, 0x1c}}

typedef u16 *(EFIAPI *EfiDevicePathToTextNode) (const EfiDevicePathProtocol *device_node, bool display_only, bool allow_shortcuts);
typedef u16 *(EFIAPI *EfiDevicePathToTextPath) (const EfiDevicePathProtocol *device_node, bool display_only, bool allow_shortcuts);

typedef struct {
	EfiDevicePathToTextNode ConvertDeviceNodeToText;
	EfiDevicePathToTextPath ConvertDevicePathToText;
} EfiDevicePathToTextProtocol;

#define EFI_LOADED_IMAGE_PROTOCOL_GUID {0x5b1b31a1,0x9562,0x11d2, {0x8e,0x3f,0x00,0xa0,0xc9,0x69,0x72,0x3b}}
#define EFI_LOADED_IMAGE_DEVICE_PATH_PROTOCOL_GUID {0xbc62157e,0x3e33,0x4fec, {0x99,0x20,0x2d,0x3b,0x36,0xd7,0x50,0xdf}}

typedef struct {
	u32 revision;
	EfiHandle parent_handle;
	EfiSystemTable *system_table;

	EfiHandle device_handle;
	EfiDevicePathProtocol *file_path;
	void *reserved;

	u32 load_options_size;
	void *load_options;

	void *image_base;
	u64 image_size;
	EfiMemoryType image_code_type;
	EfiMemoryType image_data_type;
	EfiImageUnload unload;
} EfiLoadedImageProtocol;

typedef usize (EFIAPI *EfiDevicePathUtilsGetDevicePathSize) (const EfiDevicePathProtocol *device_path);
typedef EfiDevicePathProtocol *(EFIAPI *EfiDevicePathUtilsDupDevicePath) (const EfiDevicePathProtocol *device_path);
typedef EfiDevicePathProtocol *(EFIAPI *EfiDevicePathUtilsAppendPath) (const EfiDevicePathProtocol *src1, const EfiDevicePathProtocol *src2);
typedef EfiDevicePathProtocol *(EFIAPI *EfiDevicePathUtilsAppendNode) (const EfiDevicePathProtocol *device_path, const EfiDevicePathProtocol *device_node);
typedef EfiDevicePathProtocol *(EFIAPI *EfiDevicePathUtilsAppendInstance) (const EfiDevicePathProtocol *device_path, const EfiDevicePathProtocol *device_path_instance);
typedef EfiDevicePathProtocol *(EFIAPI *EfiDevicePathUtilsGetNextInstance) (const EfiDevicePathProtocol **device_path_instance, usize *device_path_instance_size);
typedef bool (EFIAPI *EfiDevicePathUtilsIsMultiInstance) (const EfiDevicePathProtocol *device_path);
typedef EfiDevicePathProtocol *(EFIAPI *EfiDevicePathUtilsCreateNode) (u8 node_type, u8 node_sub_type, u16 node_length);

#define EFI_DEVICE_PATH_UTILITIES_PROTOCOL_GUID {0x379be4e,0xd706,0x437d,{0xb0,0x37,0xed,0xb8,0x2f,0xb7,0x72,0xa4}}

typedef struct {
	EfiDevicePathUtilsGetDevicePathSize get_device_path_size;
	EfiDevicePathUtilsDupDevicePath duplicate_device_path;
	EfiDevicePathUtilsAppendPath append_device_path;
	EfiDevicePathUtilsAppendNode append_device_node;
	EfiDevicePathUtilsAppendInstance append_device_path_instance;
	EfiDevicePathUtilsGetNextInstance get_next_device_path_instance;
	EfiDevicePathUtilsIsMultiInstance is_device_path_multi_instance;
	EfiDevicePathUtilsCreateNode create_device_node;
} EfiDevicePathUtilitiesProtocol;

struct _EfiLoadFileProtocol;

typedef struct _EfiLoadFileProtocol EfiLoadFileProtocol;

typedef EfiStatus (EFIAPI *EfiLoadFile) (EfiLoadFileProtocol *this, EfiDevicePathProtocol *file_path, bool boot_policy, usize *buffer_size, void *buffer);

#define EFI_LOAD_FILE_PROTOCOL_GUID {0x56ec3091,0x954c,0x11d2,{0x8e,0x3f,0x00,0xa0,0xc9,0x69,0x72,0x3b}}
//#define EFI_LOAD_FILE_PROTOCOL_GUID {0x4006c0c1,0xfcb3,0x403e,{0x99,0x6d,0x4a,0x6c,0x87,0x24,0xe0,0x6d}}

struct _EfiLoadFileProtocol {
	EfiLoadFile load_file;
};

struct _EfiFileProtocol;

typedef struct _EfiFileProtocol EfiFileProtocol;

#define EFI_FILE_MODE_READ 1

typedef EfiStatus (EFIAPI *EfiFileOpen) (EfiFileProtocol *this, EfiFileProtocol **new_handle, u16 *file_name, u64 open_mode, u64 attributes);
typedef EfiStatus (EFIAPI *EfiFileClose) (EfiFileProtocol *this);
typedef EfiStatus (EFIAPI *EfiFileDelete) (EfiFileProtocol *this);
typedef EfiStatus (EFIAPI *EfiFileRead) (EfiFileProtocol *this, usize *buffer_size, void *buffer);
typedef EfiStatus (EFIAPI *EfiFileWrite) (EfiFileProtocol *this, usize *buffer_size, void *buffer);
typedef EfiStatus (EFIAPI *EfiFileGetPosition) (EfiFileProtocol *this, u64 *position);
typedef EfiStatus (EFIAPI *EfiFileSetPosition) (EfiFileProtocol *this, u64 position);
typedef EfiStatus (EFIAPI *EfiFileGetInfo) (EfiFileProtocol *this, EfiGuid *information_type, usize *buffer_size, void *buffer);
typedef EfiStatus (EFIAPI *EfiFileSetInfo) (EfiFileProtocol *this, EfiGuid *information_type, usize buffer_size, void *buffer);
typedef EfiStatus (EFIAPI *EfiFileFlush) (EfiFileProtocol *this);

struct _EfiFileProtocol {
	u64 revision;
	EfiFileOpen open;
	EfiFileClose close;
	EfiFileDelete delete;
	EfiFileRead read;
	EfiFileWrite write;
	EfiFileGetPosition get_position;
	EfiFileSetPosition set_position;
	EfiFileGetInfo get_info;
	EfiFileSetInfo set_info;
	EfiFileFlush flush;
};

#define EFI_FILE_INFO_ID {0x09576e92,0x6d3f,0x11d2,{0x8e,0x39,0x00,0xa0,0xc9,0x69,0x72,0x3b}}

typedef struct {
	u64 size;
	u64 file_size;
	u64 physical_size;
	EfiTime create_time;
	EfiTime last_access_time;
	EfiTime modification_time;
	u64 attribute;
	u16 file_name[];
} EfiFileInfo;

#define EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID {0x0964e5b22,0x6459,0x11d2,{0x8e,0x39,0x00,0xa0,0xc9,0x69,0x72,0x3b}}

struct _EfiSimpleFileSystemProtocol;

typedef struct _EfiSimpleFileSystemProtocol EfiSimpleFileSystemProtocol;

typedef EfiStatus (EFIAPI *EfiSimpleFileSystemProtocolOpenVolume) (EfiSimpleFileSystemProtocol *this, EfiFileProtocol **out);

struct _EfiSimpleFileSystemProtocol {
	u64 revision;
	EfiSimpleFileSystemProtocolOpenVolume open_volume;
};

#define EFI_GRAPHICS_OUTPUT_PROTOCOL {0x9042a9de,0x23dc,0x4a38,{0x96,0xfb,0x7a,0xde,0xd0,0x80,0x51,0x6a}}

typedef enum {
	PixelRedGreenBlueReserved8BitPerColor,
	PixelBlueGreenRed8BitPerColor,
	PixelBitMask,
	PixelBltOnly,
	PixelFormatsMax
} EfiGraphicsPixelFormat;

typedef struct {
	u32 red_mask;
	u32 green_mask;
	u32 blue_mask;
	u32 reserved_mask;
} EfiPixelBitmap;

typedef struct {
	u32 version;
	u32 horizontal_resolution;
	u32 vertical_resolution;
	EfiGraphicsPixelFormat pixel_format;
	EfiPixelBitmap pixel_information;
	u32 pixels_per_scan_line;
} EfiGraphicsOutputModeInformation;

typedef struct {
	u32 max_mode;
	u32 mode;
	EfiGraphicsOutputModeInformation *info;
	usize size_of_info;
	u64 frame_buffer_base;
	usize frame_buffer_size;
} EfiGraphicsOutputProtocolMode;

struct _EfiGraphicsOutputProtocol;

typedef struct _EfiGraphicsOutputProtocol EfiGraphicsOutputProtocol;

struct _EfiGraphicsOutputProtocol {
	EfiGraphicsOutputProtocolQueryMode query_mode;
	EfiGraphicsOutputProtocolSetMode set_mode;
	EfiGraphicsOutputProtocolBlt blt;
	EfiGraphicsOutputProtocolMode *mode;
};

#endif // _EFI_H_
