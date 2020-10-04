import 'package:flutter/material.dart';

import '../model.dart';
import 'AbstractControl.dart';

class ControlBrightness extends AbstractControl {
  const ControlBrightness({Key key, ValueChanged<String> onCommand}) : super(key: key, onCommand: onCommand);

  @override
  Widget buildControl(BuildContext context, HyperCube model) {
    return Column(
      children: [
        buildCardTitle(context, 'Brightness', Icons.lightbulb_outline),
        buildSlider(
          value: model.brightness,
          min: 0,
          max: 255,
          onChanged: (value) {
            model.setBrightness(value);
          },
          onChangeEnd: (value) {
            onCommand("BRIGHT ${value.toInt()}");
          },
        ),
      ],
    );
  }
}
