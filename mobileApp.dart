import 'dart:async';

import 'package:flutter/material.dart';
import 'package:mqtt_client/mqtt_client.dart';
import 'package:mqtt_client/mqtt_server_client.dart';
import 'package:http/http.dart' as http;

final client=MqttServerClient("broker.hivemq.com", "esp");

Future<int> checkIntenet()async{
  try{
    var url=Uri.https('google.com');
    var response=await http.get(url);
    if(response.statusCode==200)return 0;
    else return 1;
  }catch(e){
    return 1;
  }
}
Future<int> setupMqttClient()async{
  client.logging(on:false);
  client.setProtocolV311();
  client.keepAlivePeriod=20;
  client.connectTimeoutPeriod=2000;
  client.onConnected=(){
    print("Connected");
  };
  client.onDisconnected=(){
    print("DisConnected");
  };

  final connMess=MqttConnectMessage()
  .withClientIdentifier("espEFS")
  .withWillTopic('willtopic')
  .withWillMessage("Will message")
  .startClean()
  .withWillQos(MqttQos.atLeastOnce);

  client.connectionMessage=connMess;
  try{
    await client.connect();
  }on NoConnectionException catch(e){
    print("Unable to connect");
    client.disconnect();
    return 1;
  }
  if(client.connectionStatus!.state==MqttConnectionState.connected){
    print("HiveMQ connected");
    return 0;
  }else{
    print("HiveMQ not connected");
    client..disconnect();
    return 1;
  }

}

void messageSender(msg)async{
  final messenger=MqttClientPayloadBuilder();
  messenger.addString(msg);
  client.publishMessage("efs/iotDisplay", MqttQos.atLeastOnce, messenger.payload!);
  await MqttUtilities.asyncSleep(10);
}
class IotDisplay extends StatefulWidget {
  const IotDisplay({super.key});

  @override
  State<IotDisplay> createState() => _IotDisplayState();
}

class _IotDisplayState extends State<IotDisplay> {
  TextEditingController messagecontroller=TextEditingController();
  String message='';
  bool mqttServer=false,internet=false;

  void checker(msg)async{
    try{
      if(client.connectionStatus!.state==MqttConnectionState.connected){
        messageSender(msg);
      }else{
        final result=await setupMqttClient();
        if(result==0)messageSender(msg);
        else print("Unable to send message");
      }
    }catch(e){
      print(e);
    }
  }
  late Timer timer;
  @override
  void initState() {
    // TODO: implement initState
    super.initState();
    timer=Timer.periodic(Duration(seconds:10), (_){
      checkIntenet().then((val){
        setState(() {
          internet=val==0;
          mqttServer=client.connectionStatus!.state==MqttConnectionState.connected;
        });
      });
    });
    intitalize();
  }

  void intitalize() async{
    int mqtt=await setupMqttClient();
    int web=await checkIntenet();
    setState(() {
      mqttServer=mqtt==0;
      internet=web==0;
    });
}
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      resizeToAvoidBottomInset: false,
      appBar:AppBar(
        backgroundColor:Color(0xff121417),
        centerTitle:true,
        title: Text("IOT Display",
        style: TextStyle(
            fontWeight:FontWeight.bold,
          color:Colors.white,
          fontSize:23,
        ),),
      ),
      body:Container(
        padding:EdgeInsets.all(15),
        width:double.infinity,
        color:Color(0xff121417),
        child:Column(
          mainAxisAlignment:MainAxisAlignment.spaceBetween,
          children: [
            Column(
              crossAxisAlignment:CrossAxisAlignment.start,
              children: [
                Row(
                  mainAxisSize: MainAxisSize.max,
                  mainAxisAlignment:MainAxisAlignment.spaceBetween,
                  children: [
                    ConnectBlock(name:"Internet",status:internet,icon:Icons.wifi,),
                    SizedBox(width:10,),
                    ConnectBlock(name:"MQTT",status:mqttServer,icon:Icons.storage,),
                  ],
                ),
                SizedBox(height:25,),
                Text("Message",style:TextStyle(
                  fontWeight:FontWeight.bold,
                  color:Colors.white,
                  fontSize:30,
                ),),
                SizedBox(height:15,),
                TextField(
                  controller:messagecontroller,
                  maxLines:5,
                  maxLength:100,
                  style:TextStyle(
                    color:Colors.white
                  ),
                  decoration:InputDecoration(
                    hintText:"Enter your Message",
                    border:OutlineInputBorder(
                      borderRadius:BorderRadius.circular(10),
                    )
                  ),
                ),
                SizedBox(height:25,),
                Text("Last Sent -"+message,style:TextStyle(color:Colors.white,fontSize:18),)
              ],
            ),
            ElevatedButton(
              style:ElevatedButton.styleFrom(
                backgroundColor:Color(0xff0D80F2),
                minimumSize:Size(double.infinity,50),
                shape:RoundedRectangleBorder(
                  borderRadius:BorderRadius.circular(10)
                )
              ),
                onPressed:(){
                setState(() {
                  message=messagecontroller.text;
                  checker(message);
                  messagecontroller.clear();
                });
                },
                child:Text("Send",style:TextStyle(color:Colors.white,fontSize:20),)
            )
          ],
        ),
      ),
    );
  }
}
class ConnectBlock extends StatelessWidget {
  bool status;
  String name;
  IconData icon;
  ConnectBlock({this.status=false,this.name="",this.icon=Icons.abc});
  @override
  Widget build(BuildContext context) {
    return Expanded(
      child: Container(
        decoration:BoxDecoration(
          borderRadius:BorderRadius.circular(10),
          color:status?Color(0xff0D80F2):Colors.red,
        ),
        child: ListTile(
          leading:Icon(
            icon,
            size:40,
            color:Colors.white,
          ),
          title:Text(name,style:TextStyle(color:Colors.white,fontSize:18),),
          subtitle:Text(status?"Connected":"Disconnected",style:TextStyle(color:Colors.white),),
        ),
      ),
    );
  }
}
