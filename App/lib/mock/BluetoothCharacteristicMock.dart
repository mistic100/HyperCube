import 'dart:convert';

import 'package:flutter_blue/flutter_blue.dart';
import 'package:flutter_blue/gen/flutterblue.pb.dart' as protos;
import 'package:rxdart/rxdart.dart';

import '../constants.dart';

class BluetoothCharacteristicMock extends BluetoothCharacteristic {
  BluetoothCharacteristicMock.fromProto(protos.BluetoothCharacteristic p) : super.fromProto(p);

  static BluetoothCharacteristicMock getInstance() {
    protos.BluetoothCharacteristic p = new protos.BluetoothCharacteristic();
    p.uuid = HM_10_CHARACTERISTIC_UUID;
    p.remoteId = MOCK_MAC;
    p.serviceUuid = HM_10_SERVICE_UUID;
    return BluetoothCharacteristicMock.fromProto(p);
  }

  BehaviorSubject<List<int>> _value = BehaviorSubject.seeded([]);
  Stream<List<int>> get value => _value.stream;

  @override
  Future<Null> write(List<int> value, {bool withoutResponse = false}) {
    if (utf8.decode(value) == 'STATE\n') {
      _value.add(utf8.encode('ON 1\n'));
      _value.add(utf8.encode('BRIGHT 100\n'));
      _value.add(utf8.encode('SPEED 8\n'));
      _value.add(utf8.encode('MODE PATTERN\n'));
      _value.add(utf8.encode('COLOR RAINBOW\n'));
      _value.add(utf8.encode('PATTERN A=0 Z=1 C=f0000f\n'));
    } else {
      _value.add(value);
    }
    return new Future.value(null);
  }

  @override
  Future<bool> setNotifyValue(bool notify) {
    return new Future.value(null);
  }
}
