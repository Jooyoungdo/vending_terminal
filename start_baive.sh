sleep 10
## 현재 daemon_process를 crontab으로 실행 시 권한이 없어서 image write를 못하는 이슈가 있음
## 이 부분 수정해야 함 
sudo $HOME/beyless_vending_terminal/bin/daemon_process 20002 ai0.beyless.com:1883 &
$HOME/beyless_vending_terminal/baive_ad/baive-ad.AppImage --id=20002 --url=https://ai0.beyless.com/baive &