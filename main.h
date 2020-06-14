// ---------------------------------------------------------------------------
#ifndef mainH
#define mainH
// ---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <SvcMgr.hpp>
#include <vcl.h>
#include <Vcl.ExtCtrls.hpp>

// ---------------------------------------------------------------------------
class TMatsumotoYukihiro : public TService {
__published: // IDE-managed Components
	TTimer *Timer1;

	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall ServiceExecute(TService *Sender);

private: // User declarations
public: // User declarations
	__fastcall TMatsumotoYukihiro(TComponent* Owner);
	TServiceController __fastcall GetServiceController(void);

	friend void __stdcall ServiceController(unsigned CtrlCode);
};

// ---------------------------------------------------------------------------
extern PACKAGE TMatsumotoYukihiro *MatsumotoYukihiro;
// ---------------------------------------------------------------------------
#endif
