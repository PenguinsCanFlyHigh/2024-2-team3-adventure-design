from flask import Flask, Response
from flask_cors import CORS
import cv2
import time
import socket
import netifaces
from zeroconf import ServiceInfo, Zeroconf

app = Flask(__name__)
CORS(app)

# 웹캠 객체 생성
camera = cv2.VideoCapture(0)

def generate_frames():
    while True:
        success, frame = camera.read()
        if not success:
            break
        else:
            # JPEG로 프레임 인코딩
            _, buffer = cv2.imencode('.jpg', frame)
            frame = buffer.tobytes()
            # 스트리밍에 필요한 MIME 멀티파트 형식으로 프레임 전송
            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')
            time.sleep(0.1)

@app.route('/stream')
def stream():
    return Response(generate_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

def get_local_ip():
    """
    가장 신뢰할 수 있는 로컬 IP 주소를 찾는 함수
    네트워크 인터페이스를 순회하며 실제 IP 주소 탐색
    """
    for interface in netifaces.interfaces():
        try:
            # IPv4 주소 확인
            addrs = netifaces.ifaddresses(interface)
            if netifaces.AF_INET in addrs:
                ip = addrs[netifaces.AF_INET][0]['addr']
                # 루프백 주소와 가상 인터페이스 제외
                if not ip.startswith('127.') and not ip.startswith('169.'):
                    return ip
        except ValueError:
            continue
    
    # 대체 방법: 소켓 연결을 통한 IP 확인
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        s.connect(("8.8.8.8", 80))
        local_ip = s.getsockname()[0]
    except Exception:
        local_ip = '127.0.0.1'
    finally:
        s.close()
    return local_ip

def register_mdns():
    """
    mDNS 서비스 등록 및 세부 설정
    더 안정적이고 상세한 서비스 정보 제공
    """
    local_ip = get_local_ip()
    print(f"Detected Local IP: {local_ip}")
    
    zeroconf = Zeroconf()
    
    # 서비스 정보 상세 설정
    service_info = ServiceInfo(
        "_http._tcp.local.",  # 서비스 타입
        "epstream._http._tcp.local.",  # 고유한 서비스 이름
        addresses=[socket.inet_aton(local_ip)],  # IP 주소 설정
        port=8080,
        properties={
            'path': '/stream',  # 스트리밍 경로 명시
            'type': 'video/jpeg',  # MIME 타입 지정
            'description': 'Flask Video Streaming Service'  # 서비스 설명
        }
    )
    
    try:
        zeroconf.register_service(service_info)
        print(f"mDNS 서비스 등록 성공: http://epstream.local:8080/stream")
        return zeroconf
    except Exception as e:
        print(f"mDNS 서비스 등록 실패: {e}")
        return None

if __name__ == '__main__':
    zeroconf = register_mdns()
    try:
        # 모든 네트워크 인터페이스에서 접근 가능하도록 설정
        app.run(host='0.0.0.0', port=8080, debug=True, threaded=True)
    except KeyboardInterrupt:
        print("서버 종료")
    finally:
        # mDNS 서비스 안전하게 종료
        if zeroconf:
            zeroconf.close()
        camera.release()