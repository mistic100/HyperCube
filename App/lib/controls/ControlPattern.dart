import 'package:flutter/material.dart';
import 'package:flutter_colorpicker/flutter_colorpicker.dart';
import 'package:provider/provider.dart';
import 'package:shared_preferences/shared_preferences.dart';

import '../constants.dart';
import '../model.dart';
import 'AbstractControl.dart';

const ANIMATIONS = [
  {'name': '0', 'label': 'None'},
  {'name': 'F', 'label': 'Forward'},
  {'name': 'B', 'label': 'Backward'},
];

class ControlPattern extends AbstractControl {
  const ControlPattern({Key key, ValueChanged<String> onCommand}) : super(key: key, onCommand: onCommand);

  @override
  Widget buildControl(BuildContext context, HyperCube model) {
    return Consumer<CustomPattern>(
      builder: (context, pattern, _) => Column(
        children: [
          ListTile(
            leading: const Icon(Icons.color_lens),
            title: const Text('Pattern'),
            subtitle: const Text('Add up to 16 color stops'),
            trailing: IconButton(
              icon: const Icon(Icons.delete_forever),
              onPressed: pattern.stops.length == 0
                  ? null
                  : () async {
                      if (await _confirmResetPattern(context)) {
                        pattern.reset();
                      }
                    },
            ),
          ),
          Padding(
            padding: const EdgeInsets.symmetric(horizontal: PADDING),
            child: Row(
              children: [
                Expanded(
                  child: Scrollbar(
                    child: Container(
                      height: 50,
                      child: ListView(
                        scrollDirection: Axis.horizontal,
                        children: mapIndexed(
                          pattern.stops,
                          (i, stop) => _buildStopTile(context, stop, i),
                        ).toList(),
                      ),
                    ),
                  ),
                ),
                Padding(
                  padding: const EdgeInsets.only(left: 10),
                  child: ButtonTheme(
                    minWidth: 40,
                    height: 40,
                    child: RaisedButton(
                      child: const Icon(Icons.add_circle),
                      padding: EdgeInsets.zero,
                      onPressed: pattern.stops.length == 16
                          ? null
                          : () async {
                              Color color = await _dialogSelectColor(context, Colors.white, true);
                              if (color != null) {
                                pattern.addStop(color);
                              }
                            },
                    ),
                  ),
                ),
              ],
            ),
          ),
          ListTile(
            title: const Text('Animation'),
            leading: const Icon(Icons.flip_to_back),
            trailing: DropdownButton(
              items: ANIMATIONS
                  .map((e) => DropdownMenuItem(
                        value: e['name'],
                        child: Text(e['label']),
                      ))
                  .toList(),
              value: pattern.animation,
              onChanged: (value) {
                pattern.setAnimation(value);
              },
            ),
          ),
          buildCardTitle(context, 'Zoom', Icons.aspect_ratio),
          buildSlider(
            value: pattern.zoom,
            min: 1,
            max: 10,
            divisions: 9,
            onChanged: (value) {
              pattern.setZoom(value);
            },
          ),
          Divider(),
          ButtonBar(
            alignment: MainAxisAlignment.spaceBetween,
            children: [
              Row(
                children: [
                  FlatButton(
                    child: const Text('Save'),
                    onPressed: pattern.stops.length == 0
                        ? null
                        : () async {
                            _dialogSavePattern(context, pattern);
                          },
                  ),
                  FlatButton(
                    child: const Text('Load'),
                    onPressed: () async {
                      _dialogLoadPattern(context, pattern);
                    },
                  ),
                ],
              ),
              RaisedButton(
                child: const Text('Apply pattern'),
                onPressed: pattern.stops.length == 0
                    ? null
                    : () {
                        pattern.setModified(false);
                        onCommand("PATTERN " + pattern.toString());
                      },
              ),
            ],
          ),
        ],
      ),
    );
  }

  /// iterate of a list like a normal programming language !!
  Iterable<E> mapIndexed<E, T>(Iterable<T> items, E Function(int index, T item) f) sync* {
    var index = 0;

    for (final item in items) {
      yield f(index, item);
      index = index + 1;
    }
  }

  /// build a color stop
  Widget _buildStopTile(BuildContext context, Color _stop, int _index) {
    return Padding(
      padding: const EdgeInsets.only(right: 5, bottom: 5, top: 5),
      child: GestureDetector(
        onTap: () async {
          Color color = await _dialogSelectColor(context, _stop, false);
          if (color == Colors.transparent) {
            context.read<CustomPattern>().removeStop(_index);
          } else if (color != null) {
            context.read<CustomPattern>().setStop(_index, color);
          }
        },
        child: Container(
          width: 40,
          height: 40,
          child: Center(
            child: Text(
              '${_index + 1}',
              style: Theme.of(context).textTheme.bodyText1.copyWith(
                    color: _stop.computeLuminance() > 0.5 ? Colors.black : Colors.white,
                  ),
            ),
          ),
          decoration: BoxDecoration(
            color: _stop,
            border: Border.all(color: Colors.white, width: 1.0),
            shape: BoxShape.circle,
          ),
        ),
      ),
    );
  }

  /// open dialog to choose a color
  Future<Color> _dialogSelectColor(BuildContext context, Color _color, bool isNew) async {
    return showDialog(
      context: context,
      child: AlertDialog(
        contentPadding: EdgeInsets.zero,
        content: SingleChildScrollView(
          child: ColorPicker(
            pickerColor: _color,
            showLabel: false,
            enableAlpha: false,
            onColorChanged: (color) {
              _color = color;
            },
          ),
        ),
        actions: [
          FlatButton(
            child: isNew ? const Text('Cancel') : const Text('Delete stop'),
            onPressed: () {
              Navigator.of(context).pop(isNew ? null : Colors.transparent);
            },
          ),
          FlatButton(
            child: isNew ? const Text('Add') : const Text('Apply'),
            color: Theme.of(context).colorScheme.primary,
            onPressed: () {
              Navigator.of(context).pop(_color);
            },
          ),
        ],
      ),
    );
  }

  /// open dialog to reset the pattern
  Future<bool> _confirmResetPattern(BuildContext context) async {
    return showDialog(
      context: context,
      child: AlertDialog(
        title: const Text('Delete all color stops?'),
        actions: [
          FlatButton(
            child: const Text('Cancel'),
            onPressed: () {
              Navigator.of(context).pop(false);
            },
          ),
          FlatButton(
            child: const Text('Ok'),
            color: Theme.of(context).colorScheme.primary,
            onPressed: () {
              Navigator.of(context).pop(true);
            },
          ),
        ],
      ),
    );
  }

  /// open dialog to save the pattern
  Future<void> _dialogSavePattern(BuildContext context, CustomPattern pattern) async {
    TextEditingController inputController = TextEditingController();

    return showDialog(
      context: context,
      child: AlertDialog(
        title: const Text('Save pattern'),
        content: TextField(
          controller: inputController,
          autofocus: true,
          decoration: const InputDecoration(labelText: 'Pattern name'),
        ),
        actions: [
          FlatButton(
            child: const Text('Cancel'),
            onPressed: () {
              Navigator.of(context).pop();
            },
          ),
          FlatButton(
            child: const Text('Save'),
            color: Theme.of(context).colorScheme.primary,
            onPressed: () async {
              if (inputController.text.isNotEmpty) {
                await StorePatternsHelper.addPattern(inputController.text, pattern.toString());
                Navigator.of(context).pop();
              }
            },
          ),
        ],
      ),
    );
  }

  /// open dialog to load a pattern
  Future<void> _dialogLoadPattern(BuildContext context, CustomPattern pattern) async {
    List<String> patternsNames = await StorePatternsHelper.listNames();

    if (patternsNames.isEmpty) {
      return showDialog(
        context: context,
        child: AlertDialog(
          title: const Text('No saved pattern available'),
          actions: [
            FlatButton(
              child: const Text('Close'),
              onPressed: () {
                Navigator.of(context).pop();
              },
            ),
          ],
        ),
      );
    }

    return showDialog(
      context: context,
      child: AlertDialog(
        title: const Text('Load pattern'),
        contentPadding: const EdgeInsets.only(top: PADDING),
        content: SingleChildScrollView(
          child: StatefulBuilder(
            builder: (context, setState) => Column(
              children: mapIndexed(
                  patternsNames,
                  (i, name) => Material(
                        child: InkWell(
                          child: ListTile(
                            title: Text(name),
                            trailing: IconButton(
                              icon: Icon(Icons.delete),
                              onPressed: () async {
                                if (await _confirmDeletePattern(context, name)) {
                                  await StorePatternsHelper.deletePattern(name);
                                  patternsNames = await StorePatternsHelper.listNames();
                                  setState(() {});
                                }
                              },
                            ),
                          ),
                          onTap: () async {
                            CustomPattern newPattern = await StorePatternsHelper.getPattern(name);
                            pattern.update(newPattern);
                            Navigator.of(context).pop();
                          },
                        ),
                      )).toList(),
            ),
          ),
        ),
        actions: [
          FlatButton(
            child: const Text('Cancel'),
            onPressed: () {
              Navigator.of(context).pop();
            },
          ),
        ],
      ),
    );
  }

  /// open dialog to delete a pattern
  Future<bool> _confirmDeletePattern(BuildContext context, String name) {
    return showDialog(
      context: context,
      child: AlertDialog(
        title: const Text('Delete pattern?'),
        content: Text(name),
        actions: [
          FlatButton(
            child: const Text('Cancel'),
            onPressed: () {
              Navigator.of(context).pop(false);
            },
          ),
          FlatButton(
            child: const Text('Delete'),
            color: Theme.of(context).colorScheme.primary,
            onPressed: () {
              Navigator.of(context).pop(true);
            },
          ),
        ],
      ),
    );
  }
}

class StorePatternsHelper {
  /// list all stored patterns
  static Future<List<String>> list() async {
    SharedPreferences prefs = await SharedPreferences.getInstance();
    List<String> patterns = prefs.getStringList(SETTING_PATTERNS);

    if (patterns == null) {
      patterns = List();
    }

    return patterns;
  }

  /// list all stored patterns names
  static Future<List<String>> listNames() async {
    List<String> patterns = await list();

    List<String> names = patterns.map((e) => e.split(SETTING_PATTERN_SPERATOR)[0]).toList();
    names.sort((a, b) => a.compareTo(b));

    return names;
  }

  /// returns a pattern by name
  static Future<CustomPattern> getPattern(String name) async {
    List<String> patterns = await list();

    String pattern =
        patterns.firstWhere((element) => element.startsWith('$name$SETTING_PATTERN_SPERATOR'), orElse: () => null);

    if (pattern != null) {
      pattern = pattern.split(SETTING_PATTERN_SPERATOR)[1];
      return CustomPattern.fromString(pattern.split(' '));
    } else {
      return null;
    }
  }

  /// deletes a pattern by name
  static Future<void> deletePattern(String name) async {
    List<String> patterns = await list();

    patterns = patterns.where((element) => !element.startsWith('$name$SETTING_PATTERN_SPERATOR')).toList();

    SharedPreferences prefs = await SharedPreferences.getInstance();
    await prefs.setStringList(SETTING_PATTERNS, patterns);
  }

  /// adds/replace a pattern
  static Future<void> addPattern(String name, String pattern) async {
    List<String> patterns = await list();

    int i = patterns.indexWhere((element) => element.startsWith('$name$SETTING_PATTERN_SPERATOR'));
    if (i == -1) {
      patterns.add('$name$SETTING_PATTERN_SPERATOR$pattern');
    } else {
      patterns[i] = '$name$SETTING_PATTERN_SPERATOR$pattern';
    }

    SharedPreferences prefs = await SharedPreferences.getInstance();
    await prefs.setStringList(SETTING_PATTERNS, patterns);
  }
}
