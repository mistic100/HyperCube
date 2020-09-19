import 'package:flutter/material.dart';

import '../model.dart';
import 'AbstractControl.dart';

var COLORS = [
  {'name': 'RAINBOW', 'color': Colors.black},
  {'name': 'RED', 'color': Colors.red.shade700},
  {'name': 'YELLOW', 'color': Colors.yellow},
  {'name': 'GREEN', 'color': Colors.green},
  {'name': 'BLUE', 'color': Colors.blue},
  {'name': 'PINK', 'color': Colors.pink.shade400},
  {'name': 'WHITE', 'color': Colors.white70},
];

class ControlColor extends AbstractControl {
  const ControlColor({Key key, ValueChanged<String> onCommand}) : super(key: key, onCommand: onCommand);

  @override
  Widget buildControl(BuildContext context, HyperCube model) {
    return Column(
      children: [
        buildCardTitle(context, 'Color', Icons.color_lens),
        Padding(
          padding: const EdgeInsets.all(PADDING),
          child: Row(
            mainAxisAlignment: MainAxisAlignment.spaceBetween,
            children: COLORS
                .map((e) => GestureDetector(
                      child: Container(
                        width: 40,
                        height: 40,
                        decoration: BoxDecoration(
                          border: e['name'] == model.color ? Border.all(width: 2.0, color: Colors.white) : null,
                          color: e['color'],
                          shape: BoxShape.circle,
                          gradient: e['name'] == 'RAINBOW'
                              ? LinearGradient(colors: [
                                  Colors.red,
                                  Colors.orange,
                                  Colors.yellow,
                                  Colors.green,
                                  Colors.blue,
                                  Colors.purple,
                                  Colors.pink,
                                ])
                              : null,
                        ),
                      ),
                      onTap: () {
                        model.setColor(e['name']);

                        onCommand("COLOR ${e['name']}");
                      },
                    ))
                .toList(),
          ),
        ),
      ],
    );
  }
}
