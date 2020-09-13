import 'package:flutter_blue/flutter_blue.dart';

class HM10Device {
  BluetoothDevice device;
  BluetoothService service;
  BluetoothCharacteristic characteristic;

  HM10Device(this.device, this.service, this.characteristic);

  String get id {
    return device.id.toString();
  }
}
