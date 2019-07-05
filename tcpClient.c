#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>

#define PORT 4444

int main(){

	struct sockaddr_in sunucuAdres; //verileri tutmak için oluşturduk
	char buffer[1024];
	char buffer2[255];
	
	int istemciSoket = socket(AF_INET, SOCK_STREAM, 0); //Soketin özelliklerini yazdık, TCP ve IpV4 türünde soket oluşturduk.
	if(istemciSoket < 0){				// Negatif sayı döndürdüğünde soketin oluşmadığını anlıyoruz, ekrana bağlantı kurulamadı yazıp çıkış yapıyoruz.
		printf("Soket Oluşturulmadı.\n");
		exit(1);
	}
	printf("----İstemci Soketi Oluşturuldu.\n");

	memset(&sunucuAdres, '\0', sizeof(sunucuAdres));
	sunucuAdres.sin_family = AF_INET;	//Sunucu Adresin özelliklerini giriyoruz.
	sunucuAdres.sin_port = htons(PORT);
	sunucuAdres.sin_addr.s_addr = inet_addr("127.0.0.1");

	int baglanti = connect(istemciSoket, (struct sockaddr*)&sunucuAdres, sizeof(sunucuAdres)); // Bağlantı kısmını yazpıyoruz,
	if(baglanti < 0){ // Negatif sayı döndürdüğünde bağlatının oluşmadığını anlıyoruz, ekrana bağlantı kurulamadı yazıp çıkış yapıyoruz.
		printf("Bağlanırken Hata Oluştu.\n");
		exit(1);
	}
	printf("---Server'a bağlandı.\n");

	while(1){
		printf("İstemci: \t"); 
		scanf("%s", &buffer[0]); //İstemcinin komut satırına girdiklerini buffera kaydediyoruz
		send(istemciSoket, buffer, strlen(buffer), 0); //Send komutu ile soketi gönderiyoruz.
		
		if(strcmp(buffer, "_send") == 0){ //girilen değer _send ise gönderme işlemini başlatıyoruz

			FILE *f;
			int kelime = 0;
			char c;
		
			f=fopen("gonderilen_dosya.txt" , "r"); //dosyayı açma kısmı
			while((c = getc(f)) != EOF){ //dosyanın içeriğini sonuna kadar okuma
				fscanf(f , "%s" , buffer2);// tek tek okuduk
				if(isspace(c) || c=='\t') //Boşluklara bakarak kelime sayısını bulduk 
					kelime++;
			}
			write(istemciSoket , &kelime , sizeof(int)); //Kelime sayısını istemci Sokete yazdık
			rewind(f); //başa dönmek için

			char ch;
			while(ch != EOF){ //sonuna kadar okuma kısmı
				fscanf(f, "%s" ,buffer2); 
				write(istemciSoket , buffer2 ,255);//kelimeleri teker teker sokete yazdık
				ch=fgetc(f); //ch içindeki karakteri aldık

			}
			
			send(istemciSoket, buffer2, strlen(buffer2), 0); //dosyaları karşıya gönderdik
			printf("Dosya başarılı bir şekilde gönderildi.\n");
		}

		if(strcmp(buffer, "_exit") == 0){ //girilen değer _exit ise gönderme işlemini başlatıyoruz
			close(istemciSoket); // İstemiciyi kapatıyoruz	
			printf("Server'dan çıkıldı.\n");
			exit(1);
		}	
	}
	return 0;
}
