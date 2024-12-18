import 'package:flutter/material.dart';
import 'package:flutter_mjpeg/flutter_mjpeg.dart';
import 'package:http/http.dart' as http;
import 'dart:convert';
import 'package:google_fonts/google_fonts.dart';

class EasyPlantingApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      theme: ThemeData(
        // 앱 테마 설정: 색상 및 폰트
        colorScheme: ColorScheme.fromSeed(
          seedColor: Color(0xFF2E7D32), // 주요 테마 색상
          primary: Color(0xFF2E7D32),
          secondary: Color(0xFF81C784),
          background: Color(0xFFF0F4F0), // 부드러운 배경색
          surface: Colors.white,
        ),
        textTheme: TextTheme(
          bodyLarge: GoogleFonts.montserrat(),
          bodyMedium: GoogleFonts.montserrat(),
          displayLarge: GoogleFonts.montserrat(),
        ),
      ),
      home: DashboardScreen(), // 앱 메인 화면
    );
  }
}

class DashboardScreen extends StatefulWidget {
  @override
  _DashboardScreenState createState() => _DashboardScreenState();
}

class _DashboardScreenState extends State<DashboardScreen> {
  // 비디오 스트리밍과 데이터 URL 설정, mDNS 사용시 'http://epstream.local:8080/stram' 주소 사용
  String streamUrl = 'http://192.168.0.70:8080/stream'; // MJPEG 비디오 스트리밍 URL
  String dataUrl = 'http://192.168.0.70:8080/data'; // 온습도 데이터 URL

  // 온습도 데이터 변수
  double temperature = 0.0;
  double humidity = 0.0;

  // 데이터 로딩 상태
  bool _isLoading = true;

  @override
  void initState() {
    super.initState();
    fetchData(); // 초기 화면에서 온습도 데이터 가져오기
  }

  // 온습도 데이터를 가져오는 함수
  Future<void> fetchData() async {
    try {
      final response = await http.get(Uri.parse(dataUrl)); // HTTP GET 요청
      if (response.statusCode == 200) {
        final data = json.decode(response.body); // JSON 데이터를 파싱
        setState(() {
          temperature = data['temperature']; // 온도 값 설정
          humidity = data['humidity']; // 습도 값 설정
          _isLoading = false; // 데이터 로딩 상태 해제
        });
      } else {
        setState(() {
          _isLoading = false;
        });
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text('데이터를 불러오는 데 실패했습니다.')),
        );
      }
    } catch (e) {
      // 네트워크 오류 시 예외 처리
      setState(() {
        _isLoading = false;
      });
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text('네트워크 오류: $e')),
      );
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: SafeArea(
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.stretch,
          children: [
            // 앱 상단: 타이틀과 새로고침 버튼
            Padding(
              padding: const EdgeInsets.all(16.0),
              child: Row(
                mainAxisAlignment: MainAxisAlignment.spaceBetween,
                children: [
                  Text(
                    'Easy Planting',
                    style: GoogleFonts.montserrat(
                      fontSize: 24,
                      fontWeight: FontWeight.bold,
                      color: Color(0xFF2E7D32),
                    ),
                  ),
                  IconButton(
                    icon: Icon(Icons.refresh, color: Color(0xFF2E7D32)),
                    onPressed: fetchData, // 새로고침 버튼 클릭 시 데이터 다시 가져오기
                  ),
                ],
              ),
            ),

            // 비디오 스트리밍 영역
            Expanded(
              flex: 7, // 화면 비율 7:3
              child: Padding(
                padding: const EdgeInsets.symmetric(horizontal: 16.0),
                child: Container(
                  decoration: BoxDecoration(
                    borderRadius: BorderRadius.circular(20),
                    boxShadow: [
                      BoxShadow(
                        color: Colors.black12,
                        blurRadius: 10,
                        offset: Offset(0, 5),
                      ),
                    ],
                  ),
                  child: ClipRRect(
                    borderRadius: BorderRadius.circular(20),
                    child: _isLoading
                        ? Center(
                      child: CircularProgressIndicator(
                        color: Color(0xFF2E7D32),
                      ),
                    )
                        : Mjpeg(
                      stream: streamUrl,
                      isLive: true,
                      fit: BoxFit.cover,
                      error: (context, error, stackTrace) => Center(
                        child: Column(
                          mainAxisAlignment: MainAxisAlignment.center,
                          children: [
                            Icon(
                              Icons.error_outline,
                              color: Colors.red,
                              size: 50,
                            ),
                            SizedBox(height: 16),
                            Text(
                              '비디오 스트리밍 실패',
                              style: GoogleFonts.montserrat(
                                color: Colors.black87,
                                fontSize: 18,
                                fontWeight: FontWeight.bold,
                              ),
                            ),
                          ],
                        ),
                      ),
                    ),
                  ),
                ),
              ),
            ),

            // 온도 및 습도 상태 카드 영역
            Expanded(
              flex: 3, // 화면 비율 7:3
              child: Padding(
                padding: const EdgeInsets.all(16.0),
                child: Row(
                  mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                  children: [
                    // 온도 상태 카드
                    _buildStatusCard(
                      '온도',
                      '${temperature.toStringAsFixed(1)} °C',
                      Icons.thermostat,
                      Color(0xFFFF7043), // 따뜻한 주황색
                    ),
                    // 습도 상태 카드
                    _buildStatusCard(
                      '습도',
                      '${humidity.toStringAsFixed(1)} %',
                      Icons.water_drop,
                      Color(0xFF29B6F6), // 밝은 파란색
                    ),
                  ],
                ),
              ),
            ),
          ],
        ),
      ),
    );
  }

  // 상태 카드 위젯 생성 함수
  Widget _buildStatusCard(
      String title, String value, IconData icon, Color color) {
    return Container(
      width: 160,
      decoration: BoxDecoration(
        gradient: LinearGradient(
          colors: [color.withOpacity(0.7), color], // 상자 그라데이션 효과
          begin: Alignment.topLeft,
          end: Alignment.bottomRight,
        ),
        borderRadius: BorderRadius.circular(20),
        boxShadow: [
          BoxShadow(
            color: color.withOpacity(0.5),
            blurRadius: 10,
            offset: Offset(0, 5),
          ),
        ],
      ),
      padding: EdgeInsets.all(16),
      child: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          Icon(icon, color: Colors.white, size: 40), // 상태 아이콘
          SizedBox(height: 10),
          Text(
            title,
            style: GoogleFonts.montserrat(
              fontSize: 16,
              fontWeight: FontWeight.w600,
              color: Colors.white,
            ),
          ),
          SizedBox(height: 6),
          Text(
            value,
            style: GoogleFonts.montserrat(
              fontSize: 22,
              fontWeight: FontWeight.bold,
              color: Colors.white,
            ),
          ),
        ],
      ),
    );
  }
}

void main() => runApp(EasyPlantingApp());
