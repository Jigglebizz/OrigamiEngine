#include "Origami/pch.h"
#include "Origami/Arch/ArchWin.h"

uint32_t Arch::GetNumberOfCores()
{
  SYSTEM_INFO sys_info;
  GetSystemInfo( &sys_info );
  return sys_info.dwNumberOfProcessors;
}