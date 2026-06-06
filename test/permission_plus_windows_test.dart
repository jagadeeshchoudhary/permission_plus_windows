import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:permission_plus_platform_interface/permission_plus_platform_interface.dart';
import 'package:permission_plus_windows/permission_plus_windows.dart';
import 'package:permission_plus_windows/src/generated/permission_plus_api.g.dart';

class FakePermissionPlusHostApi implements PermissionPlusHostApi {
  PermissionStatusMessage checkPermissionResult =
      PermissionStatusMessage.granted;
  PermissionStatusMessage requestPermissionResult =
      PermissionStatusMessage.granted;
  List<PermissionStatusMapEntry> requestPermissionsResult = [];
  bool openSettingsResult = true;
  bool shouldShowRationaleResult = false;
  LocationAccuracyMessage getLocationAccuracyResult =
      LocationAccuracyMessage.precise;
  PermissionStatusMessage requestTemporaryPreciseLocationResult =
      PermissionStatusMessage.granted;

  PermissionTypeMessage? lastCheckedPermission;
  PermissionTypeMessage? lastRequestedPermission;
  List<PermissionTypeMessage>? lastRequestedPermissions;
  PermissionTypeMessage? lastShouldShowRationalePermission;
  String? lastPurposeKey;

  @override
  Future<PermissionStatusMessage> checkPermission(
    PermissionTypeMessage permission,
  ) async {
    lastCheckedPermission = permission;
    return checkPermissionResult;
  }

  @override
  Future<PermissionStatusMessage> requestPermission(
    PermissionTypeMessage permission,
  ) async {
    lastRequestedPermission = permission;
    return requestPermissionResult;
  }

  @override
  Future<List<PermissionStatusMapEntry>> requestPermissions(
    List<PermissionTypeMessage> permissions,
  ) async {
    lastRequestedPermissions = permissions;
    return requestPermissionsResult;
  }

  @override
  Future<bool> openSettings() async {
    return openSettingsResult;
  }

  @override
  Future<bool> shouldShowRationale(PermissionTypeMessage permission) async {
    lastShouldShowRationalePermission = permission;
    return shouldShowRationaleResult;
  }

  @override
  Future<LocationAccuracyMessage> getLocationAccuracy() async {
    return getLocationAccuracyResult;
  }

  @override
  Future<PermissionStatusMessage> requestTemporaryPreciseLocation(
    String purposeKey,
  ) async {
    lastPurposeKey = purposeKey;
    return requestTemporaryPreciseLocationResult;
  }

  @override
  // ignore: non_constant_identifier_names
  String get pigeonVar_messageChannelSuffix => '';

  @override
  // ignore: non_constant_identifier_names
  BinaryMessenger? get pigeonVar_binaryMessenger => null;
}

void main() {
  late PermissionPlusWindows platform;
  late FakePermissionPlusHostApi fakeApi;

  setUp(() {
    fakeApi = FakePermissionPlusHostApi();
    platform = PermissionPlusWindows(api: fakeApi);
  });

  test('registerWith sets instance', () {
    PermissionPlusWindows.registerWith();
    expect(PermissionPlusPlatform.instance, isA<PermissionPlusWindows>());
  });

  test('checkPermission returns mapped status', () async {
    fakeApi.checkPermissionResult = PermissionStatusMessage.denied;
    final result = await platform.checkPermission(PermissionType.camera);
    expect(result, PermissionStatus.denied);
    expect(fakeApi.lastCheckedPermission, PermissionTypeMessage.camera);
  });

  test('requestPermission returns mapped status', () async {
    fakeApi.requestPermissionResult = PermissionStatusMessage.granted;
    final result = await platform.requestPermission(PermissionType.location);
    expect(result, PermissionStatus.granted);
    expect(fakeApi.lastRequestedPermission, PermissionTypeMessage.location);
  });

  test('requestPermissions returns mapped map', () async {
    fakeApi.requestPermissionsResult = [
      PermissionStatusMapEntry(
        permission: PermissionTypeMessage.camera,
        status: PermissionStatusMessage.granted,
      ),
      PermissionStatusMapEntry(
        permission: PermissionTypeMessage.microphone,
        status: PermissionStatusMessage.denied,
      ),
    ];
    final result = await platform.requestPermissions([
      PermissionType.camera,
      PermissionType.microphone,
    ]);
    expect(result, {
      PermissionType.camera: PermissionStatus.granted,
      PermissionType.microphone: PermissionStatus.denied,
    });
    expect(fakeApi.lastRequestedPermissions, [
      PermissionTypeMessage.camera,
      PermissionTypeMessage.microphone,
    ]);
  });

  test('openSettings returns result', () async {
    fakeApi.openSettingsResult = true;
    final result = await platform.openSettings();
    expect(result, isTrue);

    fakeApi.openSettingsResult = false;
    final result2 = await platform.openSettings();
    expect(result2, isFalse);
  });

  test('shouldShowRationale returns result', () async {
    fakeApi.shouldShowRationaleResult = true;
    final result = await platform.shouldShowRationale(PermissionType.camera);
    expect(result, isTrue);
    expect(
      fakeApi.lastShouldShowRationalePermission,
      PermissionTypeMessage.camera,
    );
  });

  test('getLocationAccuracy returns precise', () async {
    fakeApi.getLocationAccuracyResult = LocationAccuracyMessage.precise;
    final result = await platform.getLocationAccuracy();
    expect(result, LocationAccuracy.precise);
  });

  test('getLocationAccuracy returns reduced', () async {
    fakeApi.getLocationAccuracyResult = LocationAccuracyMessage.reduced;
    final result = await platform.getLocationAccuracy();
    expect(result, LocationAccuracy.reduced);
  });

  test('requestTemporaryPreciseLocation returns status', () async {
    fakeApi.requestTemporaryPreciseLocationResult =
        PermissionStatusMessage.granted;
    final result = await platform.requestTemporaryPreciseLocation(
      purposeKey: 'purpose',
    );
    expect(result, PermissionStatus.granted);
    expect(fakeApi.lastPurposeKey, 'purpose');
  });

  test('permissionStatusStream throws UnimplementedError', () {
    expect(
      () => platform.permissionStatusStream(PermissionType.camera),
      throwsUnimplementedError,
    );
  });

  group('Enum Mappings', () {
    test('PermissionType mapping is correct for all values', () async {
      for (final type in PermissionType.values) {
        await platform.requestPermission(type);
        expect(fakeApi.lastRequestedPermission?.index, type.index);
      }
    });

    test('PermissionStatus mapping is correct for all values', () async {
      for (final status in PermissionStatusMessage.values) {
        fakeApi.checkPermissionResult = status;
        final result = await platform.checkPermission(PermissionType.camera);
        expect(result.index, status.index);
      }
    });
  });
}
