# permission_plus_windows

The Windows implementation of [`permission_plus`](https://github.com/jagadeeshchoudhary/permission_plus).

## Usage

This package is [endorsed](https://dart.dev/tools/pub/dependencies#endorsed-packages) and will be automatically included when you depend on `permission_plus`. There is no need to add this package to your `pubspec.yaml` directly.

**Users should depend on [`permission_plus`](https://pub.dev/packages/permission_plus) instead of this package.**

## Implementation Details

- Most permissions on Windows are ungated (always granted at the OS level).
- Key value: `openSettings()` opens the correct `ms-settings:privacy-*` pages so users can manage privacy settings directly from your app.

## Issues

Please file any issues or feature requests at the [issue tracker](https://github.com/jagadeeshchoudhary/permission_plus/issues).

## NOTE

not tested on windows device.
