import 'package:flutter/material.dart';
import 'package:flutter_signin_button/button_list.dart';
import 'package:flutter_signin_button/button_view.dart';

import 'Authentification.dart';

class Login extends StatelessWidget {

  Future<void> signInWithGoogle() async {
    await Authentification().signIn();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: Colors.white,
      appBar: AppBar(
        title: Text("Login",style: TextStyle(fontSize: 20, color: Colors.grey)),
        backgroundColor: Colors.white,
        centerTitle: true,
        automaticallyImplyLeading: false,
      ),
      body: ListView(
        children: <Widget>[
          SizedBox(height: 50),
          Image.asset('assets/logo.png'),
          Image.asset('assets/smart_home.gif'),
          SignInButton(
            Buttons.Google,
            text: "Login with Google",
            onPressed: () => signInWithGoogle(),
          ),
      ]
    ),
    );
  }
}