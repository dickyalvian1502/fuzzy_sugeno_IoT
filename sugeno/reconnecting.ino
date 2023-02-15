void cek_wifi(){
  if (WiFi.status() == WL_CONNECTED){
    delay(100);
    led_on;
    delay(1000);
    led_off;
    delay(1000); 
    
  }else{
    delay(100);
    led_on;
    delay(100);
    led_off;
    delay(100); 
  }
}
