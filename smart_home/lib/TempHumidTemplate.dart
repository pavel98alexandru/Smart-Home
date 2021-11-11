import 'dart:math';

import 'package:flutter/material.dart';

class TempHumidTemplate extends CustomPainter {

  double value;
  bool temperature_or_humidty;

  TempHumidTemplate(this.value, this.temperature_or_humidty);

  @override
  bool shouldRepaint(CustomPainter oldDelegate) {
    return true;
  }

  @override
  void paint(Canvas canvas, Size size) {
    int max = temperature_or_humidty ? 40 : 100;

    Paint genericArc = Paint()
      ..strokeWidth = 20
      ..color = Colors.lightBlueAccent
      ..style = PaintingStyle.stroke;

    Paint tempArc = Paint()
      ..strokeWidth = 20
      ..color = Colors.red
      ..style = PaintingStyle.stroke
      ..strokeCap = StrokeCap.round;

    Paint humidityArc = Paint()
      ..strokeWidth = 20
      ..color = Colors.deepPurpleAccent
      ..style = PaintingStyle.stroke
      ..strokeCap = StrokeCap.round;

    Offset center = Offset(size.width / 2, size.height / 2);
    double radius = min(size.width / 2, size.height / 2) - 14;
    canvas.drawCircle(center, radius, genericArc);
    double angle = 2 * pi * (value / max);
    canvas.drawArc(Rect.fromCircle(center: center, radius: radius), -pi / 2, angle, false, temperature_or_humidty ? tempArc : humidityArc);

  }
}