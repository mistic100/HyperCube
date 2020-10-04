import 'dart:ui';
import 'package:flutter/foundation.dart';

class CustomPattern with ChangeNotifier {
  bool _modified = false;
  List<Color> _stops = List();
  String _animation = 'F';
  double _zoom = 1;

  CustomPattern();

  CustomPattern.fromString(List<String> sublist) {
    sublist.map((e) => e.split('=')).forEach((parts) {
      switch (parts[0]) {
        case 'A':
          _animation = parts[1] == '1' ? 'F' : parts[1]; // retro-compat
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

  String get animation => _animation;
  double get zoom => _zoom;
  List<Color> get stops => _stops;

  CustomPattern updateIfNotModified(CustomPattern other) {
    if (!_modified) {
      _animation = other.animation;
      _zoom = other.zoom;
      _stops = List.from(other.stops);
      notifyListeners();
    }
    return this;
  }

  CustomPattern update(CustomPattern other) {
    _animation = other.animation;
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
    _animation = 'F';
    _zoom = 1;
    _modified = true;
    notifyListeners();
  }

  void setAnimation(String animation) {
    _animation = animation;
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
    String str = "A=${_animation} Z=${_zoom.toInt()} C=";

    _stops.forEach((stop) {
      str += stop.red.toRadixString(16)[0];
      str += stop.green.toRadixString(16)[0];
      str += stop.blue.toRadixString(16)[0];
    });

    return str;
  }
}

class HyperCube with ChangeNotifier {
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
