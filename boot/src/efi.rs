#![allow(dead_code)]

pub type Status = usize;

pub type TextReset = *const ();
pub type TextString = extern "win64" fn (*const SimpleTextOutputProtocol, *const u16) -> Status;

#[repr(C)]
pub struct SimpleTextOutputProtocol {
    pub reset: TextReset,
    pub output_string: TextString,
}

pub type SimpleTextInputProtocol = ();

#[repr(C)]
pub struct TableHeader {
	pub signature: u64,
	pub revision: u32,
	pub header_size: u32,
	pub crc32: u32,
	_reserved: u32,
}

#[repr(C)]
pub struct TimeCapabilities {
    pub resolution: u32,
    pub accuracy: u32,
    pub sets_to_zero: Bool,
}

#[repr(C)]
pub struct Guid {
    pub data1: u32,
    pub data2: u16,
    pub data3: u16,
    pub data4: [u8; 8],
}

#[repr(C)]
pub enum AllocateType {
    AllocateAnyPages,
    AllocateMaxAddress,
    AllocateAddress,
    MaxAllocateType
}

#[repr(C)]
pub enum MemoryType {
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
}

#[repr(C)]
pub enum TimerDelay {
    TimerCancel,
    TimerPeriodic,
    TimerRelative,
}

#[repr(C)]
pub enum InterfaceType {
    NativeInterface,
    MaxInterface, // Not real
}

#[repr(C)]
pub enum LocateSearchType {
    AllHandles,
    ByRegisterNotify,
    ByProtocol
}

#[repr(C)]
pub struct OpenProtocolInformationEntry {
    pub agent_handle: Handle,
    pub controller_handle: Handle,
    pub attributes: u32,
    pub open_count: u32,
}

#[repr(C)]
pub struct DevicePathProtocol {
    pub ty: u8,
    pub sub_ty: u8,
    pub length: [u8; 2],
}

type Event = *mut ();
type EventNotify = extern "win64" fn(Event, *mut ());

#[repr(C)]
pub struct SystemTable {
    pub hdr: TableHeader,
    pub fw_vendor: *const u16,
    pub fw_revision: u32,
    pub console_in_handle: Handle,
    pub con_in: *const SimpleTextInputProtocol,
    pub con_out_handle: Handle,
    pub con_out: *const SimpleTextOutputProtocol,
    pub con_err_handle: Handle,
    pub con_err: *const SimpleTextOutputProtocol,
    pub runtime_services: RuntimeServices,
    pub boot_services: BootServices,
    pub num_entries: usize,
    pub configuration_table: ConfigurationTable
}

#[repr(C)]
pub struct ConfigurationTable {
    pub hdr: TableHeader,
}

#[repr(C)]
pub struct Time {
    pub year: u16,
    pub month: u8,
    pub day: u8,
    pub hour: u8,
    pub minute: u8,
    pub second: u8,
    _pad1: u8,
    pub nanosecond: u32,
    pub time_zone: i16,
    pub daylight: u8,
    _pad2: u8
}

#[repr(C)]
pub struct MemoryDescriptor {
    pub type_: u32,
    pub physical_start: u64,
    pub virtual_start: u64,
    pub number_of_pages: u64,
    pub attributes: u64
}

#[repr(C)]
pub enum ResetType {
    ResetCold,
    ResetWarm,
    ResetShutdown,
    ResetPlatformSpecific
}

pub type GetTime = extern "win64" fn(*mut Time, *const TimeCapabilities) -> Status;
pub type SetTime = extern "win64" fn(*const Time) -> Status;
pub type GetWakeupTime = extern "win64" fn(*mut Bool, *mut Bool, *mut Time) -> Status;
pub type SetWakeupTime = extern "win64" fn(Bool, *const Time) -> Status;
pub type SetVirtualAddressMap = extern "win64" fn(usize, usize, u32, MemoryDescriptor) -> Status;
pub type ConvertPointer = extern "win64" fn(usize, *const *mut ()) -> Status;
pub type GetVariable = extern "win64" fn(*const u16, *const Guid, *mut u32, *mut usize, *mut ()) -> Status;
pub type GetNextVariableName = extern "win64" fn(*mut usize, *mut u16, *const Guid) -> Status;
pub type SetVariable = extern "win64" fn(*const u16, *const Guid, u32, usize, *const ()) -> Status;
pub type GetNextHighMonoCount = extern "win64" fn(*mut u32) -> Status;
pub type ResetSystem = extern "win64" fn(ResetType, Status, usize, *const ()) -> Status;

#[repr(C)]
pub struct RuntimeServices {
    pub hdr: TableHeader,
    pub get_time: GetTime,
    pub set_time: SetTime,
    pub get_wakeup_time: GetWakeupTime,
    pub set_wakeup_time: SetWakeupTime,
    pub set_virtual_address_map: SetVirtualAddressMap,
    pub convert_pointer: ConvertPointer,
    pub get_variable: GetVariable,
    pub get_next_variable_name: GetNextVariableName,
    pub set_variable: SetVariable,
    pub get_next_high_monotonic_count: GetNextHighMonoCount,
    pub reset_system: ResetSystem
}

pub type Tpl = usize;
pub type Bool = u8;

pub type RaiseTpl = extern "win64" fn(Tpl) -> Status;
pub type RestoreTpl = extern "win64" fn(Tpl) -> Status;
pub type AllocatePages = extern "win64" fn(AllocateType, MemoryType, usize, *mut u64) -> Status;
pub type FreePages = extern "win64" fn(u64, usize) -> Status;
pub type GetMemoryMap = extern "win64" fn(*mut usize, *mut MemoryDescriptor, *mut usize, *mut usize, *mut u32) -> Status;
pub type AllocatePool = extern "win64" fn(MemoryType, usize, *mut *mut ()) -> Status;
pub type FreePool = extern "win64" fn(*const ()) -> Status;
pub type CreateEvent = extern "win64" fn(u32, Tpl, EventNotify, *const (), *const Event) -> Status;
pub type SetTimer = extern "win64" fn(Event, TimerDelay, u64) -> Status;
pub type WaitForEvent = extern "win64" fn(usize, *mut Event, *mut usize) -> Status;
pub type SignalEvent = extern "win64" fn(Event) -> Status;
pub type CloseEvent = extern "win64" fn(Event) -> Status;
pub type CheckEvent = extern "win64" fn(Event) -> Status;
pub type InstallProtocolInterface = extern "win64" fn(*mut Handle, *const Guid, InterfaceType, *mut ()) -> Status;
pub type ReinstallProtocolInterface = extern "win64" fn(Handle, *const Guid, *const (), *const ()) -> Status;
pub type UninstallProtocolInterface = extern "win64" fn(Handle, *const Guid, *const ()) -> Status;
pub type HandleProtocol = extern "win64" fn(Handle, *const Guid, *mut *mut ()) -> Status;
pub type RegisterProtocolNotify = extern "win64" fn(*const Guid, Event, *const *mut ()) -> Status;
pub type LocateHandle = extern "win64" fn(LocateSearchType, *const Guid, *const (), *mut usize, *mut Handle) -> Status;
pub type LocateDevicePath = extern "win64" fn(*const Guid, *const *mut DevicePathProtocol, *mut Handle) -> Status;
pub type InstallConfigurationTable = extern "win64" fn(*const Guid, *const ()) -> Status;
pub type ImageLoad = extern "win64" fn(Bool, Handle, DevicePathProtocol, *const (), usize, Handle) -> Status;
pub type ImageStart = extern "win64" fn(Handle, *mut usize, *mut *mut u16) -> Status;
pub type Exit = extern "win64" fn(Handle, *const usize, *const *const u16) -> Status;
pub type ImageUnload = extern "win64" fn(Handle) -> Status;
pub type ExitBootServices = extern "win64" fn(Handle, usize) -> Status;
pub type GetNextMonotonicCount = extern "win64" fn(*mut u64) -> Status;
pub type Stall = extern "win64" fn(usize) -> Status;
pub type SetWatchdogTimer = extern "win64" fn(usize, u64, usize, *const u16) -> Status;
pub type ConnectController = extern "win64" fn(Handle, *const Handle, *const Handle, *const DevicePathProtocol, Bool) -> Status;
pub type DisconnectController = extern "win64" fn(Handle, *const Handle, Handle) -> Status;
pub type OpenProtocol = extern "win64" fn(Handle, *const Guid, *mut *mut (), Handle, Handle, u32) -> Status;
pub type CloseProtocol = extern "win64" fn(Handle, *const Guid, Handle, Handle) -> Status;
pub type OpenProtocolInformation = extern "win64" fn(Handle, *const Guid, *mut *mut OpenProtocolInformationEntry, *mut usize) -> Status;
pub type ProtocolsPerHandle = extern "win64" fn(Handle, *mut *mut Guid, *mut usize) -> Status;
pub type LocateHandleBuffer = extern "win64" fn(LocateSearchType, *const Guid, *const (), *mut usize, *mut *mut Handle) -> Status;
pub type LocateProtocol = extern "win64" fn(*const Guid, *mut (), *mut *mut ()) -> Status;

#[repr(C)]
pub struct BootServices {
    pub hdr: TableHeader,
    pub raise_tpl: RaiseTpl,
    pub restore_tpl: RestoreTpl,
    pub allocate_pages: AllocatePages,
    pub free_pages: FreePages,
    pub get_memory_map: GetMemoryMap,
    pub allocate_pool: AllocatePool,
    pub free_pool: FreePool,
    pub create_event: CreateEvent,
    pub set_timer: SetTimer,
    pub wait_for_event: WaitForEvent,
    pub signal_event: SignalEvent,
    pub close_event: CloseEvent,
    pub check_event: CheckEvent,
    pub install_protocol_interface: InstallProtocolInterface,
    pub reinstall_protocol_interface: ReinstallProtocolInterface,
    pub uninstall_protocol_interface: UninstallProtocolInterface,
    pub handle_protocol: HandleProtocol,
    _reserved1: *const (),
    pub register_protocol_notify: RegisterProtocolNotify,
    pub locate_handle: LocateHandle,
    pub locate_device_path: LocateDevicePath,
    pub install_configuration_table: InstallConfigurationTable,
    pub load_image: ImageLoad,
    pub start_image: ImageStart,
    pub exit: Exit,
    pub unload_image: ImageUnload,
    pub exit_boot_services: ExitBootServices,
    pub get_next_monotonic_count: GetNextMonotonicCount,
    pub stall: Stall,
    pub set_watchdog_timer: SetWatchdogTimer,
    pub connect_controller: ConnectController,
    pub disconnect_controller: DisconnectController,
    pub open_protocol: OpenProtocol,
    pub close_protocol: CloseProtocol,
    pub open_protocol_information: OpenProtocolInformation,
    pub protocols_per_handle: ProtocolsPerHandle,
    pub locate_handle_buffer: LocateHandleBuffer,
    pub locate_protocol: LocateProtocol
}

pub type Handle = *const ();
