
import 'permission_plus_windows_platform_interface.dart';

class PermissionPlusWindows {
  Future<String?> getPlatformVersion() {
    return PermissionPlusWindowsPlatform.instance.getPlatformVersion();
  }
}
