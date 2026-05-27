import 'package:plugin_platform_interface/plugin_platform_interface.dart';

import 'permission_plus_windows_method_channel.dart';

abstract class PermissionPlusWindowsPlatform extends PlatformInterface {
  /// Constructs a PermissionPlusWindowsPlatform.
  PermissionPlusWindowsPlatform() : super(token: _token);

  static final Object _token = Object();

  static PermissionPlusWindowsPlatform _instance = MethodChannelPermissionPlusWindows();

  /// The default instance of [PermissionPlusWindowsPlatform] to use.
  ///
  /// Defaults to [MethodChannelPermissionPlusWindows].
  static PermissionPlusWindowsPlatform get instance => _instance;

  /// Platform-specific implementations should set this with their own
  /// platform-specific class that extends [PermissionPlusWindowsPlatform] when
  /// they register themselves.
  static set instance(PermissionPlusWindowsPlatform instance) {
    PlatformInterface.verifyToken(instance, _token);
    _instance = instance;
  }

  Future<String?> getPlatformVersion() {
    throw UnimplementedError('platformVersion() has not been implemented.');
  }
}
