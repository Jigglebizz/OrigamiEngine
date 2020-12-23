#pragma once

#include "Origami/Container/HashMap.h"

class GlobalSettings
{
public:
  enum ProjectType : uint8_t
  {
    kProjectTypeGame,
    kProjectTypeBuilder,
    kProjectTypeKami
  };

private:
  HeapTemplate*                    m_HeapTemplates;
  char*                            m_HeapTemplateStrings;
  uint8_t                          m_NumHeapTemplates;

  HashMap< uint32_t, uint32_t >    m_U32Values;
  HashMap< uint32_t, const char* > m_StringValues;
  char*                            m_Strings;

public:

  void ENGINE_API Init( ProjectType project_type );
  void ENGINE_API Destroy();

  const HeapTemplate* GetHeapTemplate ( const char* name );
        uint32_t      GetU32          ( uint32_t property_name_hash );
  const char*         GetString       ( uint32_t property_name_hash );
};

extern GlobalSettings g_GlobalSettings;