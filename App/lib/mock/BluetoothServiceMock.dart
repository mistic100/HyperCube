import 'package:flutter_blue/flutter_blue.dart';
import 'package:flutter_blue/gen/flutterblue.pb.dart' as protos;

import 'BluetoothCharacteristicMock.dart';
import '../constants.dart';

class BluetoothServiceMock extends BluetoothService {
  BluetoothServiceMock.fromProto(protos.BluetoothService p)
      : super.fromProto(p);

  static BluetoothServiceMock getInstance() {
    protos.BluetoothService p = new protos.BluetoothService();
    p.uuid = HM_10_SERVICE_UUID;
    p.remoteId = MOCK_MAC;
    p.isPrimary = false;
    return BluetoothServiceMock.fromProto(p);
  }

  List<BluetoothCharacteristic> get characteristics {
    return [BluetoothCharacteristicMock.getInstance()];
  }
}
