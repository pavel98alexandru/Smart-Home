import 'dart:async';

import 'package:firebase_auth/firebase_auth.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:flutter/cupertino.dart';
import 'package:flutter/material.dart';
import 'package:smart_home/TempHumidTemplate.dart';
import 'package:vibration/vibration.dart';

import 'Authentification.dart';

class Monitoring extends StatefulWidget {

  @override
  _MonitoringState createState() => _MonitoringState();
}

class _MonitoringState extends State<Monitoring> {

  final dataBaseReference= FirebaseDatabase.instance.reference();
  List<String>? receivedData;
  bool loading = false;
  Timer? _timer;
  double temp=0;
  double humid=0;
  int ac_status=0;
  int heating_status=0;
  int security_status=0;
  int irrigation_status=0;
  double desiredTemp=22;
  double irrigationDuration=10;
  int fire_detected=0;
  int gas_detected=0;
  int presence_detected=0;
  int fire_detected_copy=0;
  int gas_detected_copy=0;
  int presence_detected_copy=0;
  String sendData="";
  int sendTemp=0;
  int sendAC=0;
  int sendHeating=0;
  int sendIrrigation=0;
  int sendSecurity=0;
  int sendOpenLock=0;

  get myOnTap => null;

  void sendDataFirebase()
  {
    sendData=sendTemp.toString()+","+sendAC.toString()+","+sendHeating.toString()+","+sendIrrigation.toString()+","+sendSecurity.toString()+","+sendOpenLock.toString();
    dataBaseReference.child('App Side/Temperature,AC,Heating,Irrigation,Security,Lock').set(sendData);
  }

  Future<List<String>> getDataFirebase() async {
    String result = (await dataBaseReference.child('System Side/Temperature,Humidty,FireDetected,GasDetected,PresenceDetected,Security,AC,Heating,Irrigation').once()).value;
    List<String> list = result.split(",");
    return list;
  }

  Future<void> signOut() async {
    await Authentification().signOut();
  }

  @override
  void initState() {
    super.initState();
    _timer = Timer.periodic(Duration(milliseconds: 1000), _generateTrace);
  }

  _generateTrace(Timer t)
  {
    if (this.mounted){
    setState(() {
      getDataFirebase().then((List<String> s) => setState(() { receivedData = s;
      loading=true;
      temp=double.parse(receivedData![0]);
      humid=double.parse(receivedData![1]);
      security_status=int.parse(receivedData![5]);
      ac_status=int.parse(receivedData![6]);
      heating_status=int.parse(receivedData![7]);
      irrigation_status=int.parse(receivedData![8]);
      fire_detected=int.parse(receivedData![2]);
      gas_detected=int.parse(receivedData![3]);
      presence_detected=int.parse(receivedData![4]);
      if(fire_detected==1 && fire_detected_copy==0){_showAlert(context,"FIRE");}
      if(gas_detected==1 && gas_detected_copy==0){_showAlert(context,"GAS LEAK");}
      if(presence_detected==1 && presence_detected_copy==0){_showAlert(context,"MOVEMENT");}
      fire_detected_copy=fire_detected;
      gas_detected_copy=gas_detected;
      presence_detected_copy=presence_detected;
      }));
    });}
  }

  void _showAlert(BuildContext context, String type) {
    Vibration.vibrate(duration: 1000);
    showDialog(
      barrierColor: Colors.redAccent,
        context: context,
        builder: (context) => AlertDialog(
          backgroundColor: Colors.red,
              title: type!="MOVEMENT" ? Text("EMERGENCY ALERT" ,textAlign: TextAlign.center, style: TextStyle(fontSize: 30, fontWeight: FontWeight.bold, color: Colors.black)) : Text("SECURITY ALERT!",textAlign: TextAlign.center, style: TextStyle(fontSize: 20, fontWeight: FontWeight.bold, color: Colors.black)),
          content: Text("Reason: "+type+" DETECTED",textAlign: TextAlign.center,style: TextStyle(fontSize: 20, color: Colors.black)),
        )
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
        backgroundColor: Colors.lightBlue,
      appBar: AppBar(
        title: Text('Smart Home Dashboard'),
        centerTitle: true,
        automaticallyImplyLeading: false,
      ),

      body: loading ? ListView(
            children: <Widget>[
              SizedBox(height: 50),
              CircleAvatar(
                  radius:130,
                  backgroundColor: Colors.lightBlueAccent,
                  child: IconButton(onPressed: () {
                    sendOpenLock=1;
                    sendDataFirebase();
                    sendOpenLock=0;
                  },
                      icon: Image.asset('assets/lock_logo.png'),
                      iconSize: 240)
              ),
              SizedBox(height: 30),
              CustomPaint(
              foregroundPainter:
                TempHumidTemplate(temp, true),
                child: Container(
                  width:250,
                  height: 250,
                  child: Center(
                    child: Column(
                      mainAxisAlignment: MainAxisAlignment.center,
                      children: <Widget>[
                        Text('Temperature',  style: TextStyle(
                            fontSize: 20, fontWeight: FontWeight.bold, color: Colors.white)),
                        Text(
                          '${temp}',
                          style: TextStyle(
                              fontSize: 35, fontWeight: FontWeight.bold, color: Colors.white),
                        ),
                        Text(
                          '°C',
                          style: TextStyle(
                              fontSize: 25, fontWeight: FontWeight.bold, color: Colors.white),
                        ),
                      ],
                    ),
                  ),
                ),
              ),
              SizedBox(height: 30),
              TextButton(
                style: TextButton.styleFrom(
                  padding: const EdgeInsets.all(16.0),
                  primary: Colors.white,
                  textStyle: const TextStyle(fontSize: 20,fontWeight: FontWeight.bold),
                ),
                onPressed: () { sendTemp= desiredTemp.round(); sendAC=0; sendHeating=0; sendDataFirebase();},
                child: Text('Set temperature to '+desiredTemp.round().toString()+"°C"),
              ),
              SizedBox(height: 30),

              Slider(
                value: desiredTemp,
                min: 10,
                max: 29,
                divisions: 19,
                label: desiredTemp.round().toString()+"°C",
                onChanged: (double value) {
                setState(() {
                  desiredTemp = value;
               });
              },
              ),
              SizedBox(height: 30),
              CustomPaint(
                foregroundPainter:
                TempHumidTemplate(humid, false),
                child: Container(
                  width: 250,
                  height: 250,
                  child: Center(
                    child: Column(
                      mainAxisAlignment: MainAxisAlignment.center,
                      children: <Widget>[
                        Text('Humidity',  style: TextStyle(
                            fontSize: 20, fontWeight: FontWeight.bold, color: Colors.white)),
                        Text(
                          '${humid}',
                          style: TextStyle(
                              fontSize: 35, fontWeight: FontWeight.bold, color: Colors.white),
                        ),
                        Text(
                          '%',
                          style: TextStyle(
                              fontSize: 25, fontWeight: FontWeight.bold, color: Colors.white),
                        ),
                      ],
                    ),
                  ),
                ),
              ),
              SizedBox(height: 30),
            CircleAvatar(
            radius:130,
            backgroundColor: ac_status == 0 ? Colors.red : Colors.greenAccent,
              child: IconButton(onPressed: () {
                sendTemp=0;
                if(ac_status==0) {sendAC=1; sendHeating=0;}
                else {sendAC=0;}
                sendDataFirebase();
                },
                  icon: Image.asset('assets/AC_logo.png'),
              iconSize: 220)
            ),
              SizedBox(height: 30),
              CircleAvatar(
                  radius:130,
                  backgroundColor: heating_status == 0 ? Colors.red : Colors.greenAccent,
                  child: IconButton(onPressed: () {
                    sendTemp=0;
                    if(heating_status==0) {sendHeating=1; sendAC=0;}
                    else {sendHeating=0;}
                    sendDataFirebase();
                  },
                      icon: Image.asset('assets/heating_logo.png'),
                      iconSize: 220)
              ),
              SizedBox(height: 30),
              CircleAvatar(
                  radius:130,
                  backgroundColor: irrigation_status == 0 ? Colors.red : Colors.greenAccent,
                  child: IconButton(onPressed: () {
                    if(irrigation_status==0) {sendIrrigation=irrigationDuration.round();}
                    else {sendIrrigation=0;}
                    sendDataFirebase();
                  },
                      icon: Image.asset('assets/irrigation_logo.png'),
                      iconSize: 220)
              ),
              SizedBox(height: 30),
              TextButton(
                style: TextButton.styleFrom(
                  padding: const EdgeInsets.all(16.0),
                  primary: Colors.white,
                  textStyle: const TextStyle(fontSize: 20,fontWeight: FontWeight.bold),
                ),
                onPressed: () {},
                child: Text('Irrigation duration: '+irrigationDuration.round().toString()+" minutes"),
              ),
              SizedBox(height: 30),
              Slider(
                value: irrigationDuration,
                min: 5,
                max: 30,
                divisions: 25,
                label: irrigationDuration.round().toString()+" minutes",
                onChanged: (double value) {
                  setState(() {
                    irrigationDuration = value;
                  });
                },
              ),
              SizedBox(height: 30),
              CircleAvatar(
                  radius:130,
                  backgroundColor: security_status == 0 ? Colors.red : Colors.greenAccent,
                  child: IconButton(onPressed: () {
                    if(security_status==0) {sendSecurity=1;}
                    else {sendSecurity=0;}
                    sendDataFirebase();
                  },
                      icon: Image.asset('assets/security_logo.png'),
                      iconSize: 220)
              ),
              SizedBox(height: 50),
              TextButton(
                style: TextButton.styleFrom(
                  padding: const EdgeInsets.all(16.0),
                  primary: Colors.white,
                  backgroundColor: Colors.blueAccent,
                  textStyle: const TextStyle(fontSize: 20),
                ),
                onPressed: signOut,
                child: Text("Log out"),
              ),
              SizedBox(height: 50)
            ],
          )
                    : Container(child: Center(child: Text('Loading...',  textAlign: TextAlign.center,
        style: TextStyle(fontSize: 30, fontWeight: FontWeight.bold,  color: Colors.white),)))
      );
  }
}
