void get_bobot_suhu( double val_data_set,int baris){
double y,m,c;
double awal,puncak,akhir;
double konstanta=1;
double iterasi;
iterasi = max_suhu/konstanta;
double nilai_iterasi;
for (int j=0;j<baris-1;j++){
  bobot_suhu[j]=0;
}
for (int j=0;j<=baris-1;j++){
  awal=mf_sen_suhu[j][0];
  puncak=mf_sen_suhu[j][1];
  akhir=mf_sen_suhu[j][2]; 
  nilai_iterasi=0;
  for (int i=0;i<=iterasi-1;i++){
    nilai_iterasi=nilai_iterasi+konstanta;
   if (nilai_iterasi<awal)
   {
     if (j == 0){ y = 1; }
     else
     {
       y = 0;
     }
   }
   else if (nilai_iterasi>=awal && nilai_iterasi<puncak)
      {
      if (j==0) 
         { y=1;}
      else
      {m=(puncak-awal);
      m=1/m;
      c=(m*puncak);
      c=1-c;
      y=(m*nilai_iterasi);
      y=y+c;
      }
      }
  else if (nilai_iterasi>=puncak && nilai_iterasi<akhir)
     {
     if (j==baris-1) 
         {y=1;}
     else
      {
      m=(puncak-akhir);
      m=1/m;
      c=(m*puncak);
      c=1-c;
      y=(m*nilai_iterasi);
      y=y+c;
     }
     }
  else if (nilai_iterasi>=akhir )
  {
      if( j==baris-1) 
         {y=1;}
     else
          {y=0;}
       
  }
//  Serial.print(data_set[i]);
//  Serial.print(" ");
//  Serial.println(val_data_set);


if (nilai_iterasi >= val_data_set)
  {
   bobot_suhu[j]=y;
   Serial.print("Membership Function Suhu: ");
   Serial.print(j);
   Serial.print(" : ");
   Serial.println(bobot_suhu[j]);
   break;
  } 

}

}
 Serial.println("==========================");
}




void get_bobot_ph( double val_data_set,int baris){
double y,m,c;
double awal,puncak,akhir;
double konstanta=0.1;
double iterasi;
iterasi = max_ph/konstanta;
double nilai_iterasi;

for (int j=0;j<baris-1;j++){
  bobot_ph[j]=0;
}

for (int j=0;j<=baris-1;j++){
  awal=mf_sen_ph[j][0];
  puncak=mf_sen_ph[j][1];
  akhir=mf_sen_ph[j][2]; 
  nilai_iterasi=0;
//  Serial.print("AW:");
//  Serial.print(awal);
//  Serial.print(" ");
//  Serial.print(puncak);
//  Serial.print(" ");
//  Serial.print(akhir);
//  Serial.println(" ");
  
  for (int i=0;i<=iterasi-1;i++){
    nilai_iterasi=nilai_iterasi+konstanta;
    
   if (nilai_iterasi<awal)
   {
     if (j == 0){ y = 1; }
     else
     {
       y = 0;
     }
   }
   else if (nilai_iterasi>=awal && nilai_iterasi<puncak)
      {
      if (j==0) 
         { y=1;}
      else
      {m=(puncak-awal);
      m=1/m;
      c=(m*puncak);
      c=1-c;
      y=(m*nilai_iterasi);
      y=y+c;
      }
      }
  else if (nilai_iterasi>=puncak && nilai_iterasi<akhir)
     {
     if (j==baris-1) 
         {y=1;}
     else
      {
      m=(puncak-akhir);
      m=1/m;
      c=(m*puncak);
      c=1-c;
      y=(m*nilai_iterasi);
      y=y+c;
     }
     }
  else if (nilai_iterasi>=akhir )
  {
      if( j==baris-1) 
         {y=1;}
     else
          {y=0;}
       
  }
//  Serial.print(nilai_iterasi);
//  Serial.print(" ");
//  Serial.print(y);
//  Serial.print(" ");
//  Serial.println(val_data_set);


if (nilai_iterasi >= val_data_set)
  {
   bobot_ph[j]=y;
   Serial.print("Membership Function pH: ");
   Serial.print(j);
   Serial.print(" : ");
   Serial.println(bobot_ph[j]);
   break;
  } 

}

}
 Serial.println("==========================");
}
