String send(String url) {
  if (!client.connect(host, httpPort)) {
    return "Error Request Confirm";
  }
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");
  String body = "";
   long last_millis_x=millis();
  long next_millis_x=millis();
  while (client.connected()) {
      if ((millis()-last_millis_x)>10000){
        goto next3;
      }
      next_millis_x=millis();
    while (client.available()) {
       if ((millis()-next_millis_x)>10000){
        goto next3;
      }
      char c = client.read();
      body += String(c);
    }
  }
  next3:
  client.stop();
  if (body.indexOf("OK")>=0){
    body="OK";
  }else{
    body="Error Send";
  }
  return body;
}
