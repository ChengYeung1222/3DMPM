#ifndef _MESH_UTILITY_DEF_H
#define _MESH_UTILITY_DEF_H

#ifdef _USRDLL
#	ifdef MESH_UTILITY_EXPORTS
#		define MESH_UTILITY_API __declspec(dllexport)
#	else	// MESH_UTILITY_EXPORTS
#		define MESH_UTILITY_API __declspec(dllimport)
#	endif	// MESH_UTILITY_EXPORTS
#else	// _USRDLL
#	define MESH_UTILITY_API
#endif	// _USRDLL

#endif // _MESH_UTILITY_DEF_H
