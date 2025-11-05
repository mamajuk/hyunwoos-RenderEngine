#include "UniqueableObject.h"

std::vector<hyunwoo::UniqueableObject::MemoryUsageDescriptor> hyunwoo::UniqueableObject::m_descs         = { hyunwoo::UniqueableObject::MemoryUsageDescriptor(nullptr, 1) };
std::vector<uint32_t>									      hyunwoo::UniqueableObject::m_free_list     = { 0 };