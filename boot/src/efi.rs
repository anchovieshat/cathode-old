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

pub struct MemoryDescriptor {
    pub type_: u32,
    pub physical_start: u64,
    pub virtual_start: u64,
    pub number_of_pages: u64,
    pub attributes: u64
}

pub enum ResetType {
    ResetCold,
    ResetWarm,
    ResetShutdown,
    ResetPlatformSpecific
}

pub type GetTime = extern "win64" fn(*mut Time, *const TimeCapabilities) -> Status;
pub type SetTime = extern "win64" fn(*const Time) -> Status;
pub type GetWakeupTime = extern "win64" fn(*mut bool, *mut bool, *mut Time) -> Status;
pub type SetWakeupTime = extern "win64" fn(bool, *const Time) -> Status;
pub type SetVirtualAddressMap = extern "win64" fn(usize, usize, u32, MemoryDescriptor) -> Status;
pub type ConvertPointer = extern "win64" fn(usize, *const *mut ()) -> Status;
pub type GetVariable = extern "win64" fn(*const u16, *const Guid, *mut u32, *mut usize, *mut ()) -> Status;
pub type GetNextVariableName = extern "win64" fn(*mut usize, *mut u16, *const Guid) -> Status;
pub type SetVariable = extern "win64" fn(*const u16, *const Guid, u32, usize, *const ()) -> Status;
pub type GetNextHighMonoCount = extern "win64" fn(*mut u32) -> Status;
pub type ResetSystem = extern "win64" fn(ResetType, Status, usize, *const ()) -> Status;

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

pub type RaiseTpl = extern "win64" fn(Tpl) -> Status;
pub type RestoreTpl = extern "win64" fn(Tpl) -> Status;
pub type AllocatePages = extern "win64" fn(AllocateType, MemoryType, usize, *mut u64) -> Status;
pub type FreePages = extern "win64" fn(u64, usize) -> Status;
pub type GetMemoryMap = extern "win64" fn(*mut usize, *mut MemoryDescriptor, *mut usize, *mut usize, *mut u32) -> Status;
pub type AllocatePool = extern "win64" fn(MemoryType, usize, *const *mut ()) -> Status;
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
pub type HandleProtocol = extern "win64" fn(Handle, *const Guid, *const *mut ()) -> Status;
pub type RegisterProtocolNotify = extern "win64" fn(*const Guid, Event, *const *mut ()) -> Status;
pub type LocateHandle = extern "win64" fn(LocateSearchType, *const Guid, *const (), *mut usize, *mut Handle) -> Status;
pub type LocateDevicePath = extern "win64" fn(*const Guid, *const *mut DevicePathProtocol, *mut Handle) -> Status;
pub type InstallConfigurationTable = extern "win64" fn(*const Guid, *const ()) -> Status;

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
