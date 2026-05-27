import 'package:flutter_test/flutter_test.dart';
import 'package:permission_plus_windows/permission_plus_windows.dart';
import 'package:permission_plus_windows/permission_plus_windows_platform_interface.dart';
import 'package:permission_plus_windows/permission_plus_windows_method_channel.dart';
import 'package:plugin_platform_interface/plugin_platform_interface.dart';

class MockPermissionPlusWindowsPlatform
    with MockPlatformInterfaceMixin
    implements PermissionPlusWindowsPlatform {
  @override
  Future<String?> getPlatformVersion() => Future.value('42');
}

void main() {
  final PermissionPlusWindowsPlatform initialPlatform = PermissionPlusWindowsPlatform.instance;

  test('$MethodChannelPermissionPlusWindows is the default instance', () {
    expect(initialPlatform, isInstanceOf<MethodChannelPermissionPlusWindows>());
  });

  test('getPlatformVersion', () async {
    PermissionPlusWindows permissionPlusWindowsPlugin = PermissionPlusWindows();
    MockPermissionPlusWindowsPlatform fakePlatform = MockPermissionPlusWindowsPlatform();
    PermissionPlusWindowsPlatform.instance = fakePlatform;

    expect(await permissionPlusWindowsPlugin.getPlatformVersion(), '42');
  });
}
