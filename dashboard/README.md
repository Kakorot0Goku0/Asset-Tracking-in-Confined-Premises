Command to expose the local server

ssh -R 80:localhost:3000 ssh.localhost.run


mosquitto_pub -t doctorstats -h 172.21.4.72 -m 'R24030307,nitin,F2'
mosquitto_pub -t doctorstats -h 172.21.4.72 -m 'R24030308,npar,F2'
mosquitto_pub -t doctorstats -h 172.21.4.72 -m 'R24030393,prashnt,F3'
mosquitto_pub -t doctorstats -h 172.21.4.72 -m 'R1ba9ffa1,adw,F2'
mosquitto_pub -t doctorstats -h 172.21.4.72 -m 'R1ba9ffa2,eve,F1'
mosquitto_pub -t doctorstats -h 172.21.4.72 -m 'R1ba9ffa3,esv,F2'
mosquitto_pub -t doctorstats -h 172.21.4.72 -m 'R1ba9ffa4,loj,F2'