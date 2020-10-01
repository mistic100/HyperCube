import 'dart:ui';
import 'package:flutter/foundation.dart';

class CustomPattern with ChangeNotifier {
  bool _modified = false;
  List<Color> _stops = List();
  bool _animate = true;
  double _zoom = 1;

  CustomPattern();

  CustomPattern.fromString(List<String> sublist) {
    sublist.map((e) => e.split('=')).forEach((parts) {
      switch (parts[0]) {
        case 'A':
          _animate = parts[1] == '1';
          break;

        case 'Z':
          _zoom = double.parse(parts[1]);
          break;

        case 'C':
          int r = -1, g = -1, b = -1;
          for (var item in parts[1].split('')) {
            if (r == -1) {
              r = int.parse('$item$item', radix: 16);
            } else if (g == -1) {
              g = int.parse('$item$item', radix: 16);
            } else {
              b = int.parse('$item$item', radix: 16);
              _stops.add(Color.fromARGB(255, r, g, b));
              r = g = b = -1;
            }
          }
          break;
      }
    });
  }

  bool get animate => _animate;
  double get zoom => _zoom;
  List<Color> get stops => _stops;

  CustomPattern updateIfNotModified(CustomPattern other) {
    if (!_modified) {
      _animate = other.animate;
      _zoom = other.zoom;
      _stops = List.from(other.stops);
      notifyListeners();
    }
    return this;
  }

  CustomPattern update(CustomPattern other) {
    _animate = other.animate;
    _zoom = other.zoom;
    _stops = List.from(other.stops);
    _modified = true;
    notifyListeners();
    return this;
  }

  void removeStop(int index) {
    _stops.removeAt(index);
    _modified = true;
    notifyListeners();
  }

  void addStop(Color stop) {
    _stops.add(stop);
    _modified = true;
    notifyListeners();
  }

  void setStop(int index, Color stop) {
    _stops[index] = stop;
    _modified = true;
    notifyListeners();
  }

  void reset() {
    _stops.clear();
    _animate = true;
    _zoom = 1;
    _modified = true;
    notifyListeners();
  }

  void setAnimate(bool animate) {
    _animate = animate;
    _modified = true;
    notifyListeners();
  }

  void setZoom(double zoom) {
    _zoom = zoom;
    _modified = true;
    notifyListeners();
  }

  void setModified(bool modified) {
    _modified = modified;
  }

  @override
  String toString() {
    String str = "A=${_animate ? '1' : '0'} Z=${_zoom.toInt()} C=";

    _stops.forEach((stop) {
      str += stop.red.toRadixString(16)[0];
      str += stop.green.toRadixString(16)[0];
      str += stop.blue.toRadixString(16)[0];
    });

    return str;
  }
}

class HyperCube with ChangeNotifier, DiagnosticableTreeMixin {
  bool _isOn = true;
  double _brightness = 127;
  double _speed = 8;
  String _mode = 'RAINBOW';
  String _color = 'RAINBOW';
  CustomPattern _pattern = CustomPattern();

  bool get isOn => _isOn;
  double get brightness => _brightness;
  double get speed => _speed;
  String get mode => _mode;
  String get color => _color;
  CustomPattern get pattern => _pattern;

  void setIsOn(bool isOn) {
    _isOn = isOn;
    notifyListeners();
  }

  void setBrightness(double brightness) {
    _brightness = brightness;
    notifyListeners();
  }

  void setSpeed(double speed) {
    _speed = speed;
    notifyListeners();
  }

  void setMode(String mode) {
    _mode = mode;
    notifyListeners();
  }

  void setColor(String color) {
    _color = color;
    notifyListeners();
  }

  void setPattern(CustomPattern pattern) {
    _pattern = pattern;
    notifyListeners();
  }

  @override
  void debugFillProperties(DiagnosticPropertiesBuilder properties) {
    super.debugFillProperties(properties);
    properties.add(FlagProperty("isOn", value: _isOn));
    properties.add(DoubleProperty("brightness", _brightness));
    properties.add(DoubleProperty("speed", _speed));
    properties.add(StringProperty("mode", _mode));
    properties.add(StringProperty("color", _color));
  }
}

class Logs with ChangeNotifier {
  List<String> _logs = [];

  String get log => _logs.join('\n');

  void appendLog(String value) {
    value.split('\n').where((v) => v.isNotEmpty).forEach((v) => _logs.add(v));

    while (_logs.length > 100) {
      _logs.removeAt(0);
    }

    notifyListeners();
  }
}
