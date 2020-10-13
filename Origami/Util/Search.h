#pragma once

size_t ENGINE_API BinarySearch32( uint32_t value, const void* buf, size_t stride, size_t len );
size_t ENGINE_API BinarySearch64( uint64_t value, const void* buf, size_t stride, size_t len );