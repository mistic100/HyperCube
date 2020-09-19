import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:provider/provider.dart';

import '../model.dart';
import 'AbstractControl.dart';

class ControlConsole extends StatefulWidget {
  final ValueChanged<String> onCommand;

  const ControlConsole({Key key, this.onCommand}) : super(key: key);

  @override
  _ControlConsole createState() => _ControlConsole(onCommand);
}

class _ControlConsole extends State<ControlConsole> {
  final ValueChanged<String> onCommand;
  final ScrollController _scrollController = ScrollController();
  final TextEditingController _commandController = TextEditingController();

  Logs logs;

  _ControlConsole(this.onCommand);

  @override
  void initState() {
    super.initState();

    Future.delayed(Duration.zero, () {
      logs = Provider.of<Logs>(context, listen: false);
      logs.addListener(_onNewLogs);
    });
  }

  @override
  void dispose() {
    logs.removeListener(_onNewLogs);
    super.dispose();
  }

  void _onNewLogs() {
    if (_scrollController.hasClients) {
      _scrollController.animateTo(0, duration: Duration(milliseconds: 200), curve: Curves.ease);
    }
  }

  @override
  Widget build(BuildContext context) {
    return Card(
      child: Column(
        children: [
          Expanded(
            child: Padding(
              padding: const EdgeInsets.all(PADDING),
              child: Container(
                decoration: BoxDecoration(
                  color: Colors.grey.shade900,
                  borderRadius: BorderRadius.circular(4),
                ),
                child: SingleChildScrollView(
                  controller: _scrollController,
                  reverse: true,
                  child: Text(
                    context.watch<Logs>().log,
                    textAlign: TextAlign.left,
                    style: Theme.of(context).textTheme.bodyText1.copyWith(
                          fontFamily: 'monospace',
                          color: Colors.grey,
                        ),
                  ),
                ),
                width: double.infinity,
                height: 200,
              ),
            ),
          ),
          Padding(
            padding: const EdgeInsets.symmetric(horizontal: PADDING),
            child: Stack(
              alignment: Alignment.bottomRight,
              children: [
                TextField(
                  controller: _commandController,
                  autocorrect: false,
                  enableSuggestions: false,
                  decoration: const InputDecoration(labelText: 'Command'),
                  inputFormatters: [
                    TextInputFormatter.withFunction((oldValue, newValue) => TextEditingValue(
                          text: newValue.text?.toUpperCase(),
                          selection: newValue.selection,
                        ))
                  ],
                  onSubmitted: (value) {
                    onCommand(_commandController.text);
                  },
                ),
                IconButton(
                  padding: const EdgeInsets.all(0),
                  iconSize: 16,
                  splashRadius: 16,
                  icon: const Icon(Icons.clear),
                  onPressed: () {
                    _commandController.text = "";
                  },
                ),
              ],
            ),
          ),
          ButtonBar(
            children: [
              FlatButton(
                child: const Text('Send'),
                onPressed: () {
                  onCommand(_commandController.text);
                },
              ),
            ],
          ),
        ],
      ),
    );
  }
}
