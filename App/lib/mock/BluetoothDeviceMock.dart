import 'package:flutter_blue/flutter_blue.dart';
import 'package:flutter_blue/gen/flutterblue.pb.dart' as protos;

import '../constants.dart';
import 'BluetoothServiceMock.dart';

class BluetoothDeviceMock extends BluetoothDevice {
  BluetoothDeviceMock.fromProto(protos.BluetoothDevice p) : super.fromProto(p);

  static BluetoothDeviceMock getInstance() {
    protos.BluetoothDevice p = new protos.BluetoothDevice();
    p.remoteId = MOCK_MAC;
    p.name = 'Mock';
    p.type = protos.BluetoothDevice_Type.LE;
    return BluetoothDeviceMock.fromProto(p);
  }

  @override
  Future<void> connect({Duration timeout, bool autoConnect = true}) {
    return new Future.value(null);
  }

  @override
  Future disconnect() {
    return new Future.value(null);
  }

  @override
  Future<List<BluetoothService>> discoverServices() {
    return new Future.value([BluetoothServiceMock.getInstance()]);
  }
}
