#ifndef FLUTTER_PLUGIN_PERMISSION_PLUS_WINDOWS_PLUGIN_H_
#define FLUTTER_PLUGIN_PERMISSION_PLUS_WINDOWS_PLUGIN_H_

#include <flutter/plugin_registrar_windows.h>

#include <memory>

#include "permission_plus_api.g.h"

namespace permission_plus_windows {

/// Windows implementation of the PermissionPlusHostApi.
///
/// Windows desktop (Win32) apps generally don't have a mobile-style permission
/// model. Most permissions (contacts, calendar, storage, etc.) are freely
/// available to Win32 apps. Camera and microphone access is controlled via
/// Windows Privacy Settings, which we check using WinRT APIs where available.
class PermissionPlusWindowsPlugin : public flutter::Plugin,
                                     public PermissionPlusHostApi {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows* registrar);

  PermissionPlusWindowsPlugin();
  virtual ~PermissionPlusWindowsPlugin();

  // Disallow copy and assign.
  PermissionPlusWindowsPlugin(const PermissionPlusWindowsPlugin&) = delete;
  PermissionPlusWindowsPlugin& operator=(const PermissionPlusWindowsPlugin&) = delete;

  // PermissionPlusHostApi implementation
  void CheckPermission(
      const PermissionTypeMessage& permission,
      std::function<void(ErrorOr<PermissionStatusMessage> reply)> result) override;

  void RequestPermission(
      const PermissionTypeMessage& permission,
      std::function<void(ErrorOr<PermissionStatusMessage> reply)> result) override;

  void RequestPermissions(
      const flutter::EncodableList& permissions,
      std::function<void(ErrorOr<flutter::EncodableList> reply)> result) override;

  void OpenSettings(
      std::function<void(ErrorOr<bool> reply)> result) override;

  void ShouldShowRationale(
      const PermissionTypeMessage& permission,
      std::function<void(ErrorOr<bool> reply)> result) override;

  void GetLocationAccuracy(
      std::function<void(ErrorOr<LocationAccuracyMessage> reply)> result) override;

  void RequestTemporaryPreciseLocation(
      const std::string& purpose_key,
      std::function<void(ErrorOr<PermissionStatusMessage> reply)> result) override;

 private:
  /// Returns the permission status for the given permission type.
  PermissionStatusMessage GetPermissionStatus(
      const PermissionTypeMessage& permission);
};

}  // namespace permission_plus_windows

#endif  // FLUTTER_PLUGIN_PERMISSION_PLUS_WINDOWS_PLUGIN_H_
