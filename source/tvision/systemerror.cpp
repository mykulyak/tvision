#include <tvision/systemerror.h>

bool TSystemError::ctrlBreakHit = false;
bool TSystemError::saveCtrlBreak = false;

TSystemError::TSystemError() noexcept { resume(); }

TSystemError::~TSystemError() { suspend(); }

void TSystemError::resume() noexcept { THardwareInfo::setCtrlBrkHandler(TRUE); }

void TSystemError::suspend() noexcept { THardwareInfo::setCtrlBrkHandler(FALSE); }
