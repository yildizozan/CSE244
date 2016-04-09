# CSE241 System Programming
### Assoc. Prof. Erkan Zergeroğlu
Homeworks for Computer Engineering at Gebze Technical University

## Homework 01
Tekbir dosya için arama yapmamızı sağlıyor.

## Homework 02

## Homework 03
### Step 1
	checkFile() fonksiyonu gerekli parametreleri aldıktan sonra main içinden.
	klasörü açar ve while dögüsü ile teker teker okumaya başlar. (.) ve (..) olanları atlar.
	Sadece klasör ve file olmasını önemser. Öncesinde fifo, pipe ve fork yapar oluşan child readdir'in 
	okumuş olduğu dosyayı folder mı file mı diye kontrol eder. Eğer folder ise recursive fonksiyon olarak çalışır. Eğer file ise searchInFile() fonksiyonuna path, filename, aranacak kelimeyi, ve pipe file descriptionu gönderir.

### Step 2
	Şimdi searchInFile() fonksiyonun içerisindeyiz. Sayaçlar için değişkenleri tanımladık. Gelen parametrelerden yeni path oluşturduk temp file oluşturduk ve başlığını hazırladık. File açıp aramaya başlıyoruz sonuçları buldukça temp file ın içine yazıyoruz. İŞimiz bitince temp file
	kapatıp içindekileri pipe file'a yazıyoruz. Eğer hiç bir şey aradığımız kelimeyi bulamamışsak
	dosya içerisinde temp file'ı unlink yapıp siliyoruz.

### Step 3
	Pipe ile gelen verileri fifo'ya aktarmaya çok uğraştım ancak istediğim şekilde başaramadım. Bu
	Sebeple pipe'den gelen verileri log file'a yazıp olayı bitiriyorum. Hüzünle.