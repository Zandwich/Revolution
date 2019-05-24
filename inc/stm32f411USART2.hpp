/*
 * STM32F411USART2.hpp
 *
 *  Created on: Apr 24, 2019
 *      Author: rev
 */

#ifndef STM32F411USART2_HPP_
#define STM32F411USART2_HPP_

#include "stm32f4xx.h"
#include "buffer.hpp"

extern "C" void USART2_IRQHandler(void);

class STM32F411USART2 {
	friend void USART2_IRQHandler(void);
	bool isTransmitting = false;
	Buffer<uint8_t, 1024> rxBuffer;
	Buffer<uint8_t, 1024> txBuffer;
	STM32F411USART2();
	static STM32F411USART2 * instance;
public:
	/**
	 * M�thode static du singleton qui retourne l'instance de l'objet si elle est libre.
	 * @return Retourne l'instance de l'objet
	 */
	static STM32F411USART2* getInstance();
	/**
	 * Destructeur
	 */
	virtual ~STM32F411USART2();
	/**
	 * Envoie un byte dans le port de communication
	 * @param data � envoyer
	 */
	void write(uint8_t data);
	/**
	 * Envoie une s�rie de Bytes dans le port de communication
	 * @param data Tableau � envoyer
	 * @param nBytes nombre de byte � envoyer
	 */
	void sendBytes(uint8_t *data, uint32_t nBytes);
	/**
	 * Envoie la valeur d�cimal de la variable vers le port de communication
	 * @param byte Variable � envoyer
	 */
	void sendByteToString(uint32_t byte);
	/**
	 * Envoie la valeur binaire de la variable ver le port de communication
	 * @param data Variabl � envoyer
	 * Ex: 2 ---> 00000010
	 */
	void sendByte8ToBinaryString(uint8_t data);
	/**
	 * Envoie la valeur binaire de la variable ver le port de communication
	 * @param data Variabl � envoyer
	 * Ex: 2 ---> 0000000000000010
	 */
	void sendByte16ToBinaryString(uint16_t data);
	/**
	 * Envoie la valeur binaire de la variable ver le port de communication
	 * @param data Variabl � envoyer
	 * Ex: 2 ---> 00000000000000000000000000000010
	 */
	void sendByte32ToBinaryString(uint32_t data);
	/**
	 * Envoie un chaine de caract�re termin� par un Null vers le port de communication
	 * @param s La chainne de caract�res
	 */
	void sendString(const char *s);
	/**
	 * Envoie un chaine de caract�re termin� par un Null vers le port de communication
	 * @param s La chainne de caract�res
	 */
	void sendString(uint8_t *u);
	/**
	 * Lie la donn� re�u dans le port de communication
	 * @return
	 */
	uint8_t read();
	/**
	 * Retourne vrai lorsu'un donn�e est disponible dans le port de communication
	 * @return 1 lorsqu'un donn�e est disponible.
	 */
	bool dataAvailable();
	/**
	 * Configure la vitesse du port de communication
	 * @param baudrate
	 */
	/**
	 * Ajuste la vitesse de communication du port
	 * @param baudrate
	 */
	void setBaudRate(uint32_t baudrate);
	/**
	 * Active l'�cho de r�ception pour fin de d�verminage
	 * @param state 1 = on, 0 = off
	 */
	void setEcho(bool state);
};

#endif /* STM32F411USART2_HPP_ */

