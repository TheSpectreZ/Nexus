#include "nxpch.h"
#include "Asset.h"

Nexus::Asset::Asset()
{
	m_Id = CreateUUID();
}