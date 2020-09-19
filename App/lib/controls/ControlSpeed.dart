import 'package:flutter/material.dart';

import '../model.dart';
import 'AbstractControl.dart';

class ControlSpeed extends AbstractControl {
  const ControlSpeed({Key key, ValueChanged<String> onCommand}) : super(key: key, onCommand: onCommand);

  @override
  Widget buildControl(BuildContext context, HyperCube model) {
    return Column(
      children: [
        buildCardTitle(context, 'Speed', Icons.fast_forward),
        buildSlider(
          value: model.speed,
          min: 1,
          max: 10,
          divisions: 9,
          onChanged: (value) {
            model.setSpeed(value);
          },
          onChangeEnd: (value) {
            onCommand("SPEED ${value.toInt()}");
          },
        ),
      ],
    );
  }
}
