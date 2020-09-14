import 'dart:async';
import 'dart:convert';
import 'dart:ui' as ui;
import 'package:flutter/foundation.dart';
import 'package:flutter/material.dart';

import './HM10Device.dart';

const double PADDING = 16;

class ControlPage extends StatefulWidget {
  final HM10Device device;

  const ControlPage({Key key, this.device}) : super(key: key);

  @override
  _ControlPage createState() => new _ControlPage();
}

class _ControlPage extends State<ControlPage> {
  Timer _stateTimer;
  StreamSubscription _stateSubsription;
  ScrollController _scrollController = ScrollController();
  String _buffer = "";
  List<String> _logs = [];

  bool _on = true;
  double _brightness = 127;
  double _speed = 5;
  String _mode = 'RAINBOW';
  String _hue = 'RAINBOW';

  var MODES = [
    {'name': 'RAINBOW', 'selected': false},
    {'name': 'RAINDROP', 'selected': false},
    {'name': 'PULSE', 'selected': false},
    {'name': 'CYLON', 'selected': false},
    {'name': 'CHASER', 'selected': false},
    {'name': 'NOISE', 'selected': false},
    {'name': 'GRADIENT', 'selected': false},
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
                'Power',
                style: Theme.of(context).textTheme.headline6,
              ),
              secondary: Icon(Icons.power_settings_new),
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
                _buildCardTitle('Brightness', Icons.lightbulb_outline),
                Row(
                  children: [
                    Expanded(
                      child: Slider(
                        value: _brightness,
                        min: 0,
                        max: 255,
                        divisions: 8,
                        onChanged: (value) {
                          setState(() {
                            _brightness = value;
                          });
                        },
                        onChangeEnd: (value) {
                          _send("BRIGHT ${_brightness.toInt()}");
                        },
                      ),
                    ),
                    Padding(
                      child: SizedBox(
                        child: Text('${_brightness.toInt()}'),
                        width: 30,
                      ),
                      padding: EdgeInsets.only(right: PADDING),
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
                _buildCardTitle('Color', Icons.color_lens),
                Padding(
                  child: ToggleButtons(
                    renderBorder: false,
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
                  padding: EdgeInsets.symmetric(vertical: PADDING),
                ),
              ],
            ),
          ),
          Card(
            child: Column(
              children: [
                _buildCardTitle('Speed', Icons.fast_forward),
                Row(
                  children: [
                    Expanded(
                      child: Slider(
                        value: _speed,
                        min: 1,
                        max: 10,
                        divisions: 9,
                        onChanged: (value) {
                          setState(() {
                            _speed = value;
                          });
                        },
                        onChangeEnd: (value) {
                          _send("SPEED ${_speed.toInt()}");
                        },
                      ),
                    ),
                    Padding(
                      child: SizedBox(
                        child: Text('${_speed.toInt()}'),
                        width: 30,
                      ),
                      padding: EdgeInsets.only(right: PADDING),
                    ),
                  ],
                ),
              ],
            ),
          ),
          Card(
            child: Column(
              children: [
                ExpansionTile(
                  title: Text('Console'),
                  leading: Icon(Icons.keyboard_hide),
                  initiallyExpanded: kDebugMode,
                  children: [
                    Padding(
                      child: SizedBox(
                        child: SingleChildScrollView(
                          controller: _scrollController,
                          reverse: true,
                          child: Text(
                            _logs.join('\n'),
                            textAlign: TextAlign.left,
                            style:
                                Theme.of(context).textTheme.bodyText1.copyWith(
                                      fontFamily: 'monospace',
                                      color: Colors.grey,
                                    ),
                          ),
                        ),
                        width: double.infinity,
                        height: 150,
                      ),
                      padding: EdgeInsets.only(
                          left: PADDING, right: PADDING, bottom: PADDING),
                    ),
                  ],
                ),
              ],
            ),
          ),
        ],
      ),
    );
  }

  /// Component similar to ListTile but without bottom padding
  Widget _buildCardTitle(String title, IconData icon) {
    return Row(
      children: [
        Padding(
          child: Icon(icon),
          padding: EdgeInsets.only(top: PADDING, left: PADDING, right: PADDING),
        ),
        Padding(
          child: Text(
            title,
            style: Theme.of(context).textTheme.subtitle1,
          ),
          padding: EdgeInsets.only(top: PADDING, left: PADDING, right: PADDING),
        ),
      ],
    );
  }

  /// Send data to the device
  void _send(String data) {
    _log("$data\n");

    try {
      widget.device.characteristic.write(utf8.encode("$data\n"));
    } catch (e) {
      print(e.toString());
    }
  }

  /// Parse data received from the device
  void _parseValue(String value) {
    _log(value);

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
              _brightness = double.parse(parts[1].trim());
              break;

            case 'MODE':
              _mode = parts[1].trim();
              break;

            case 'HUE':
              _hue = parts[1].trim();
              break;

            case 'SPEED':
              _speed = double.parse(parts[1].trim());
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

  /// Refresh selected mode
  void _refreshModes() {
    MODES.forEach((element) {
      element['selected'] = element['name'] == _mode;
    });
  }

  /// Refresh selected hue
  void _refreshHues() {
    HUES.forEach((element) {
      element['selected'] = element['name'] == _hue;
    });
  }

  /// append log
  void _log(String value) {
    value.split('\n').where((v) => v.isNotEmpty).forEach((v) => _logs.add(v));

    while (_logs.length > 50) {
      _logs.removeAt(0);
    }

    if (_scrollController.positions.isNotEmpty) {
      _scrollController.jumpTo(0);
    }
  }
}
