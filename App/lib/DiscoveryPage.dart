import 'dart:async';
import 'package:flutter/material.dart';
import 'package:flutter_blue/flutter_blue.dart';

import './HM10Device.dart';
import 'constants.dart';

class DiscoveryPage extends StatefulWidget {
  final String lastDevice;

  const DiscoveryPage({Key key, this.lastDevice = ''}) : super(key: key);

  @override
  _DiscoveryPage createState() => new _DiscoveryPage();
}

class _DiscoveryPage extends State<DiscoveryPage> {
  List<BluetoothDevice> devices = List<BluetoothDevice>();
  StreamSubscription _devicesSubscription;
  bool _isDiscovering = false;
  bool _tryLastDevice = false;

  @override
  void initState() {
    super.initState();

    _tryLastDevice = widget.lastDevice.isNotEmpty;

    _startDiscovery();
  }

  @override
  void dispose() {
    _stopDiscovery();

    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title:
            _isDiscovering ? Text('Searching devices') : Text('Found devices'),
        actions: [
          _isDiscovering
              ? Row(
                  children: [
                    SizedBox(
                      child: CircularProgressIndicator(
                        strokeWidth: 3,
                      ),
                      width: 24,
                      height: 24,
                    ),
                    IconButton(
                      icon: Icon(Icons.stop),
                      onPressed: _stopDiscovery,
                    ),
                  ],
                )
              : IconButton(
                  icon: Icon(Icons.search),
                  onPressed: _startDiscovery,
                ),
        ],
      ),
      body: ListView.builder(
        itemCount: devices.length,
        itemBuilder: (BuildContext context, index) {
          BluetoothDevice device = devices[index];
          return ListTile(
            leading: Icon(
              Icons.bluetooth_connected,
              color: Theme.of(context).colorScheme.primary,
            ),
            title: Text(device.name),
            subtitle: Text(device.id.toString()),
            onTap: () async {
              _connectDeviceAndPop(device);
            },
          );
        },
      ),
    );
  }

  /// Stops devices discovery
  Future<void> _stopDiscovery() async {
    if (mounted) {
      setState(() {
        _isDiscovering = false;
      });
    }

    _devicesSubscription?.cancel();
    await FlutterBlue.instance.stopScan();
  }

  /// Starts devices discovery
  Future<void> _startDiscovery() async {
    setState(() {
      devices.clear();
      _isDiscovering = true;
    });

    FlutterBlue.instance.connectedDevices.then((devices) {
      setState(() {
        devices.forEach((device) {
          if (!this.devices.contains(device)) {
            this.devices.add(device);
          }
        });
      });

      _autoConnect();
    });

    _devicesSubscription = FlutterBlue.instance.scanResults.listen((results) {
      setState(() {
        results.forEach((result) {
          if (!this.devices.contains(result.device)) {
            this.devices.add(result.device);
          }
        });
      });

      _autoConnect();
    });

    await FlutterBlue.instance.startScan(timeout: Duration(seconds: 30));

    if (mounted) {
      _autoConnect();

      setState(() {
        _isDiscovering = false;
      });
    }
  }

  /// Tries to connect to a device and pop the context
  void _connectDeviceAndPop(BluetoothDevice device) async {
    HM10Device _device = await _connectDevice(device);

    if (device == null) {
      _showAlert();
    } else {
      Navigator.of(context).pop(_device);
    }
  }

  /// Tries to connect to a device
  Future<HM10Device> _connectDevice(BluetoothDevice device) async {
    try {
      await device.connect(timeout: Duration(seconds: 10));
    } catch (e) {
      if (e.code != 'already_connected') {
        print('Cannot connect: ' + e.code);
        return new Future.value(null);
      }
      print('Already connected');
    }

    List<BluetoothService> services = await device.discoverServices();

    BluetoothService service = services.firstWhere(
        (service) =>
            service.uuid.toString().toUpperCase() == HM_10_SERVICE_UUID,
        orElse: () => null);

    if (service == null) {
      print('Service not found');
      return new Future.value(null);
    }

    List<BluetoothCharacteristic> characteristics = service.characteristics;

    BluetoothCharacteristic characteristic = characteristics.firstWhere(
        (characteristic) =>
            characteristic.uuid.toString().toUpperCase() ==
            HM_10_CHARACTERISTIC_UUID,
        orElse: () => null);

    if (characteristic == null) {
      print('Characteristic not found');
      return new Future.value(null);
    }

    print('Successfully connected to ' + device.name);

    return new Future.value(new HM10Device(device, service, characteristic));
  }

  /// Displays connection error
  void _showAlert() async {
    return showDialog<void>(
      context: context,
      builder: (BuildContext context) {
        return AlertDialog(
          title: Text('Unable to connect'),
          content:
              Text('The device does not respond or is not a HM-10 module.'),
          actions: [
            FlatButton(
              child: Text('Close'),
              onPressed: () {
                Navigator.of(context).pop();
              },
            ),
          ],
        );
      },
    );
  }

  /// Tries to autoconnect to the last device if present
  void _autoConnect() {
    if (_tryLastDevice) {
      BluetoothDevice device = devices.firstWhere(
          (device) => device.id.toString() == widget.lastDevice,
          orElse: () => null);

      if (device != null) {
        _tryLastDevice = false;
        _connectDeviceAndPop(device);
      }
    }
  }
}