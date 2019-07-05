#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 4444

int main(){
	struct sockaddr_in sunucuAdres;
	int yeniSoket;
	struct sockaddr_in yeniAdres;
	FILE *dosya;

	socklen_t adres_boyut;
	char buffer[1024]; //verileri tutmak için oluşturduk
	char buffer2[255];
	pid_t childpid;
	
	int sunucuSoket= socket(AF_INET, SOCK_STREAM, 0); //Soketin özelliklerini yazdık, TCP ve IpV4 türünde soket oluşturduk.
	if(sunucuSoket < 0){ // Negatif sayı döndürdüğünde soketin oluşmadığını anlıyoruz, ekrana bağlantı kurulamadı yazıp çıkış yapıyoruz.
		printf("Soket Oluşturulmadı.\n");
		exit(1);
	}
	printf("----Sunucu Soketi Oluşturuldu.\n");

	memset(&sunucuAdres, '\0', sizeof(sunucuAdres));
	sunucuAdres.sin_family = AF_INET;	//Sunucu Adresin özelliklerini giriyoruz.
	sunucuAdres.sin_port = htons(PORT);
	sunucuAdres.sin_addr.s_addr = inet_addr("127.0.0.1");

	int baglanti = bind(sunucuSoket, (struct sockaddr*)&sunucuAdres, sizeof(sunucuAdres)); // sokete bir port bağlamak için bind kullandık
	if(baglanti < 0){ // Negatif sayı döndürdüğünde bind oluşmadığını anlıyoruz, ekrana bind oluşturulamadı yazıp çıkış yapıyoruz.
		printf("Bind Oluşturulamadı.\n");
		exit(1);
	}
	printf("---Bind Portu %d\n", 4444);

	if(listen(sunucuSoket, 10) == 0){ // Bağlanan port dinliyoruz, eğer hata oluşmuşsa ekrana çıktı olarak veriyoruz.
		printf("--Dinleniyor....\n");
	} else {
		printf("Bind edilirken hata oluştu.\n");
	}
	int count = 0;
	
	while(1){
		yeniSoket = accept(sunucuSoket, (struct sockaddr*)&yeniAdres, &adres_boyut); // yeniSokete Bağlantı varsa kabul ediyoruz.

		if(yeniSoket < 0){ // Negatif sayı döndürdüğünde bağlantı oluşmadığını anlıyoruz, çıkış yapıyoruz.
			exit(1);
		}else {
		printf("%s:%d tarafından sunucuya bağlanıldı \n", inet_ntoa(yeniAdres.sin_addr), ntohs(yeniAdres.sin_port)); //Hangi adres ve hangi port üzerinden bağlandığını gösteriyoruz
		count++;
		}
		printf("%d tane istemci bağlandı \n", count); //Kaç tane istemcinin bağlandığını gösteriyoruz

		if((childpid = fork()) == 0){
			close(sunucuSoket);
			while(1){
				recv(yeniSoket, buffer, 1024, 0); // Yeni sokete gelen verileri alıyoruz

				if(strcmp(buffer, "_exit") == 0){ // eğer gelen veri _exit ise çıkış yapıyoruz. Kaç tane istemci kaldığını ekranda gösteriyoruz.
					printf("%s:%d Tarafından çıkış yapıldı \n", inet_ntoa(yeniAdres.sin_addr), ntohs(yeniAdres.sin_port));
					count--;
					printf("%d tane istemci kaldı \n", count);
					break;
				} else if (strcmp(buffer, "_send") == 0){ // eğer gelen _send ise gönderilen dosyanın içindeki verileri gelen_soya içerisine yazıyoruz.
					int ch=0;
					int kelime;
					read(yeniSoket, &kelime , sizeof(int));//kelimeleri aldık
					dosya=fopen("gelen_dosya.txt" , "a");
					printf("Dosya başarılı bir şekilde alındı.\n");

					while(ch != kelime){
						read(yeniSoket, buffer2 , 255);
						fprintf(dosya,"%s ",buffer2); //dosyanın içine teker teker yazdık
						ch++;
					}
					fclose(dosya);	//dosyayı kapatıyoruz.
				} else {
					printf("İstemci-%s:%d : %s\n",inet_ntoa(yeniAdres.sin_addr), ntohs(yeniAdres.sin_port),buffer); //Server kısmında tüm istemcilerin ortak mesajlaşmasını sağlıyoruz.
					send(yeniSoket, buffer, strlen(buffer), 0);
					bzero(buffer, sizeof(buffer));		
				}			
			}
		}
	}

	close(yeniSoket);

	return 0;
}






