import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'permission_plus_windows_platform_interface.dart';

/// An implementation of [PermissionPlusWindowsPlatform] that uses method channels.
class MethodChannelPermissionPlusWindows extends PermissionPlusWindowsPlatform {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel = const MethodChannel('permission_plus_windows');

  @override
  Future<String?> getPlatformVersion() async {
    final version = await methodChannel.invokeMethod<String>(
      'getPlatformVersion',
    );
    return version;
  }
}
