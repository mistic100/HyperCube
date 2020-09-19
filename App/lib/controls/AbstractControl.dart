import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import '../model.dart';

const double PADDING = 16;

abstract class AbstractControl extends StatelessWidget {
  final ValueChanged<String> onCommand;

  const AbstractControl({Key key, this.onCommand}) : super(key: key);

  Widget buildControl(BuildContext context, HyperCube model);

  @override
  Widget build(BuildContext context) {
    return Consumer<HyperCube>(
      builder: (context, model, _) => Card(
        child: buildControl(context, model),
      ),
    );
  }

  /// build a component similar to ListTile without bottom padding
  @protected
  Widget buildCardTitle(BuildContext context, String title, IconData icon) {
    return Row(
      children: [
        Padding(
          child: Icon(icon),
          padding: const EdgeInsets.only(top: PADDING, left: PADDING, right: PADDING),
        ),
        Padding(
          child: Text(
            title,
            style: Theme.of(context).textTheme.subtitle1,
          ),
          padding: const EdgeInsets.only(top: PADDING, left: PADDING, right: PADDING),
        ),
      ],
    );
  }

  /// build a slider with display of the value
  @protected
  Widget buildSlider(
      {double value,
      double min,
      double max,
      int divisions,
      ValueChanged<double> onChanged,
      ValueChanged<double> onChangeEnd}) {
    return Row(
      children: [
        Expanded(
          child: Slider(
            value: value,
            min: min,
            max: max,
            divisions: divisions,
            onChanged: onChanged,
            onChangeEnd: onChangeEnd,
          ),
        ),
        Padding(
          child: SizedBox(
            child: Text('${value.toInt()}'),
            width: 30,
          ),
          padding: const EdgeInsets.only(right: PADDING),
        ),
      ],
    );
  }
}
