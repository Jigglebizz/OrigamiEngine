#pragma once

class GlobalSettings
{
  HeapTemplate* m_HeapTemplates;
  char*         m_HeapTemplateStrings;
  uint8_t       m_NumHeapTemplates;

public:
  enum ProjectType : uint8_t
  {
    kProjectTypeGame,
    kProjectTypeBuilder,
    kProjectTypeKami
  };

  void ENGINE_API Init( ProjectType project_type );
  void ENGINE_API Destroy();

  const HeapTemplate* GetHeapTemplate( const char* name );
};

extern GlobalSettings g_GameSettings;