/**
 * @file flash.hpp
 * @author Alexandre Charrier
 * @version 1.0
 * @date 20 Fev 2019
 * @brief Librarie permettant d'acceder � la m�moire flash (AT45DQ321 30Mbit) via un port SPI pour le fonctionnement du R�volution (Afficheur DEL rotatif).
 *
 * La m�moire flash est configur�e principalement pour contenir et acceder des images BMP 256 x 192 x 16bit.
 * Des donn�es de configuration servant au bon fonctionnement de l'afficheur DEL y sont �galement sauvegard�es.
 *
 * Les images sauvegard�s sont pr�alablement trait�es par l'entremise du serveur WEB du R�volution (afficheur DEL)
 * pour �tre directement transf�r� dans les LED Drivers TLC5955.
 * Elles sont sauvegard�es colonnes par colonnes et elles sont lues de la m�me fa�on.
 *
 */

#ifndef FLASH_HPP_
#define FLASH_HPP_

#include "stm32f4xx_spi.h"
#include "hardware.h"
#include <stdio.h>

class Flash {
public:
	///Nombre d'image � afficher dans le carrousel
	uint8_t numberOfImageInCarrousel = 0;
	///Positions des images dans le carrousel. Ex:xx00000000000101 = Images 0 et 2 pr�sentes.
	uint16_t positionOfPresentImages = 0;
	///Param�tre de courrant Maximum pour le TLC5955
	uint8_t redMaxCurrent = 127;
	///Param�tre de courrant Maximum pour le TLC5955
	uint8_t greenMaxCurrent = 127;
	///Param�tre de courrant Maximum pour le TLC5955
	uint8_t blueMaxCurrent = 127;
	///Param�tre de courrant Maximum pour le TLC5955
	uint8_t globalBrightness = 127;
	/**
	 * Structure d'address pour acc�der la m�moire flash.
	 * Les 8 191 pages contiennent 512 bytes chacunes.
	 */
	struct address_t {
		uint16_t page = 0;
		uint16_t byte = 0;
	};
	/**
	 *
	 * @param debug : Active la sortie des messages de d�verminage vers la console
	 * @return L'instance de l'objet FLash
	 */
	static Flash* getInstance(bool debug = 0);
	///Images maximums pouvant �tre sauvegard�es dans la flash
	static constexpr uint8_t MaxImageStored = 14;
	///Nombre de caract�res maximum par nom de fichier
	static constexpr uint16_t FilenameSize = 16;
	///Taille du tableau de donn�s envoy� vers trois TLC5955 par port SPI
	static constexpr uint16_t SPIBufferSize = 289;
	///Taille total d'une colonne de 192 pixels
	static constexpr uint16_t ColumnPixelArraySize = 1156;
	///Taille d'un fichier d'image
	static constexpr uint32_t ImageFileSize = 295936;
	///Nombre de pages de m�moire par fichier d'image.
	static constexpr uint16_t PagesPerImage = 578;
	///Adresse de la premi�re image du carrousel
	static constexpr uint8_t FirstImagePageAddress = 99;
	///Taille des pages de m�moire flash en Byte
	static constexpr uint16_t PageSize = 512;
	///R�solution de l'axe de X maximum
	static constexpr uint16_t MaxColumnCount = 256;
	/**
	 * Destructeur de l'objet
	 */
	~Flash();
	/**
	 * Initialise le port SPI5 du STM32F411RE
	 */
	void init();
	/**
	 * Methode d'envoie et de r�ception pour le port SPI
	 * @param data : Donn� en byte � transf�rer dans le port SPI
	 * @return byte lu par le port SPI
	 */
	uint8_t inline spiTransfer(uint8_t data);
	/**
	 * Active et d�sactive le Chip Select du port SPI
	 * @param state : 1 = active le Chip Select,  0 = d�sactive le Chip Select
	 */
	void setCS(bool state);
	/**
	 * Int�roge la flash sur son �tat d'activit�
	 * @return busy state. 0 = Flash est pr�t pour une nouvelle op�ration
	 */
	bool isBusy();
	/**
	 * Lie le regsitre d'�tat de la m�moire flash
	 * @return Le registre d'�tat de la flash
	 */
	uint16_t readStatusRegister();
	/**
	 * Lie le regsitre de configuration de la m�moire flash
	 * @return Le registre de configuration de la flash
	 */
	uint8_t readConfigurationRegister();
	/**
	 * Lie le registre de configuration et l'envoie en format texte dans la console
	 */
	void readStatusRegisterToString();
	/**
	 * �crit un byte dans la flash � une adresse donn�e
	 * @param add : Adresse de la flash
	 * @param byte : Byte � �crire
	 */
	void writeByte(const address_t *add, uint8_t byte);
	/**
	 * �crit un tableau de byte dans la flash � une adresse donn�e
	 * @param add : Adresse de la flash
	 * @param byte : Tableau de uint8_t � transf�rer
	 * @param nByte : nombre de bytes � tranf�rer. � noter que les pages contiennent 512 Bytes.
	 * si l'�criture d�passe le byte 511, l'�criture se continue au d�but de la m�me page au byte 0.
	 * @param offsetByte : D�calage de lecture du tableau � transf�rer.
	 * Ex:  235 signifirait que la lecture des donn�s � �crire dans la flash d�buterait � la 235e case du tableau (byte[235]).
	 */
	void writeByte(const address_t *add, uint8_t *byte, uint16_t nByte,
			uint16_t offsetByte = 0);
	/**
	 * �crit un tableau de byte dans la flash � une adresse donn�e
	 * @param add : Adresse de la flash de d�part
	 * @param byte : Tableau de uint8_t � transf�rer
	 * @param nByte : nombre de bytes � tranf�rer. � noter que les pages contiennent 512 Bytes.
	 * si l'�criture d�passe le byte 511, l'�criture se continuera au d�but de la m�me page au byte 0.
	 */
	void writeByte(const address_t *add, const char *byte, uint16_t nByte);
	/**
	 * �crit les deux bytes d'un uint16_t dans la flash
	 * @param add : Adresse de la flash de d�part
	 * @param byte : uint16_t � �crire (2 bytes)
	 */
	void writeByteU16(const address_t *add, uint16_t byte);
	/**
	 * �crit les quatre bytes d'un uint32_t dans la flash
	 * @param add : Adresse de la flash de d�part
	 * @param byte : uint32_t � �crire (4 bytes)
	 */
	void writeByteU32(const address_t *add, uint32_t byte);
	/**
	 * Lie un byte dans la flash � une adresse donn�e
	 * @param add : Adresse de la flash
	 * @return Le byte lu � l'adresse
	 */
	uint8_t readByte(const address_t *add);
	/**
	 * Lie une s�rie de bytes dans la flash � partir d'une adresse donn�e
	 * @param add : Adresse de la flash
	 * @param buffer : Tableau dans laquelle la lecture sera tranf�r�e.
	 * @param nBytes : nombre de bytes � transf�rer. � noter que les pages contiennent 512 Bytes.
	 * Si la lecture d�passe le byte 511, la lecture se continuera au d�but de la m�me page au byte 0.
	 */
	void readByte(const address_t *add, uint8_t *buffer, uint16_t nBytes);
	/**
	 * Lie une s�rie de bytes dans la flash � partir d'une adresse donn�e
	 * @param add : Adresse de la flash
	 * @param buffer : Tableau dans laquelle la lecture sera tranf�r�e.
	 * @param nBytes : nombre de bytes � transf�rer. � noter que les pages contiennent 512 Bytes.
	 * Si la lecture d�passe le byte 511, la lecture se continuera au d�but de la m�me page au byte 0.
	 */
	void readByte(const address_t *add, char *buffer, uint16_t nBytes);
	/**
	 * Lecture en continue d'une page � l'autre.
	 * La lecture se fait en continue et la flash incr�mente
	 * automatiquement l'adresse pour lire les pages pr�c�dantes.
	 * @param add : Adresse de la flash
	 * @param buffer : Tableau dans laquelle la lecture sera tranf�r�e.
	 * @param nBytes : nombre de bytes � transf�rer. La lecture se fait en continue et la flash incr�mente
	 * automatiquement l'adresse pour lire les pages pr�c�dantes.
	 */
	void readPageArray(const address_t *add, uint8_t *buffer, uint32_t nByte);
	/**
	 * Efface le contenue de la page � l'adresse sp�cifi�e.
	 * @param add : Adresse de la flash
	 */
	void formatPage(const address_t *add);
	/**
	 * Efface la m�moire enti�re de la flash.
	 */
	void formatChip();
	/**
	 * D�sactive la protection d'�criture s�lective de la flash
	 */
	void disableSectorProtection();
	/**
	 * Lie l'indetification du manifacturier dans la flash
	 * @param buffer : Tableau de grandeur minimum de 5 dans lequelle le Device id est retourn�.
	 */
	void getDeviceID(uint8_t *buffer);
	/**
	 * M�thode d'initialisation des formats de page en 512 Bytes par page de m�moire.
	 */
	void setPageSizeBinary();
	/**
	 * Retrouve les donn�es de controle du TLC5955 enregistr�es dans la Flash. Ces donn�es sont emagasin�es
	 * dans les membres de l'objet : redMaxCurrent, greenMaxCurrent, blueMaxCurrent et gloalBrightness.
	 */
	void readControlRegister();
	/**
	 * Sauvegarde les donn�es de controle du TLC5955 enregistr�es dans la Flash. Ces donn�es proviennent
	 * dess membres de l'objet : redMaxCurrent, greenMaxCurrent, blueMaxCurrent et gloalBrightness.
	 */
	void writeControlRegister();
	/**
	 * Int�roge la flash pour savoir les images pr�sentes dans le caroussel
	 * @return uint16_t contenant les positions d'images pr�sentes dans le carrousel.
	 * Chaque bit repr�sente une image (Max 14 images)
	 * Ex: xx00010000000011 ----> Images 0, 1 et 10 pr�sentes dans le carrousel.
	 */
	uint16_t getPositionOfPresentImagesInCarrousel();
	/**
	 * Sauvegarde le positionnement des images pr�sentes dans le carrousel.
	 * Chaque bit repr�sente une image (Max 14 images)
	 * Ex: xx00010000000011 ----> Images 0, 1 et 10 pr�sentes dans le carrousel.
	 */
	void savePositionOfPresentImagesInCarrousel();
	/**
	 * Inscrit une image � afficher dans le carrousel
	 * @param imageNo : Le num�ro de l'image � inscrire du carrousel.
	 */
	void setImageInCarrousel(uint8_t imageNo);
	/**
	 * Retire une image � afficher dans le carrousel
	 * @param imageNo : Le num�ro de l'image � retirer du carrousel.
	 */
	void resetImageInCarrousel(uint8_t imageNo);
	/**
	 * Vide le carrousel
	 */
	void formatCarrousel();
	/**
	 * Retourne le nombre d'image inscrites dans le carrousel
	 * @return Le nombre d'images disponible dans le carrousel.
	 */
	uint8_t getNumberOfImagesInCarrousel();
	/**
	 * Retourne le prochain emplacement libre pour sauvegader une image.
	 * Si le carrousel est plein, l'image no 0 sera �cras�e.
	 * @return uint8_t avec la valeur d�cimal de l'emplacement vide
	 */
	uint8_t getNextFreeImageSlot();
	/**
	 * Retrouve le nom de fichier d'une image
	 * @param imageNo : Le num�ro d'image.
	 * @param destination : Tableau dans lequel le nom sera transf�r�. Le tableau doit au minimum de FilenameSize.
	 */
	void getFilename(uint8_t imageNo, char *destination);
	/**
	 * �crit le nom du fichier d'un image
	 * @param imageNo : Le num�ro de l'image.
	 * @param fileName : Tableau du nom du fichier. Si le tableau est plus grad que FilenameSize
	 */
	void setFilename(uint8_t imageNo, uint8_t *fileName);
	/**
	 * Remet � z�ro l'espace allou� pour le nom de fichier d'une image
	 * @param imageNo
	 */
	void resetFilename(uint8_t imageNo);
	/**
	 * Lie une colonne d'affichage emagasin� dans la flash par l'entremise de la DMA
	 * @param imageNo : Num�ro de l'image
	 * @param columnNo : Num�ro de la colonne
	 * @param spiBuffer : Tableau dans lequel les donn�es sont transf�r�s
	 * @return 1 quand le transf�re est compl�t�
	 */
	bool getPixelColumnDMA(uint8_t imageNo, uint8_t columnNo,
			uint8_t* spiBuffer);
	/**
	 * Lie une colonne d'affichage emagasin� dans la flash
	 * @param imageNo : Num�ro de l'image
	 * @param columnNo : Num�ro de la colonne
	 * @param spiBuffer : Tableau dans lequel les donn�es sont transf�r�s
	 * @return 1 quand le transf�re est compl�t�
	 */
	bool getPixelColumn(uint8_t imageNo, uint8_t columnNo, uint8_t* spiBuffer);
	/**
	 * Sauvegarde une colonne d'affichage dans la flash
	 * @param imageNo : Num�ro de l'image
	 * @param columnNo : Num�ro de la colonne
	 * @param spiBuffer : Tableau dans lequel les donn�es sont transf�r�s
	 * @return 1 quand le transf�re est compl�t�
	 */
	bool savePixelColumn(uint8_t imageNo, uint8_t columnNo, uint8_t* source);
	/**
	 * Transmet un colonne d'image n format texte vers la console de d�verminage
	 * @param imageNo : Num�ro de l'image
	 * @param columnNo : Num�ro de la colonne
	 */
	void getPixelColumnToString(uint8_t imageNo, uint8_t columnNo);
	/**
	 * Permet la sortie des message de d�verminage vers la console
	 * @param debug 1 = Fonctionnel, 0 = Non fonctionnel
	 */
	void setDebug(bool debug);
	/**
	 * Compte le nombe de set bits dans une variable
	 * @param n La variable � comter
	 * @return le nombre de bits � 1 dans la variable
	 */
	uint8_t countSetBits(uint32_t n);

private:
	/**
	 * Constructeur par d�faut
	 */
	Flash();
	/**
	 * Contructeur optionnel
	 * @param debugEnable Configure la console de d�verminage
	 */
	Flash(bool debugEnable);
	/**
	 * Membre static d'instance de l'objet
	 */
	static Flash* instance;

	/**
	 * Flash read/write and SPI command
	 */

	///Commande SPI FLASH BusyFlag
	static constexpr uint16_t BusyFlag = 0x8080;
	///Commande SPI FLASH ReadWrite Error flag
	static constexpr uint16_t ReadWriteErrorFlag = 0x0020;
	///Commande SPI FLASH Dummybyte pour la lecture de la flash
	static constexpr uint8_t DummyByte = 0x00;
	///Commande SPI FLASH Lecture de la m�moire flash
	static constexpr uint8_t MainMemmoryPageRead = 0xD2;
	///Commande SPI FLASH Lecture en continue avec incr�mentation automatique de page
	static constexpr uint8_t ContinuousPageRead = 0x1B;
	///Commande SPI FLASH �criture dans la flash via le Buffer 1
	static constexpr uint8_t WrtitePagesThroughBuf1BIE = 0x82;
	///Commande SPI FLASH �criture dans la flash via le Buffer 2
	static constexpr uint8_t WrtitePagesThroughBuf2BIE = 0x85;
	///Commande SPI FLASH pour effacer un page de m�moire
	static constexpr uint8_t PageErase = 0x81;
	///Commande SPI FLASH pour lire le num�ro d'identification du manifacturier de la flash
	static constexpr uint8_t DeviceID = 0x9F;
	///Commande SPI FLASH pour lire le registre d'�tat de la flash
	static constexpr uint8_t StatusRegisterRead = 0xD7;
	///Commande SPI FLASH pour lire le registre de configuration de la flash
	static constexpr uint8_t ConfigurationRegisterRead = 0x3F;
	///Commande SPI FLASH pour configurer les pages de m�moire en bloc de 512 Bytes
	static constexpr uint8_t BinaryPageSize[4] = { 0x3D, 0x2A, 0x80, 0xA6 };
	///Commande SPI FLASH pour formater la flash
	static constexpr uint8_t ChipErase[4] = { 0xC7, 0x94, 0x80, 0x9A };
	///Commande SPI FLASH pour d�sactiver la protection de secteurs
	static constexpr uint8_t DisableSectorProtect[4] =
			{ 0x3D, 0x2A, 0x7F, 0x9A };


	///Pages de m�moires alou�es au nom de fichiers. 14 noms * 16 bytes = 226 Bytes Total
	const address_t FilenamePage = { 9, 0 }; 								//14 images * 16 char = 226 bytes
	///M�moire allou�e pour la variable de GlogalBrightness du carrousel
	const address_t GlobalBrightnessSettingAddress = { 10, 0 };				//2 bytes
	///M�moire allou�e pour la variable de RedMaxCurrentSetting du carrousel
	const address_t RedMaxCurrentSettingAddress = { 10, 2 };				//2 bytes
	///M�moire allou�e pour la variable de GreenMaxCurrentSetting du carrousel
	const address_t GreenMaxCurrentSettingAddress = { 10, 4 };				//2 bytes
	///M�moire allou�e pour la variable de BlueMaxCurrentSetting du carrousel
	const address_t BlueMaxCurrentSettingAddress = { 10, 6 };				//2 Bytes
	///M�moire allou�e pour la variable de images pr�sentes dans le carrousel
	const address_t PositionOfPresentImagesInCarrouselAddress = { 10, 8 };	//2 bytes
	///D�verminage on ou off
	bool debug = false;
};

#endif /* FLASH_HPP_ */

