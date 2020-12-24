#pragma once

class AssetId
{
  uint32_t m_Value;
  friend const bool operator==( const AssetId&, const AssetId& );
  friend const bool operator!=( const AssetId&, const AssetId& );
public:
  static const AssetId kInvalidAssetId;

  //---------------------------------------------------------------------------------
  static inline constexpr AssetId  ENGINE_API FromAssetPath ( const char* path );
  static inline constexpr AssetId  ENGINE_API FromU32       ( uint32_t value   );
  
  //---------------------------------------------------------------------------------
         inline           uint32_t ENGINE_API ToU32         ( ) const;
  
};

//---------------------------------------------------------------------------------
constexpr AssetId AssetId::FromAssetPath( const char* path )
{
  return FromU32( Crc32(path) );
}

//---------------------------------------------------------------------------------
constexpr AssetId AssetId::FromU32( uint32_t value )
{
  AssetId asset;
  asset.m_Value = value;
  return asset;
}

//---------------------------------------------------------------------------------
uint32_t AssetId::ToU32() const
{
  return m_Value;
}

//---------------------------------------------------------------------------------
inline const bool operator==( const AssetId& a, const AssetId& b )
{
  return a.m_Value == b.m_Value;
}

//---------------------------------------------------------------------------------
inline const bool operator!=( const AssetId& a, const AssetId& b )
{
  return a.m_Value != b.m_Value;
}