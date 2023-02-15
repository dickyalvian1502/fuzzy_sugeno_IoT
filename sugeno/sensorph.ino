//ADS1115 READ
void get_ph(){
  int samples = 10;
  float measurings=0;
  for (int i = 0; i < samples; i++)
  {
    measurings += adc.convert(ADS1115_CHANNEL3, ADS1115_RANGE_6144);
    delay(10);
  }
  float teg0 = measurings/samples;
  float voltage=scale0.scale(teg0)/1000;
  ph = 7+((2.5 - voltage) / 0.18);
  ph = ph-0.6;
  if (ph<0){ph=0;}
//  Serial.print(teg0);
//  Serial.print("->");
//  Serial.print(voltage);
//  Serial.print("-> PH:");
//  Serial.println(ph);
}
