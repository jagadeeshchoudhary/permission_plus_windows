#include "include/permission_plus_windows/permission_plus_windows_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "permission_plus_windows_plugin.h"

void PermissionPlusWindowsPluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  permission_plus_windows::PermissionPlusWindowsPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
