Iterable<String> splitByLength(String str, int length) {
  List<StringBuffer> result = List();

  str.split('').forEach((char) {
    if (result.isEmpty || result.last.length == length) {
      result.add(StringBuffer());
    }
    result.last.write(char);
  });

  return result.map((e) => e.toString());
}
