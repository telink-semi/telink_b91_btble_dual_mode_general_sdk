#include "tlkapi/tlkapi_stdio.h"
#include "tlklib/usb/tlkusb_stdio.h"
#if (TLK_CFG_USB_ENABLE)
#include "tlklib/usb/tlkusb_desc.h"
#include "tlklib/usb/tlkusb_module.h"

#if (TLKUSB_UDB_ENABLE)
extern const tlkusb_module_t gTlkUsbUdbModule;
#endif
#if (TLKUSB_AUD_ENABLE)
extern const tlkusb_module_t gTlkUsbAudModule;
#endif
#if (TLKUSB_MSC_ENABLE)
extern const tlkusb_module_t gTlkUsbMscModule;
#endif
#if (TLKUSB_CDC_ENABLE)
extern const tlkusb_module_t gTlkUsbCdcModule;
#endif
#if (TLKUSB_USR_ENABLE)
extern const tlkusb_module_t gTlkUsbUsrModule;
#endif
static const tlkusb_module_t *sTlkUsbModule[TLKUSB_MODTYPE_MAX] = 
{
	#if (TLKUSB_UDB_ENABLE)
	&gTlkUsbUdbModule,
	#else
	nullptr,
	#endif
	#if (TLKUSB_AUD_ENABLE)
	&gTlkUsbAudModule,
	#else
	nullptr,
	#endif
	#if (TLKUSB_MSC_ENABLE)
	&gTlkUsbMscModule,
	#else
	nullptr,
	#endif
	#if (TLKUSB_CDC_ENABLE)
	&gTlkUsbCdcModule,
	#else
	nullptr,
	#endif
	#if (TLKUSB_USR_ENABLE)
	&gTlkUsbUsrModule,
	#else
	nullptr,
	#endif
};


int tlkusb_module_init(uint08 modType)
{
	if(modType >= TLKUSB_MODTYPE_MAX) return -TLK_EPARAM;
	if(sTlkUsbModule[modType] == nullptr || sTlkUsbModule[modType]->pCtrl == nullptr
		|| sTlkUsbModule[modType]->pCtrl->Init == nullptr){
		return -TLK_ENOSUPPORT;
	}
	return sTlkUsbModule[modType]->pCtrl->Init();
}
void tlkusb_module_reset(uint08 modType)
{
	if(modType >= TLKUSB_MODTYPE_MAX) return;
	if(sTlkUsbModule[modType] == nullptr || sTlkUsbModule[modType]->pCtrl == nullptr
		|| sTlkUsbModule[modType]->pCtrl->Reset == nullptr){
		return;
	}
	sTlkUsbModule[modType]->pCtrl->Reset();
}
void tlkusb_module_deinit(uint08 modType)
{
	if(modType >= TLKUSB_MODTYPE_MAX) return;
	if(sTlkUsbModule[modType] == nullptr || sTlkUsbModule[modType]->pCtrl == nullptr
		|| sTlkUsbModule[modType]->pCtrl->Deinit == nullptr){
		return;
	}
	sTlkUsbModule[modType]->pCtrl->Deinit();
}
void tlkusb_module_handler(uint08 modType)
{
	if(modType >= TLKUSB_MODTYPE_MAX) return;
	if(sTlkUsbModule[modType] == nullptr || sTlkUsbModule[modType]->pCtrl == nullptr
		|| sTlkUsbModule[modType]->pCtrl->Handler == nullptr){
		return;
	}
	sTlkUsbModule[modType]->pCtrl->Handler();
}

int tlkusb_module_getClassInf(uint08 modType, tlkusb_setup_req_t *pSetup, uint08 infNumb)
{
	if(modType >= TLKUSB_MODTYPE_MAX) return -TLK_EPARAM;
	if(sTlkUsbModule[modType] == nullptr || sTlkUsbModule[modType]->pCtrl == nullptr
		|| sTlkUsbModule[modType]->pCtrl->GetClassInf == nullptr){
		return -TLK_ENOSUPPORT;
	}
	return sTlkUsbModule[modType]->pCtrl->GetClassInf(pSetup, infNumb);
}
int tlkusb_module_setClassInf(uint08 modType, tlkusb_setup_req_t *pSetup, uint08 infNumb)
{
	if(modType >= TLKUSB_MODTYPE_MAX) return -TLK_EPARAM;
	if(sTlkUsbModule[modType] == nullptr || sTlkUsbModule[modType]->pCtrl == nullptr
		|| sTlkUsbModule[modType]->pCtrl->SetClassInf == nullptr){
		return -TLK_ENOSUPPORT;
	}
	return sTlkUsbModule[modType]->pCtrl->SetClassInf(pSetup, infNumb);
}
int tlkusb_module_getClassEdp(uint08 modType, tlkusb_setup_req_t *pSetup, uint08 edpNumb)
{
	if(modType >= TLKUSB_MODTYPE_MAX) return -TLK_EPARAM;
	if(sTlkUsbModule[modType] == nullptr || sTlkUsbModule[modType]->pCtrl == nullptr
		|| sTlkUsbModule[modType]->pCtrl->GetClassEdp == nullptr){
		return -TLK_ENOSUPPORT;
	}
	return sTlkUsbModule[modType]->pCtrl->GetClassEdp(pSetup, edpNumb);
}
int tlkusb_module_setClassEdp(uint08 modType, tlkusb_setup_req_t *pSetup, uint08 edpNumb)
{
	if(modType >= TLKUSB_MODTYPE_MAX) return -TLK_EPARAM;
	if(sTlkUsbModule[modType] == nullptr || sTlkUsbModule[modType]->pCtrl == nullptr
		|| sTlkUsbModule[modType]->pCtrl->SetClassEdp == nullptr){
		return -TLK_ENOSUPPORT;
	}
	return sTlkUsbModule[modType]->pCtrl->SetClassEdp(pSetup, edpNumb);
}
int tlkusb_module_getInterface(uint08 modType, tlkusb_setup_req_t *pSetup, uint08 infNumb)
{
	if(modType >= TLKUSB_MODTYPE_MAX) return -TLK_EPARAM;
	if(sTlkUsbModule[modType] == nullptr || sTlkUsbModule[modType]->pCtrl == nullptr
		|| sTlkUsbModule[modType]->pCtrl->GetInterface == nullptr){
		return -TLK_ENOSUPPORT;
	}
	return sTlkUsbModule[modType]->pCtrl->GetInterface(pSetup, infNumb);
}
int tlkusb_module_setInterface(uint08 modType, tlkusb_setup_req_t *pSetup, uint08 infNumb)
{
	if(modType >= TLKUSB_MODTYPE_MAX) return -TLK_EPARAM;
	if(sTlkUsbModule[modType] == nullptr || sTlkUsbModule[modType]->pCtrl == nullptr
		|| sTlkUsbModule[modType]->pCtrl->SetInterface == nullptr){
		return -TLK_ENOSUPPORT;
	}
	return sTlkUsbModule[modType]->pCtrl->SetInterface(pSetup, infNumb);
}

uint16 tlkusb_module_getDeviceLens(uint08 modType)
{
	if(modType >= TLKUSB_MODTYPE_MAX) return 0;
	if(sTlkUsbModule[modType] == nullptr || sTlkUsbModule[modType]->pDesc == nullptr
		|| sTlkUsbModule[modType]->pDesc->GetDeviceLens == nullptr){
		return 0;
	}
	return sTlkUsbModule[modType]->pDesc->GetDeviceLens();
}
uint16 tlkusb_module_getConfigLens(uint08 modType)
{
	if(modType >= TLKUSB_MODTYPE_MAX) return 0;
	if(sTlkUsbModule[modType] == nullptr || sTlkUsbModule[modType]->pDesc == nullptr
		|| sTlkUsbModule[modType]->pDesc->GetConfigLens == nullptr){
		return 0;
	}
	return sTlkUsbModule[modType]->pDesc->GetConfigLens();
}
uint16 tlkusb_module_getStringLens(uint08 modType, uint08 index)
{
	if(modType >= TLKUSB_MODTYPE_MAX) return 0;
	if(sTlkUsbModule[modType] == nullptr || sTlkUsbModule[modType]->pDesc == nullptr
		|| sTlkUsbModule[modType]->pDesc->GetStringLens == nullptr){
		return 0;
	}
	return sTlkUsbModule[modType]->pDesc->GetStringLens(index);
}
uint16 tlkusb_module_getReportLens(uint08 modType, uint08 index)
{
	if(modType >= TLKUSB_MODTYPE_MAX) return 0;
	if(sTlkUsbModule[modType] == nullptr || sTlkUsbModule[modType]->pDesc == nullptr
		|| sTlkUsbModule[modType]->pDesc->GetReportLens == nullptr){
		return 0;
	}
	return sTlkUsbModule[modType]->pDesc->GetReportLens(index);
}

uint08 *tlkusb_module_getDeviceDesc(uint08 modType)
{
	if(modType >= TLKUSB_MODTYPE_MAX) return nullptr;
	if(sTlkUsbModule[modType] == nullptr || sTlkUsbModule[modType]->pDesc == nullptr
		|| sTlkUsbModule[modType]->pDesc->GetDeviceDesc == nullptr){
		return nullptr;
	}
	return sTlkUsbModule[modType]->pDesc->GetDeviceDesc();
}
uint08 *tlkusb_module_getConfigDesc(uint08 modType)
{
	if(modType >= TLKUSB_MODTYPE_MAX) return nullptr;
	if(sTlkUsbModule[modType] == nullptr || sTlkUsbModule[modType]->pDesc == nullptr
		|| sTlkUsbModule[modType]->pDesc->GetConfigDesc == nullptr){
		return nullptr;
	}
	return sTlkUsbModule[modType]->pDesc->GetConfigDesc();
}
uint08 *tlkusb_module_getStringDesc(uint08 modType, uint08 index)
{
	if(modType >= TLKUSB_MODTYPE_MAX) return nullptr;
	if(sTlkUsbModule[modType] == nullptr || sTlkUsbModule[modType]->pDesc == nullptr
		|| sTlkUsbModule[modType]->pDesc->GetStringDesc == nullptr){
		return nullptr;
	}
	return sTlkUsbModule[modType]->pDesc->GetStringDesc(index);
}
uint08 *tlkusb_module_getReportDesc(uint08 modType, uint08 index)
{
	if(modType >= TLKUSB_MODTYPE_MAX) return nullptr;
	if(sTlkUsbModule[modType] == nullptr || sTlkUsbModule[modType]->pDesc == nullptr
		|| sTlkUsbModule[modType]->pDesc->GetReportDesc == nullptr){
		return nullptr;
	}
	return sTlkUsbModule[modType]->pDesc->GetReportDesc(index);
}



#endif

