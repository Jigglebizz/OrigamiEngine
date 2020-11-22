#include "Origami/pch.h"
#include "BuilderCommon.h"
#include "Origami/Filesystem/Filesystem.h"

#include <rapidjson/writer.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>

//---------------------------------------------------------------------------------
MemAllocHeap BuilderCommon::g_DynamicHeap;

//---------------------------------------------------------------------------------
void BuilderCommon::Init()
{
  g_DynamicHeap.InitWithBacking( g_DynamicHeapBacking, sizeof( g_DynamicHeapBacking ), "Builder Commong Heap" );
}

//---------------------------------------------------------------------------------
void BuilderCommon::Destroy()
{
  g_DynamicHeap.Destroy();
}

//---------------------------------------------------------------------------------
void BuilderCommon::ParseAsset( const char* asset_file, AssetCommonData* asset_data_out )
{
  uint64_t json_size = Filesystem::GetFileSize( asset_file );
  char* json_data    = (char*)g_DynamicHeap.Alloc( json_size );

  Filesystem::ReadFile( asset_file, json_data, &json_size );

  rapidjson::Document doc;
  doc.Parse< rapidjson::kParseStopWhenDoneFlag >( json_data );

  {
    MemZero( asset_data_out->m_BuildDependents, sizeof( asset_data_out->m_BuildDependents ) );
    uint32_t dependents_count = 0;

    rapidjson::GenericArray< false, rapidjson::Value > dependents =  doc[ "Build Dependents" ].GetArray();
    for ( uint32_t i_dependency = 0; i_dependency < dependents.Size(); i_dependency++ )
    {
      const char* dep = dependents[ i_dependency ].GetString();
      asset_data_out->m_BuildDependents[ dependents_count++ ] = AssetId::FromAssetPath( dep );
    }

    asset_data_out->m_BuildDependentsCount = dependents_count;
  }

  {
    MemZero( asset_data_out->m_LoadDependencies, sizeof( asset_data_out->m_LoadDependencies ) );
    uint32_t dependencies_count = 0;

    rapidjson::GenericArray< false, rapidjson::Value > dependencies = doc[ "Load Dependencies" ].GetArray();
    for ( uint32_t i_dependency = 0; i_dependency < dependencies.Size(); i_dependency++ )
    {
      const char* dep = dependencies[ i_dependency ].GetString();
      asset_data_out->m_LoadDependencies[ dependencies_count++ ] = AssetId::FromAssetPath( dep );
    }

    asset_data_out->m_LoadDependenciesCount = dependencies_count;
  }

  g_DynamicHeap.Free( json_data );
}