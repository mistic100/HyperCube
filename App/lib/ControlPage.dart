import 'dart:async';
import 'dart:convert';
import 'package:flutter/foundation.dart';
import 'package:flutter/material.dart';
import 'package:flutter_blue/flutter_blue.dart';
import 'package:hyper_cube/utils.dart';
import 'package:provider/provider.dart';

import 'controls/ControlColor.dart';
import 'controls/ControlMode.dart';
import 'controls/ControlPattern.dart';
import 'controls/ControlSpeed.dart';
import 'controls/ControlBrightness.dart';
import 'controls/ControlConsole.dart';
import 'HM10Device.dart';
import 'model.dart';

class ControlPage extends StatefulWidget {
  final HM10Device device;

  const ControlPage({Key key, this.device}) : super(key: key);

  @override
  _ControlPage createState() => new _ControlPage();
}

class _ControlPage extends State<ControlPage> {
  StreamSubscription _stateSubscription;
  StreamSubscription _valueSubscription;
  String _buffer = "";
  String _command = "";

  @override
  void dispose() {
    _valueSubscription.cancel();
    _stateSubscription.cancel();

    widget.device.device.disconnect();

    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return MultiProvider(
      providers: [
        ChangeNotifierProvider(create: (_) => HyperCube()),
        ChangeNotifierProvider(create: (_) => Logs()),
        ChangeNotifierProxyProvider<HyperCube, CustomPattern>(
          create: (context) => CustomPattern(),
          update: (context, model, pattern) => pattern.updateIfNotModified(model.pattern),
        ),
      ],
      builder: (context, _) {
        var onCommand = (value) {
          _pushCommand(context.read<Logs>(), value);
        };

        _startSubscription(
            Provider.of<HyperCube>(context, listen: false), //
            Provider.of<Logs>(context, listen: false));

        return DefaultTabController(
          length: 2,
          child: Scaffold(
            appBar: AppBar(
              title: Text(widget.device.device.name),
              actions: [
                Switch(
                  value: context.watch<HyperCube>().isOn,
                  activeColor: Theme.of(context).colorScheme.primary,
                  onChanged: (value) {
                    context.read<HyperCube>().setIsOn(value);

                    onCommand(value ? "ON" : "OFF");
                  },
                )
              ],
              bottom: TabBar(
                tabs: [
                  const Tab(text: 'Configuration'),
                  const Tab(text: 'Console'),
                ],
              ),
            ),
            body: TabBarView(
              children: [
                ListView(
                  //shrinkWrap: true,
                  children: [
                    ControlBrightness(onCommand: onCommand),
                    ControlMode(onCommand: onCommand),
                    Visibility(
                      visible: context.watch<HyperCube>().mode != 'PATTERN',
                      child: ControlColor(onCommand: onCommand),
                    ),
                    Visibility(
                      visible: context.watch<HyperCube>().mode == 'PATTERN',
                      child: ControlPattern(onCommand: onCommand),
                    ),
                    ControlSpeed(onCommand: onCommand),
                  ],
                ),
                ControlConsole(onCommand: onCommand),
              ],
            ),
          ),
        );
      },
    );
  }

  /// listen to device data
  void _startSubscription(HyperCube model, Logs logs) {
    if (_valueSubscription == null) {
      _stateSubscription = widget.device.device.state.listen((event) {
        if (event != BluetoothDeviceState.connected) {
          Navigator.of(context).pop();
        }
      });

      _valueSubscription = widget.device.characteristic.value.listen((value) {
        _parseValue(model, logs, utf8.decode(value));
      });
      widget.device.characteristic.setNotifyValue(true);

      _pushCommand(logs, "STATE");
    }
  }

  /// Try to open command transaction
  void _pushCommand(Logs logs, String data) async {
    if (data.isEmpty) {
      return;
    }

    print("$data");
    logs.appendLog("> $data\n");

    await _send("###");
    _command = "$data\n";
  }

  /// Send data to the device
  Future<void> _send(String command) async {
    if (command.isEmpty) {
      return;
    }

    try {
      await Future.forEach(splitByLength(command, 20), (part) async {
        await widget.device.characteristic.write(utf8.encode(part));
      });
    } catch (e) {
      print(e.toString());
      widget.device.characteristic.write(utf8.encode("\n"));
    }
  }

  /// Parse data received from the device
  void _parseValue(HyperCube model, Logs logs, String value) {
    if (value.isEmpty) {
      return;
    }

    _buffer += value;

    if (_buffer[_buffer.length - 1] == '\n') {
      print(_buffer);

      if (_buffer.startsWith('OK')) {
        _send(_command);
        _command = "";
      } else {
        logs.appendLog(_buffer);

        _buffer
            .split('\n')
            .where((element) => element != null && element.isNotEmpty)
            .map((element) => element.trim().split(' '))
            .forEach((parts) {
          switch (parts[0]) {
            case 'ON':
              model.setIsOn(parts[1] == '1');
              break;

            case 'BRIGHT':
              model.setBrightness(double.parse(parts[1]));
              break;

            case 'MODE':
              model.setMode(parts[1]);
              break;

            case 'COLOR':
              model.setColor(parts[1]);
              break;

            case 'SPEED':
              model.setSpeed(double.parse(parts[1]));
              break;

            case 'PATTERN':
              model.setPattern(CustomPattern.fromString(parts.sublist(1)));
              break;
          }
        });
      }

      _buffer = "";
    }
  }
}
