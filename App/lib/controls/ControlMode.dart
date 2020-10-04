import 'package:flutter/material.dart';

import '../utils.dart';
import '../model.dart';
import 'AbstractControl.dart';

const MODES = [
  {'name': 'RAINBOW'},
  {'name': 'RAINDROP'},
  {'name': 'PULSE'},
  {'name': 'CYLON'},
  {'name': 'CHASER'},
  {'name': 'NOISE'},
  {'name': 'GRADIENT'},
  {'name': 'PATTERN'},
];

class ControlMode extends AbstractControl {
  const ControlMode({Key key, ValueChanged<String> onCommand}) : super(key: key, onCommand: onCommand);

  @override
  Widget buildControl(BuildContext context, HyperCube model) {
    return ListTile(
      leading: Icon(Icons.burst_mode),
      title: const Text('Mode'),
      trailing: DropdownButton(
        items: MODES
            .map((e) => DropdownMenuItem(
                  value: e['name'],
                  child: Text(e['name'].capitalize()),
                ))
            .toList(),
        value: model.mode,
        onChanged: (value) {
          model.setMode(value);

          onCommand("MODE ${value}");
        },
      ),
    );
  }
}
