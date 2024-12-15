import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
import 'dart:convert';

class EasyPlantingApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false, // 디버그 배너 제거
      theme: ThemeData(primarySwatch: Colors.green),
      home: DashboardScreen(),
    );
  }
}

class DashboardScreen extends StatefulWidget {
  @override
  _DashboardScreenState createState() => _DashboardScreenState();
}

class _DashboardScreenState extends State<DashboardScreen> {
  String streamUrl = 'http://<ESP32-IP>/stream'; // ESP32-CAM 스트리밍 URL
  String dataUrl = 'http://<ESP32-IP>/data';    // ESP32 데이터 URL
  String controlUrl = 'http://<ESP32-IP>/control';

  double temperature = 0.0;
  double humidity = 0.0;
  bool lightOn = false;
  bool fanOn = false;

  @override
  void initState() {
    super.initState();
    fetchData();
  }

  Future<void> fetchData() async {
    try {
      final response = await http.get(Uri.parse(dataUrl));
      if (response.statusCode == 200) {
        final data = json.decode(response.body);
        setState(() {
          temperature = data['temperature'];
          humidity = data['humidity'];
          lightOn = data['light'];
          fanOn = data['fan'];
        });
      } else {
        print('Failed to load data');
      }
    } catch (e) {
      print('Error fetching data: $e');
    }
  }

  Future<void> sendControl(String device, bool state) async {
    try {
      final response = await http.get(Uri.parse('$controlUrl?device=$device&state=${state ? "on" : "off"}'));
      if (response.statusCode == 200) {
        print('$device control success');
        fetchData();
      } else {
        print('$device control failed');
      }
    } catch (e) {
      print('Error sending control: $e');
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Column(
        children: [
          // 1영역: 상단 영상 (크기 확대)
          Expanded(
            flex: 4, // 비율을 더 높임
            child: Container(
              color: Colors.black,
              child: Image.network(
                streamUrl,
                fit: BoxFit.cover,
                errorBuilder: (context, error, stackTrace) =>
                    Center(child: Text('비디오 스트리밍 실패', style: TextStyle(color: Colors.white))),
              ),
            ),
          ),
          // 2영역: 관리 박스 (크기 축소)
          Expanded(
            flex: 3, // 관리 영역 비율 축소
            child: Container(
              padding: EdgeInsets.all(12.0),
              decoration: BoxDecoration(
                color: Colors.white,
                borderRadius: BorderRadius.vertical(top: Radius.circular(16.0)),
                boxShadow: [
                  BoxShadow(color: Colors.black12, blurRadius: 5.0, offset: Offset(0, -3)),
                ],
              ),
              child: GridView.count(
                crossAxisCount: 2, // 한 줄에 2개의 박스
                mainAxisSpacing: 12.0,
                crossAxisSpacing: 12.0,
                childAspectRatio: 4 / 3, // 박스의 크기 비율을 조정
                children: [
                  // 온도 박스
                  _buildStatusCard(
                    title: '온도',
                    value: '${temperature.toStringAsFixed(1)} °C',
                    backgroundColor: Colors.blue[100],
                  ),
                  // 습도 박스
                  _buildStatusCard(
                    title: '습도',
                    value: '${humidity.toStringAsFixed(1)} %',
                    backgroundColor: Colors.green[100],
                  ),
                  // 조명 버튼 박스
                  _buildControlCard(
                    title: '조명',
                    isOn: lightOn,
                    onTap: () => sendControl('light', !lightOn),
                  ),
                  // 팬 버튼 박스
                  _buildControlCard(
                    title: '팬',
                    isOn: fanOn,
                    onTap: () => sendControl('fan', !fanOn),
                  ),
                ],
              ),
            ),
          ),
        ],
      ),
    );
  }

  // 상태 표시용 박스
  Widget _buildStatusCard({required String title, required String value, required Color? backgroundColor}) {
    return Container(
      decoration: BoxDecoration(
        color: backgroundColor,
        borderRadius: BorderRadius.circular(12.0),
        boxShadow: [BoxShadow(color: Colors.black26, blurRadius: 5.0)],
      ),
      padding: EdgeInsets.all(8.0),
      child: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        crossAxisAlignment: CrossAxisAlignment.center,
        children: [
          Text(
            title,
            style: TextStyle(fontSize: 20, fontWeight: FontWeight.bold),
          ),
          SizedBox(height: 8),
          Text(
            value,
            style: TextStyle(fontSize: 14),
          ),
        ],
      ),
    );
  }

  // 제어용 박스 (조명/팬)
  Widget _buildControlCard({required String title, required bool isOn, required VoidCallback onTap}) {
    return InkWell(
      onTap: onTap,
      child: Container(
        decoration: BoxDecoration(
          color: isOn ? Colors.orange[100] : Colors.grey[200],
          borderRadius: BorderRadius.circular(12.0),
          boxShadow: [BoxShadow(color: Colors.black26, blurRadius: 5.0)],
        ),
        padding: EdgeInsets.all(8.0),
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          crossAxisAlignment: CrossAxisAlignment.center,
          children: [
            Text(
              title,
              style: TextStyle(fontSize: 20, fontWeight: FontWeight.bold),
            ),
            SizedBox(height: 8),
            Text(
              isOn ? '켜짐' : '꺼짐',
              style: TextStyle(fontSize: 14),
            ),
          ],
        ),
      ),
    );
  }
}

void main() => runApp(EasyPlantingApp());
