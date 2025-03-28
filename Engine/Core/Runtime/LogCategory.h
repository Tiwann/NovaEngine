#pragma once
#include "Containers/String.h"


#define NOVA_DECLARE_LOG_CATEGORY_STATIC(ClassName, CategoryName) \
struct ClassName##LogCategory \
{\
    static inline Nova::String s_CategoryName = CategoryName; \
};

