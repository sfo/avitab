/*
 *   AviTab - Aviator's Virtual Tablet
 *   Copyright (C) 2024 Folke Will <folko@solhost.org>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Affero General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Affero General Public License for more details.
 *
 *   You should have received a copy of the GNU Affero General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "DataRefExport.h"

#include <XPLM/XPLMPlugin.h>

# define MSG_ADD_DATAREF 0x01000000

namespace avitab {

template <>
DataRefExport<int>::DataRefExport(const std::string &name, void *ref, std::function<int(void *)> onRd, std::function<void(void *, int)> onWr)
:   ownerRef(ref), onRead(onRd), onWrite(onWr)
{
    xpDataRef = XPLMRegisterDataAccessor(name.c_str(), xplmType_Int, onWr != nullptr,
        [] (void *r) { auto self = reinterpret_cast<DataRefExport<int> *>(r); return self->onRead(self->ownerRef); },
        onWr ? [] (void *r, int v) { auto self = reinterpret_cast<DataRefExport<int> *>(r); self->onWrite(self->ownerRef, v); } : (XPLMSetDatai_f)nullptr,
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        this, onWr? this : nullptr
    );
    registerDataRef(name);
}

template <>
DataRefExport<int>::DataRefExport(const std::string &name, void *ref, std::function<int(void *)> onRd)
: DataRefExport<int>(name, ref, onRd, nullptr)
{
}

template <>
DataRefExport<float>::DataRefExport(const std::string &name, void *ref, std::function<float(void *)> onRd, std::function<void(void *, float)> onWr)
:   ownerRef(ref), onRead(onRd), onWrite(onWr)
{
    xpDataRef = XPLMRegisterDataAccessor(name.c_str(), xplmType_Float, onWr != nullptr,
        nullptr, nullptr,
        [] (void *r) { auto self = reinterpret_cast<DataRefExport<float> *>(r); return self->onRead(self->ownerRef); },
        onWr ? [] (void *r, float v) { auto self = reinterpret_cast<DataRefExport<float> *>(r); self->onWrite(self->ownerRef, v); } : (XPLMSetDataf_f)nullptr,
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        this, onWr ? this : nullptr
    );
    registerDataRef(name);
}

template <>
DataRefExport<float>::DataRefExport(const std::string &name, void *ref, std::function<float(void *)> onRd)
:   DataRefExport<float>(name, ref, onRd, nullptr)
{
}

template <typename T>
DataRefExport<T>::~DataRefExport()
{
    XPLMUnregisterDataAccessor(xpDataRef);
}

template <typename T>
void DataRefExport<T>::registerDataRef(const std::string &name) {
    XPLMPluginID PluginID = XPLMFindPluginBySignature("xplanesdk.examples.DataRefEditor");
    if (PluginID != XPLM_NO_PLUGIN_ID){
        XPLMSendMessageToPlugin(PluginID, MSG_ADD_DATAREF, (void*)name.c_str());
    }
}

template class DataRefExport<int>;
template class DataRefExport<float>;

}
