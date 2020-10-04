import 'package:flutter/material.dart';
import 'package:flutter_blue/flutter_blue.dart';
import 'package:hyper_cube/constants.dart';
import 'package:system_setting/system_setting.dart';
import 'package:shared_preferences/shared_preferences.dart';

import 'DiscoveryPage.dart';
import 'ControlPage.dart';
import 'HM10Device.dart';

class HomePage extends StatefulWidget {
  @override
  _HomePage createState() => _HomePage();
}

class _HomePage extends State<HomePage> {
  BluetoothState _bluetoothState = BluetoothState.unknown;
  bool _isLoading = true;

  @override
  void initState() {
    super.initState();

    FlutterBlue.instance.state.listen((state) {
      setState(() {
        _bluetoothState = state;
        _isLoading = false;

        if (state == BluetoothState.on) {
          _autoConnect();
        }
      });
    });
  }

  @override
  void dispose() {
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('HyperCube'),
      ),
      body: _isLoading
          ? Center(child: CircularProgressIndicator())
          : _bluetoothState != BluetoothState.on ? _buildEnableBluetooth() : _buildSelectDevice(),
    );
  }

  /// Displays BT connection page
  Widget _buildEnableBluetooth() {
    return Center(
      child: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        // crossAxisAlignment: CrossAxisAlignment.center,
        children: [
          Ink(
            decoration: ShapeDecoration(
              color: Theme.of(context).colorScheme.primary,
              shape: CircleBorder(),
            ),
            child: IconButton(
              icon: const Icon(Icons.bluetooth),
              color: Theme.of(context).colorScheme.onPrimary,
              iconSize: 80,
              onPressed: () {
                SystemSetting.goto(SettingTarget.BLUETOOTH);
              },
            ),
          ),
          Divider(),
          Text(
            'Please enable Bluetooth',
            textAlign: TextAlign.center,
            style: Theme.of(context).textTheme.headline6,
          ),
        ],
      ),
    );
  }

  /// Displays select device page
  Widget _buildSelectDevice() {
    return Center(
      child: RaisedButton(
        child: Text(
          'Select a device',
          style: Theme.of(context).textTheme.headline6.copyWith(
                color: Theme.of(context).colorScheme.onPrimary,
              ),
        ),
        padding: const EdgeInsets.all(20),
        color: Theme.of(context).colorScheme.primary,
        shape: RoundedRectangleBorder(
          borderRadius: BorderRadius.circular(40),
        ),
        onPressed: _selectDevice,
      ),
    );
  }

  /// Shows the device selection page then the control page
  void _selectDevice({String lastDevice = ''}) async {
    final HM10Device selectedDevice = await Navigator.of(context).push(
      MaterialPageRoute(
        builder: (context) {
          return DiscoveryPage(lastDevice: lastDevice);
        },
      ),
    );

    if (selectedDevice != null) {
      try {
        SharedPreferences prefs = await SharedPreferences.getInstance();
        await prefs.setString(SETTING_LAST_DEVICE, selectedDevice.id);
      } catch (e) {
        print('Cannot write preference');
      }

      Navigator.of(context).push(
        MaterialPageRoute(
          builder: (context) {
            return ControlPage(device: selectedDevice);
          },
        ),
      );
    } else {
      try {
        SharedPreferences prefs = await SharedPreferences.getInstance();
        await prefs.remove(SETTING_LAST_DEVICE);
      } catch (e) {
        print('Cannot write preference');
      }
    }
  }

  /// Tries to autoconnect to the last device if present
  void _autoConnect() async {
    SharedPreferences prefs = await SharedPreferences.getInstance();
    String lastDevice = prefs.getString(SETTING_LAST_DEVICE);

    if (lastDevice != null && lastDevice.isNotEmpty) {
      _selectDevice(lastDevice: lastDevice);
    }
  }
}
