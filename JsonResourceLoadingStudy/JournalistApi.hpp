#pragma once

// DLL export/import helper.
//
// JsonResourceLoadingStudy.vcxproj defines CPPDYNAMICLIBRARYTEMPLATE_EXPORTS when building the DLL.
// Consuming projects should NOT define it, so they will import the symbols.

#if defined(JOURNALIST_STATIC)
#  define JOURNALIST_API
#elif defined(_WIN32)
#  if defined(CPPDYNAMICLIBRARYTEMPLATE_EXPORTS)
#    define JOURNALIST_API __declspec(dllexport)
#  else
#    define JOURNALIST_API __declspec(dllimport)
#  endif
#else
#  define JOURNALIST_API
#endif

