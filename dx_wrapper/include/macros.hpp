#pragma once

template <typename T>
void** MyIID_PPV_ARGS_Helper(T** pp)
{ return reinterpret_cast<void**>(pp); }

#define IID_PPV_ARGS(ppType) __uuidof(**(ppType)), MyIID_PPV_ARGS_Helper(ppType)
