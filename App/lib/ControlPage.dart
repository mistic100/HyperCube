import 'dart:async';
import 'dart:convert';
import 'dart:ui' as ui;
import 'package:flutter/material.dart';

import './HM10Device.dart';

class ControlPage extends StatefulWidget {
  final HM10Device device;

  const ControlPage({Key key, this.device}) : super(key: key);

  @override
  _ControlPage createState() => new _ControlPage();
}

class _ControlPage extends State<ControlPage> {
  Timer _stateTimer;
  StreamSubscription _stateSubsription;

  bool _on = true;
  int _brightness = 100;
  String _mode = 'RAINBOW';
  String _hue = 'RAINBOW';

  var MODES = [
    {'name': 'RAINBOW', 'icon': Icons.done, 'selected': false},
    {'name': 'RAINDROP', 'icon': Icons.done, 'selected': false},
    {'name': 'PULSE', 'icon': Icons.done, 'selected': false},
    {'name': 'CYLON', 'icon': Icons.done, 'selected': false},
    {'name': 'CHASER', 'icon': Icons.done, 'selected': false},
    {'name': 'RANDOM', 'icon': Icons.done, 'selected': false},
  ];

  var HUES = [
    {'name': 'RAINBOW', 'color': Colors.black, 'selected': false},
    {'name': 'RED', 'color': Colors.red.shade800, 'selected': false},
    {'name': 'YELLOW', 'color': Colors.yellow, 'selected': false},
    {'name': 'GREEN', 'color': Colors.green, 'selected': false},
    {'name': 'BLUE', 'color': Colors.blue, 'selected': false},
    {'name': 'PINK', 'color': Colors.pink.shade400, 'selected': false},
    {'name': 'WHITE', 'color': Colors.white70, 'selected': false},
  ];

  @override
  void initState() {
    super.initState();

    _stateSubsription = widget.device.characteristic.value.listen((value) {
      _parseValue(utf8.decode(value));
    });
    widget.device.characteristic.setNotifyValue(true);

    _send("STATE");
    _stateTimer = new Timer.periodic(Duration(seconds: 10), (timer) {
      _send("STATE");
    });
  }

  @override
  void dispose() {
    _stateTimer.cancel();
    _stateSubsription.cancel();

    widget.device.device.disconnect();

    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text(widget.device.device.name),
      ),
      body: ListView(
        children: [
          Card(
            child: SwitchListTile(
              title: Text(
                'ON/OFF',
                style: Theme.of(context).textTheme.headline6,
              ),
              value: _on,
              activeColor: Theme.of(context).colorScheme.primary,
              onChanged: (value) {
                setState(() {
                  _on = value;
                });

                _send(_on ? "ON" : "OFF");
              },
            ),
          ),
          Card(
            child: Column(
              children: [
                ListTile(
                  leading: Icon(Icons.lightbulb_outline),
                  title: Text('Brightness'),
                ),
                Row(
                  children: [
                    Expanded(
                      child: Slider(
                        value: _brightness.toDouble(),
                        min: 0,
                        max: 255,
                        divisions: 8,
                        onChanged: (value) {
                          setState(() {
                            _brightness = value.toInt();
                          });
                        },
                        onChangeEnd: (value) {
                          _send("BRIGHT $_brightness");
                        },
                      ),
                    ),
                    Padding(
                      child: SizedBox(
                        child: Text('$_brightness'),
                        width: 30,
                      ),
                      padding: EdgeInsets.only(right: 15),
                    ),
                  ],
                ),
              ],
            ),
          ),
          Card(
            child: ListTile(
              leading: Icon(Icons.burst_mode),
              title: Text('Mode'),
              trailing: DropdownButton(
                items: MODES
                    .map((e) => DropdownMenuItem(
                          value: e['name'],
                          child: Text(e['name']),
                        ))
                    .toList(),
                value: _mode,
                onChanged: (value) {
                  setState(() {
                    _mode = value;
                  });

                  _send("MODE $_mode");
                },
              ),
            ),
          ),
          Card(
            child: Column(
              children: [
                ListTile(
                  leading: Icon(Icons.color_lens),
                  title: Text('Hue'),
                ),
                Padding(
                  child: ToggleButtons(
                    borderWidth: null,
                    children: HUES
                        .map((e) => e['name'] == 'RAINBOW'
                            ? ShaderMask(
                                blendMode: BlendMode.srcIn,
                                shaderCallback: (Rect bounds) => rainbow(),
                                child: Icon(
                                  Icons.lens,
                                  size: 40,
                                ),
                              )
                            : Icon(
                                Icons.lens,
                                color: e['color'],
                                size: 40,
                              ))
                        .toList(),
                    isSelected: HUES.map((e) => e['selected'] as bool).toList(),
                    onPressed: (index) {
                      setState(() {
                        _hue = HUES[index]['name'];
                        _refreshHues();
                      });

                      _send("HUE $_hue");
                    },
                  ),
                  padding: EdgeInsets.only(bottom: 15),
                ),
              ],
            ),
          ),
        ],
      ),
    );
  }

  /// Send data to the device
  void _send(String data) {
    try {
      widget.device.characteristic.write(utf8.encode("$data\n"));
    } catch (e) {
      print(e.toString());
    }
  }

  String _buffer = "";

  /// Parse data received from the device
  void _parseValue(String value) {
    _buffer += value;

    if (_buffer[_buffer.length - 1] == '\n') {
      print(_buffer);

      List<String> commands = _buffer.split('\n');
      _buffer = "";

      setState(() {
        commands
            .where((element) => element != null && element.isNotEmpty)
            .map((element) => element.split('='))
            .where((parts) => parts.length == 2)
            .forEach((parts) {
          switch (parts[0]) {
            case 'ON':
              _on = parts[1].trim() == '1';
              break;

            case 'BRIGHT':
              _brightness = int.parse(parts[1].trim());
              break;

            case 'MODE':
              _mode = parts[1].trim();
              break;

            case 'HUE':
              _hue = parts[1].trim();
              break;
          }
        });

        _refreshModes();
        _refreshHues();
      });
    }
  }

  /// Creates a rainbow gradient
  ui.Gradient rainbow() {
    return ui.Gradient.linear(Offset(8, 8), Offset(32, 32), [
      Colors.red,
      Colors.orange,
      Colors.yellow,
      Colors.green,
      Colors.blue,
      Colors.purple,
      Colors.pink,
    ], [
      0,
      0.166,
      0.333,
      0.5,
      0.666,
      0.833,
      1.0
    ]);
  }

  void _refreshModes() {
    MODES.forEach((element) {
      element['selected'] = element['name'] == _mode;
    });
  }

  void _refreshHues() {
    HUES.forEach((element) {
      element['selected'] = element['name'] == _hue;
    });
  }
}
