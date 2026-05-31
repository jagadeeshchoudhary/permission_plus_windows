#include "permission_plus_windows_plugin.h"

#include <windows.h>
#include <shellapi.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Devices.Enumeration.h>
#include <winrt/Windows.Devices.Geolocation.h>

#include <flutter/plugin_registrar_windows.h>

#include <memory>
#include <string>
#include <sstream>

#pragma comment(lib, "windowsapp")

namespace permission_plus_windows {

using flutter::EncodableList;
using flutter::EncodableValue;

// static
void PermissionPlusWindowsPlugin::RegisterWithRegistrar(
    flutter::PluginRegistrarWindows* registrar) {
  auto plugin = std::make_unique<PermissionPlusWindowsPlugin>();

  PermissionPlusHostApi::SetUp(registrar->messenger(), plugin.get());

  registrar->AddPlugin(std::move(plugin));
}

PermissionPlusWindowsPlugin::PermissionPlusWindowsPlugin() {
  winrt::init_apartment(winrt::apartment_type::single_threaded);
}

PermissionPlusWindowsPlugin::~PermissionPlusWindowsPlugin() {}

// ── Private Helpers ──────────────────────────────────────────────────────

PermissionStatusMessage PermissionPlusWindowsPlugin::GetPermissionStatus(
    const PermissionTypeMessage& permission) {
  switch (permission) {
    // Camera: check via WinRT DeviceAccessInformation
    case PermissionTypeMessage::kCamera: {
      try {
        auto access_info =
            winrt::Windows::Devices::Enumeration::DeviceAccessInformation::
                CreateFromDeviceClass(
                    winrt::Windows::Devices::Enumeration::DeviceClass::
                        VideoCapture);
        auto status = access_info.CurrentStatus();
        switch (status) {
          case winrt::Windows::Devices::Enumeration::DeviceAccessStatus::
              Allowed:
            return PermissionStatusMessage::kGranted;
          case winrt::Windows::Devices::Enumeration::DeviceAccessStatus::
              DeniedByUser:
            return PermissionStatusMessage::kPermanentlyDenied;
          case winrt::Windows::Devices::Enumeration::DeviceAccessStatus::
              DeniedBySystem:
            return PermissionStatusMessage::kRestricted;
          case winrt::Windows::Devices::Enumeration::DeviceAccessStatus::
              Unspecified:
          default:
            return PermissionStatusMessage::kNotDetermined;
        }
      } catch (...) {
        // WinRT not available or no camera device — assume granted
        return PermissionStatusMessage::kGranted;
      }
    }

    // Microphone: check via WinRT DeviceAccessInformation
    case PermissionTypeMessage::kMicrophone: {
      try {
        auto access_info =
            winrt::Windows::Devices::Enumeration::DeviceAccessInformation::
                CreateFromDeviceClass(
                    winrt::Windows::Devices::Enumeration::DeviceClass::
                        AudioCapture);
        auto status = access_info.CurrentStatus();
        switch (status) {
          case winrt::Windows::Devices::Enumeration::DeviceAccessStatus::
              Allowed:
            return PermissionStatusMessage::kGranted;
          case winrt::Windows::Devices::Enumeration::DeviceAccessStatus::
              DeniedByUser:
            return PermissionStatusMessage::kPermanentlyDenied;
          case winrt::Windows::Devices::Enumeration::DeviceAccessStatus::
              DeniedBySystem:
            return PermissionStatusMessage::kRestricted;
          case winrt::Windows::Devices::Enumeration::DeviceAccessStatus::
              Unspecified:
          default:
            return PermissionStatusMessage::kNotDetermined;
        }
      } catch (...) {
        return PermissionStatusMessage::kGranted;
      }
    }

    // Location: check via WinRT Geolocator
    case PermissionTypeMessage::kLocation:
    case PermissionTypeMessage::kLocationAlways:
    case PermissionTypeMessage::kLocationWhenInUse: {
      try {
        auto access_status =
            winrt::Windows::Devices::Geolocation::Geolocator::
                RequestAccessAsync()
                    .get();
        switch (access_status) {
          case winrt::Windows::Devices::Geolocation::GeolocationAccessStatus::
              Allowed:
            return PermissionStatusMessage::kGranted;
          case winrt::Windows::Devices::Geolocation::GeolocationAccessStatus::
              Denied:
            return PermissionStatusMessage::kPermanentlyDenied;
          case winrt::Windows::Devices::Geolocation::GeolocationAccessStatus::
              Unspecified:
          default:
            return PermissionStatusMessage::kNotDetermined;
        }
      } catch (...) {
        return PermissionStatusMessage::kGranted;
      }
    }

    // Notification: Windows desktop apps can always send notifications
    case PermissionTypeMessage::kNotification:
    case PermissionTypeMessage::kCriticalAlerts:
      return PermissionStatusMessage::kGranted;

    // Bluetooth: check via WinRT DeviceAccessInformation
    case PermissionTypeMessage::kBluetooth: {
      try {
        // Use the Bluetooth device selector GUID
        auto access_info =
            winrt::Windows::Devices::Enumeration::DeviceAccessInformation::
                CreateFromId(L"{e0cbf06c-cd8b-4647-bb8a-263b43f0f974}");
        auto status = access_info.CurrentStatus();
        switch (status) {
          case winrt::Windows::Devices::Enumeration::DeviceAccessStatus::
              Allowed:
            return PermissionStatusMessage::kGranted;
          case winrt::Windows::Devices::Enumeration::DeviceAccessStatus::
              DeniedByUser:
            return PermissionStatusMessage::kPermanentlyDenied;
          case winrt::Windows::Devices::Enumeration::DeviceAccessStatus::
              DeniedBySystem:
            return PermissionStatusMessage::kRestricted;
          case winrt::Windows::Devices::Enumeration::DeviceAccessStatus::
              Unspecified:
          default:
            return PermissionStatusMessage::kNotDetermined;
        }
      } catch (...) {
        return PermissionStatusMessage::kGranted;
      }
    }

    // The following permissions have no Windows equivalent.
    // Win32 desktop apps have unrestricted access to these resources.
    case PermissionTypeMessage::kPhotos:
    case PermissionTypeMessage::kPhotosAddOnly:
    case PermissionTypeMessage::kContacts:
    case PermissionTypeMessage::kContactsReadOnly:
    case PermissionTypeMessage::kContactsWriteOnly:
    case PermissionTypeMessage::kCalendar:
    case PermissionTypeMessage::kCalendarReadOnly:
    case PermissionTypeMessage::kCalendarWriteOnly:
    case PermissionTypeMessage::kReminders:
    case PermissionTypeMessage::kStorage:
    case PermissionTypeMessage::kStorageReadOnly:
    case PermissionTypeMessage::kStorageWriteOnly:
    case PermissionTypeMessage::kSpeech:
    case PermissionTypeMessage::kMediaLibrary:
    case PermissionTypeMessage::kSensors:
    case PermissionTypeMessage::kPhone:
    case PermissionTypeMessage::kSms:
    case PermissionTypeMessage::kAppTrackingTransparency:
    case PermissionTypeMessage::kVideos:
    case PermissionTypeMessage::kAudio:
    default:
      return PermissionStatusMessage::kGranted;
  }
}

// ── PermissionPlusHostApi Implementation ──────────────────────────────────

void PermissionPlusWindowsPlugin::CheckPermission(
    const PermissionTypeMessage& permission,
    std::function<void(ErrorOr<PermissionStatusMessage> reply)> result) {
  result(GetPermissionStatus(permission));
}

void PermissionPlusWindowsPlugin::RequestPermission(
    const PermissionTypeMessage& permission,
    std::function<void(ErrorOr<PermissionStatusMessage> reply)> result) {
  // On Windows, there's no native permission dialog for Win32 apps.
  // We check the current status — if denied, the user must change it
  // in Windows Settings (Privacy & Security).
  auto status = GetPermissionStatus(permission);

  // If location is not determined, requesting access triggers the WinRT dialog
  if ((permission == PermissionTypeMessage::kLocation ||
       permission == PermissionTypeMessage::kLocationAlways ||
       permission == PermissionTypeMessage::kLocationWhenInUse) &&
      status == PermissionStatusMessage::kNotDetermined) {
    try {
      auto access_status =
          winrt::Windows::Devices::Geolocation::Geolocator::
              RequestAccessAsync()
                  .get();
      switch (access_status) {
        case winrt::Windows::Devices::Geolocation::GeolocationAccessStatus::
            Allowed:
          result(PermissionStatusMessage::kGranted);
          return;
        case winrt::Windows::Devices::Geolocation::GeolocationAccessStatus::
            Denied:
          result(PermissionStatusMessage::kPermanentlyDenied);
          return;
        default:
          result(PermissionStatusMessage::kNotDetermined);
          return;
      }
    } catch (...) {
      result(PermissionStatusMessage::kGranted);
      return;
    }
  }

  result(status);
}

void PermissionPlusWindowsPlugin::RequestPermissions(
    const EncodableList& permissions,
    std::function<void(ErrorOr<EncodableList> reply)> result) {
  EncodableList entries;

  for (const auto& perm_value : permissions) {
    auto perm = static_cast<PermissionTypeMessage>(std::get<int>(perm_value));
    auto status = GetPermissionStatus(perm);
    auto entry = PermissionStatusMapEntry(perm, status);
    entries.push_back(flutter::CustomEncodableValue(entry));
  }

  result(entries);
}

void PermissionPlusWindowsPlugin::OpenSettings(
    std::function<void(ErrorOr<bool> reply)> result) {
  // Open Windows Settings > Privacy & Security
  HINSTANCE res = ShellExecuteW(
      nullptr, L"open", L"ms-settings:privacy", nullptr, nullptr, SW_SHOW);
  bool success = reinterpret_cast<intptr_t>(res) > 32;
  result(success);
}

void PermissionPlusWindowsPlugin::ShouldShowRationale(
    const PermissionTypeMessage& permission,
    std::function<void(ErrorOr<bool> reply)> result) {
  // Windows has no "show rationale" concept
  result(false);
}

void PermissionPlusWindowsPlugin::GetLocationAccuracy(
    std::function<void(ErrorOr<LocationAccuracyMessage> reply)> result) {
  // Windows always provides precise location when granted
  result(LocationAccuracyMessage::kPrecise);
}

void PermissionPlusWindowsPlugin::RequestTemporaryPreciseLocation(
    const std::string& purpose_key,
    std::function<void(ErrorOr<PermissionStatusMessage> reply)> result) {
  // Not applicable on Windows — return current location status
  result(GetPermissionStatus(PermissionTypeMessage::kLocation));
}

}  // namespace permission_plus_windows
