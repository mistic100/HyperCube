import 'package:flutter_blue/flutter_blue.dart';
import 'package:flutter_blue/gen/flutterblue.pb.dart' as protos;
import 'package:rxdart/rxdart.dart';

import '../constants.dart';

class BluetoothCharacteristicMock extends BluetoothCharacteristic {
  BluetoothCharacteristicMock.fromProto(protos.BluetoothCharacteristic p)
      : super.fromProto(p);

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
    _value.add(value);
    return new Future.value(null);
  }

  @override
  Future<bool> setNotifyValue(bool notify) {
    return new Future.value(null);
  }
}
