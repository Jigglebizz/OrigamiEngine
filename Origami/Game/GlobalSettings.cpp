#include "Origami/pch.h"
#include "Origami/Game/GlobalSettings.h"

#include "Origami/Filesystem/Filesystem.h"
#include "Origami/Memory/Memory.h"
#include "Origami/Util/Sort.h"
#include "Origami/Util/Search.h"
#include "Origami/Util/StringUtil.h"
#include "Origami/Util/Log.h"

#include <rapidjson/writer.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>

DISABLE_OPTS

GlobalSettings g_GlobalSettings;

//---------------------------------------------------------------------------------
static constexpr char   kMemoryDesc[] = "Memory";
static constexpr char   kBudgetDesc[] = "Budget";
static constexpr char   kCommonDesc[] = "Common";

//---------------------------------------------------------------------------------
size_t ParseStringSize( const char* string_size )
{
  unsigned digit_len = 0;
  size_t   size_value = 0;
  bool     calculated_value = false;

  char     value_str[6];

  size_t str_len = StrLen( string_size );
  for ( unsigned i_char = 0; i_char < str_len; ++i_char )
  {
    if ( string_size[ i_char ] >= '0' && string_size[ i_char ] <= '9' )
    {
      value_str[ digit_len++] = string_size[ i_char ];
      ASSERT_MSG( digit_len < 6, "Heap size value is too large. Use size suffixes to shorten" );
    }
    else
    {
      if ( calculated_value == false )
      {
        value_str[ digit_len ] = '\0';
        size_value = atoll( value_str );
        calculated_value = true;
      }

      if ( digit_len == str_len || strncmp( &string_size[ i_char ], "B", 2) == 0 )
      {
        return size_value;
      }
      else if ( strncmp( &string_size[ i_char ], "KB", 3) == 0 )
      {
        return size_value * 1024;
      }
      else if ( strncmp( &string_size[ i_char ], "MB", 3) == 0 )
      {
        return size_value * 1024 * 1024;
      }
      else if ( strncmp( & string_size[ i_char ], "GB", 3) == 0 )
      {
        return size_value * 1024 * 1024 * 1024;
      }
    }

  }

  ASSERT_ALWAYS( "string size parameter in .set file cannot be parsed" );
  return 0;
}

//---------------------------------------------------------------------------------
void ToStringSize( size_t mem_size, char* str, size_t str_len )
{
  uint8_t order_of_magnitude = 0;
  while ( mem_size % 1024 == 0 )
  {
    order_of_magnitude++;
    mem_size /= 1024;
  }

  const char* order_prefix = "";
  switch ( order_of_magnitude )
  {
  case 0:
    order_prefix = "";
    break;
  case 1:
    order_prefix = "K";
    break;
  case 2:
    order_prefix = "M";
    break;
  case 3:
    order_prefix = "G";
    break;
  case 4:
    order_prefix = "T";
    break;
  default:
    ASSERT_ALWAYS("That number is way too big");
  }

  snprintf( str, str_len, "%llu %sB", mem_size, order_prefix );
}

//---------------------------------------------------------------------------------
const char* ProjectTypeToJsonDesc( GlobalSettings::ProjectType project_type )
{
  switch ( project_type )
  {
    case GlobalSettings::kProjectTypeGame:
      return "Game";
    case GlobalSettings::kProjectTypeBuilder:
      return "Builder";
    case GlobalSettings::kProjectTypeKami:
      return "Kami";
    default:
      return "Unknown";
  }
}

//---------------------------------------------------------------------------------
template< typename Cb >
void DoForBudgetHelper( rapidjson::Value::ConstMemberIterator template_itr, Cb callback )
{
  HeapTemplate heap_template;

  heap_template.m_Name = template_itr->name.GetString();
  const rapidjson::Value& template_value = template_itr->value;
  for ( rapidjson::Value::ConstMemberIterator template_member_itr = template_value.MemberBegin(); template_member_itr != template_value.MemberEnd(); ++template_member_itr )
  {
    const char* member_name = template_member_itr->name.GetString();
    if ( strncmp( member_name, kBudgetDesc, sizeof( kBudgetDesc ) ) == 0 )
    {
      const char* budget_size_str = template_member_itr->value.GetString();
      heap_template.m_Size = ParseStringSize( budget_size_str );
      callback( &heap_template );
    }
  }
}

//---------------------------------------------------------------------------------
template< typename Cb >
void DoForEachBudget( rapidjson::Document& doc, GlobalSettings::ProjectType project_type, Cb callback)
{
  const char* project_type_desc = ProjectTypeToJsonDesc( project_type );

  const rapidjson::Value& project_types    = doc[ kMemoryDesc ];
  const rapidjson::Value& common_templates = project_types[ kCommonDesc ];
  const rapidjson::Value* spec_templates   = nullptr;

  if ( project_types.HasMember( project_type_desc) )
  {
    spec_templates = &project_types[ project_type_desc ];
  }

  for ( rapidjson::Value::ConstMemberIterator template_itr = common_templates.MemberBegin(); template_itr != common_templates.MemberEnd(); ++template_itr )
  {
    if ( spec_templates != nullptr && spec_templates->HasMember( template_itr->name ) )
    {
      continue;
    }

    DoForBudgetHelper( template_itr, callback );
  }

  if ( spec_templates != nullptr )
  {
    for ( rapidjson::Value::ConstMemberIterator template_itr = spec_templates->MemberBegin(); template_itr != spec_templates->MemberEnd(); ++template_itr )
    {
      DoForBudgetHelper( template_itr, callback );
    }
  }
}

//---------------------------------------------------------------------------------
void GlobalSettings::Init( ProjectType project_type )
{
  m_NumHeapTemplates = 0;

  char engine_settings_file[ Filesystem::kMaxPathLen ];
  snprintf( engine_settings_file, sizeof( engine_settings_file ), "%s\\required\\engine.set", Filesystem::GetAssetsSourcePath() );
  
  size_t file_size = Filesystem::GetFileSize( engine_settings_file );
  char* engine_settings_file_contents = (char*)malloc( file_size );

  Filesystem::ReadFile( engine_settings_file, engine_settings_file_contents, &file_size );

  rapidjson::Document doc;
  doc.Parse< rapidjson::kParseStopWhenDoneFlag >( engine_settings_file_contents );

  size_t   total_memory_size = 0;
  uint32_t strings_size = 0;
  DoForEachBudget( doc, project_type, [ this, &total_memory_size, &strings_size ]( const HeapTemplate* heap_template ) {
    total_memory_size += heap_template->m_Size;
    strings_size      += StrLen( heap_template->m_Name ) + 1;
    m_NumHeapTemplates++;
  } );
  
  Memory::InitGlobalBacking( total_memory_size );

  m_HeapTemplates       = (HeapTemplate*)g_DynamicHeap.Alloc( sizeof( HeapTemplate ) * m_NumHeapTemplates );
  m_HeapTemplateStrings = (char*)g_DynamicHeap.Alloc( strings_size );

  uint8_t  heap_idx = 0;
  char*    strings_ptr = m_HeapTemplateStrings;
  uint32_t max_str_len = 0;
  DoForEachBudget( doc, project_type, [ this, &heap_idx, &strings_ptr, &max_str_len ]( const HeapTemplate* heap_template )
  {
    uint32_t str_len = StrLen( heap_template->m_Name ) + 1;
    max_str_len = ( str_len > max_str_len ) ? str_len : max_str_len;
    StrCpy( strings_ptr, heap_template->m_Name );

    HeapTemplate* current_template = &m_HeapTemplates[ heap_idx ];
    current_template->m_Size     = heap_template->m_Size;
    current_template->m_Name     = strings_ptr;
    current_template->m_NameHash = Crc32( current_template->m_Name );

    strings_ptr += str_len;
    heap_idx++;
  } );

  QuickSort32( &m_HeapTemplates[ 0 ], sizeof( HeapTemplate ), m_NumHeapTemplates );

  #if BUILD < BUILD_FINAL
  {
    Log::LogInfo( "Using Memory Budgets:\n" );
    Log::LogInfo( "================================\n" );
    char budget_fmt[ 25 ];
    snprintf( budget_fmt, sizeof( budget_fmt ), "%%-%ds: %%s\n", max_str_len );

    for ( uint8_t i_template = 0; i_template < m_NumHeapTemplates; ++i_template )
    {
      HeapTemplate* cur_template = &m_HeapTemplates[ i_template ];
      char str_size[32];
      ToStringSize( cur_template->m_Size, str_size, sizeof( str_size ) );
      Log::LogInfo (budget_fmt, cur_template->m_Name, str_size);
    }
    Log::LogInfo("================================\n\n");

  }
  #endif

  free( engine_settings_file_contents );
}

//---------------------------------------------------------------------------------
void GlobalSettings::Destroy()
{
  g_DynamicHeap.Free( m_HeapTemplates );
  Memory::DestroyGlobalBacking();
}

//---------------------------------------------------------------------------------
const HeapTemplate* GlobalSettings::GetHeapTemplate( const char* name )
{
  uint32_t name_hash = Crc32( name );
  size_t idx = BinarySearch32( name_hash, m_HeapTemplates, sizeof( HeapTemplate ), m_NumHeapTemplates );
  ASSERT_MSG( idx != -1, "Heap template not found" );

  return &m_HeapTemplates[ idx ];
}