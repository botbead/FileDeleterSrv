// ---------------------------------------------------------------------------
#include "main.h"
#include "clean_operation.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TMatsumotoYukihiro *MatsumotoYukihiro;

// ---------------------------------------------------------------------------
__fastcall TMatsumotoYukihiro::TMatsumotoYukihiro(TComponent* Owner)
	: TService(Owner) {
}

TServiceController __fastcall TMatsumotoYukihiro::GetServiceController(void) {
	return (TServiceController) ServiceController;
}

void __stdcall ServiceController(unsigned CtrlCode) {
	MatsumotoYukihiro->Controller(CtrlCode);
}

// ---------------------------------------------------------------------------
void __fastcall TMatsumotoYukihiro::Timer1Timer(TObject *Sender) {
	String process = "WSSERVER.EXE";
	exec_delete(&process);
}
// ---------------------------------------------------------------------------

void __fastcall TMatsumotoYukihiro::ServiceExecute(TService *Sender) {
	ServiceThread->ProcessRequests(true);
}
// ---------------------------------------------------------------------------
